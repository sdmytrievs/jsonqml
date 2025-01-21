#include <QUrl>
#include <QDir>

#include "jsonqml/clients/settings_client.h"
#include "jsonqml/arango_database.h"
#include "arango-cpp/arangoconnect.h"
#include "jsonio/dbconnect.h"
#include "jsonio/dbdriverarango.h"

namespace jsonqml {
extern std::shared_ptr<spdlog::logger> ui_logger;


std::string ArangoDatabase::resources_database_name = "resources";

std::map<std::string, ArangoDatabase::CollectionData> ArangoDatabase::user_defined_collections=
{
    {"queries", {{"Query"}, {"name", "qschema", "comment"}}},
    {"docpages", {{"DocPages"}, {"type", "name", "ext"}}},
    {"impexdefs", {{"ImpexFormat"}, {"direction", "name", "impexschema", "schema", "comment"}}}
};
std::map<std::string, ArangoDatabase::CollectionData> ArangoDatabase::defined_collections=
        ArangoDatabase::user_defined_collections;


ArangoDatabase& arango_db()
{
    static  ArangoDatabase data;
    return data;
}

/// Private \class ArangoDatabase - monitoring database
class ArangoDatabasePrivate
{
    Q_DISABLE_COPY_MOVE(ArangoDatabasePrivate)

    friend class ArangoDatabase;
public:

    /// Constructor
    explicit ArangoDatabasePrivate();

    /// Destructor
    ~ArangoDatabasePrivate(){}

    void init();
    void save_db_settings(const std::string& db_group);
    void read_db_settings(const std::string& db_group);
    bool update_database();

    /// Current work Database exist
    bool db_connected() const
    {
        return (work_database.get()!=nullptr && work_database->connected());
    }

protected:

    /// Current database credentials group
    QString db_connect_current;

    /// The connection's database URL
    QString db_url;
    /// The connection's database name
    QString db_name;
    /// The connection's user name
    QString db_user;
    /// The connection's user password name
    QString db_user_password;
    /// Database access  "rw" (read&write) or "ro" (read-only if true)
    bool db_access;
    /// The flag to creating the empty database if they are not present
    /// Work if defined root credentials
    bool    db_create;

    /// Link to JsonioSettings - storing preferences to JSONIO
    jsonio::JsonioSettings& jsonio_settings;
    /// Link to jsonui settings in configuration data
    jsonio::SectionSettings jsonui_group;

    /// Current work Database
    std::shared_ptr<jsonio::DataBase> work_database;
    /// Current resourse Database
    std::shared_ptr<jsonio::DataBase> resourse_database;
    /// Current ArangoDB root client
    std::shared_ptr<arangocpp::ArangoDBRootClient> root_client;

    /// Current root database connection
    arangocpp::ArangoDBRootClient* root_connect() const
    {
        jsonio::JSONIO_THROW_IF(root_client.get()==nullptr, "Preferences", 12,
                                " the root database connection is not defined.");
        return root_client.get();
    }

    void update_work_database(bool create_if_noexist, const arangocpp::ArangoDBConnection& db_data);
    void update_resource_database(bool create_if_noexist, arangocpp::ArangoDBConnection db_data);
    void update_root_client(const std::string& db_group);
    void create_collection_if_no_exist(jsonio::AbstractDBDriver* db_driver);
    void get_system_lists(const std::string& db_group,
                          QStringList& db_all_databases,
                          QStringList& db_all_users);
};

ArangoDatabasePrivate::ArangoDatabasePrivate():
    jsonio_settings(jsonio::ioSettings()),
    jsonui_group(jsonio_settings.section(Preferences::jsonui_section_name)),
    work_database(nullptr),
    resourse_database(nullptr)
{
    jsonio_settings.set_module_level("jsonqml", "debug");
    init();
}

void ArangoDatabasePrivate::init()
{
    auto dbconnection = jsonui_group.value<std::string>("CurrentDBConnection","ArangoDBLocal");
    db_connect_current = QString::fromStdString(dbconnection);
    read_db_settings(dbconnection);
    update_database();
}

void ArangoDatabasePrivate::save_db_settings(const std::string& db_group)
{
    auto db_section =jsonio_settings.section(jsonio::arangodb_section(db_group));
    db_section.setValue("DB_URL", db_url.toStdString());
    db_section.setValue("DBName", db_name.toStdString());
    db_section.setValue("DBUser", db_user.toStdString());
    db_section.setValue("DBUserPassword", db_user_password.toStdString());
    db_section.setValue("DBAccess", (db_access? "ro" : "rw"));
    db_section.setValue("DBCreate", db_create);

    jsonio_settings.setValue(jsonio::arangodb_section("UseArangoDBInstance"), db_group);
    jsonui_group.setValue("CurrentDBConnection", db_group);
}

void ArangoDatabasePrivate::read_db_settings(const std::string& db_group)
{
    auto db_section =jsonio_settings.section(jsonio::arangodb_section(db_group));

    if(db_group.find("Local")!=std::string::npos) {
        db_url = QString::fromStdString(db_section.value("DB_URL", std::string(arangocpp::ArangoDBConnection::local_server_endpoint)));
        db_name = QString::fromStdString(db_section.value("DBName", std::string(arangocpp::ArangoDBConnection::local_server_database)));
        db_user = QString::fromStdString(db_section.value("DBUser", std::string(arangocpp::ArangoDBConnection::local_server_username)));
        db_user_password = QString::fromStdString(db_section.value("DBUserPassword", std::string(arangocpp::ArangoDBConnection::local_server_password)));
        auto access = db_section.value("DBAccess", std::string("rw"));
        db_access = (access=="ro");
        db_create = db_section.value("DBCreate", true);
    }
    else {
        db_url = QString::fromStdString(db_section.value("DB_URL", std::string(arangocpp::ArangoDBConnection::remote_server_endpoint)));
        db_name = QString::fromStdString(db_section.value("DBName", std::string(arangocpp::ArangoDBConnection::remote_server_database)));
        db_user = QString::fromStdString(db_section.value("DBUser", std::string(arangocpp::ArangoDBConnection::remote_server_username)));
        db_user_password = QString::fromStdString(db_section.value("DBUserPassword", std::string(arangocpp::ArangoDBConnection::remote_server_password)));
        auto access = db_section.value("DBAccess", std::string("ro"));
        db_access = (access=="ro");
        db_create = db_section.value("DBCreate", false);
    }
    ///emit q->dbNamesListChanged();
    ///emit q->dbUsersListChanged();
    ui_logger->debug("Changed db credentials to: {}", db_group);
}

//----------------------------------------------------------------------------

void ArangoDatabasePrivate::update_root_client(const std::string& db_group)
{
    auto  root_connect= jsonio::getFromSettings(jsonio_settings.section(jsonio::arangodb_section(db_group)), true);
    root_client.reset(new arangocpp::ArangoDBRootClient(root_connect));
    // To do: Check existence and true credentials, if error set nullptr
}

void ArangoDatabasePrivate::get_system_lists(const std::string& db_group,
                                             QStringList& db_all_databases,
                                             QStringList& db_all_users)
{
    update_root_client(db_group);
    auto users = root_connect()->userNames();
    std::transform(users.begin(), users.end(),
                   std::back_inserter(db_all_users),
                   [](const std::string &v){ return QString::fromStdString(v); });
    auto dbnames = root_connect()->databaseNames();
    std::transform(dbnames.begin(), dbnames.end(),
                   std::back_inserter(db_all_databases),
                   [](const std::string &v){ return QString::fromStdString(v); });
}

void ArangoDatabasePrivate::create_collection_if_no_exist(jsonio::AbstractDBDriver* db_driver)
{
    for(const auto& vertex_col: jsonio::DataBase::usedVertexCollections()) {
        db_driver->create_collection(vertex_col.second, "vertex");
    }
    for(const auto& edge_col: jsonio::DataBase::usedEdgeCollections()) {
        db_driver->create_collection(edge_col.second, "edge");
    }
}

void ArangoDatabasePrivate::update_resource_database(bool create_if_noexist, arangocpp::ArangoDBConnection db_data)
{
    try {
        db_data.databaseName = ArangoDatabase::resources_database_name;

        if(create_if_noexist && !root_connect()->existDatabase(db_data.databaseName)) {
            root_connect()->createDatabase(db_data.databaseName, {db_data.user});
        }

        auto db_driver = std::make_shared<jsonio::ArangoDBClient>(db_data);
        if(!db_driver->connected()) {
            resourse_database.reset();
            uiSettings().setError(QString::fromStdString(db_driver->status()));
            ui_logger->error(db_driver->status());
            return;
        }
        //db_driver->createCollection("impexdefs", "vertex");
        //db_driver->createCollection("queries", "vertex");
        //db_driver->createCollection(HelpMainWindow::help_collection_name, "vertex");

        if(resourse_database.get() == nullptr) {
            resourse_database.reset(new jsonio::DataBase(db_driver));
        }
        else {
            resourse_database->updateDriver(db_driver);
            if(!resourse_database->connected()) {
                ui_logger->warn(resourse_database->status());
            }
        }
    }
    catch(std::exception& e) {
        resourse_database.reset();
        throw;
    }
}

void ArangoDatabasePrivate::update_work_database(bool create_if_noexist, const arangocpp::ArangoDBConnection& db_data)
{
    try {
        if(create_if_noexist && !root_connect()->existDatabase(db_data.databaseName)) {
            root_connect()->createDatabase(db_data.databaseName, {db_data.user});
        }

        auto db_driver = std::make_shared<jsonio::ArangoDBClient>(db_data);
        if(!db_driver->connected()) {
            work_database.reset();
            uiSettings().setError(QString::fromStdString(db_driver->status()));
            ui_logger->error(db_driver->status());
            return;
        }

        create_collection_if_no_exist(db_driver.get());
        if(work_database.get() == nullptr) {
            work_database.reset(new jsonio::DataBase(db_driver));
        }
        else {
            work_database->updateDriver(db_driver);
        }
    }
    catch(std::exception& e) {
        work_database.reset();
        throw;
    }
}

bool ArangoDatabasePrivate::update_database()
{
    uiSettings().setError(QString());
    try {
        auto db_group = jsonui_group.value<std::string>("CurrentDBConnection","ArangoDBLocal");
        bool db_create = jsonio_settings.section(jsonio::arangodb_section(db_group)).value("DBCreate", false);
        auto db_connect = jsonio::getFromSettings(jsonio_settings.section(jsonio::arangodb_section(db_group)), false);

        if(work_database.get()!=nullptr) { // compare with old
            jsonio::ArangoDBClient* old_db_link = dynamic_cast<jsonio::ArangoDBClient*>(work_database->theDriver());
            if(old_db_link && db_connect==old_db_link->connect_data()) {
                return false;  // nothing changed
            }
        }

        if(db_create) { // try link as root
            update_root_client(db_group);
        }

        update_work_database(db_create, db_connect);
        update_resource_database(db_create, db_connect);

        ui_logger->info("Update database connect to: {} {}", db_connect.serverUrl, db_connect.databaseName);
        return true;
    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
        ui_logger->error("Exception when updating database {}", e.what());
    }
    return true; // clear connection
}


//----------------------------------------------------------------------------

ArangoDatabase::ArangoDatabase()
{
    impl_ptr.reset(new ArangoDatabasePrivate());
    qDebug() << "ArangoDatabase construct";
}

ArangoDatabase::~ArangoDatabase()
{
    qDebug() << "ArangoDatabase destruct";
}

bool ArangoDatabase::dbConnected() const
{
    return impl_func()->db_connected();
}

void ArangoDatabase::afterUpdatedDocument(std::string schema_name, std::string doc_id)
{
    Q_UNUSED(schema_name);
    Q_UNUSED(doc_id);
}

void ArangoDatabase::afterDeletedDocument(std::string schema_name, std::string doc_id)
{
    Q_UNUSED(schema_name);
    Q_UNUSED(doc_id);
}

void ArangoDatabase::ConnectTo(const QString &url, const QString &name,
                               const QString &user, const QString &user_passwd)
{

}

void ArangoDatabase::ConnectFromSettings()
{

}

QScopedPointer<ArangoDocument> ArangoDatabase::createDocument(DocumentType type, const QString &document_schema_name)
{

}

QScopedPointer<ArangoDocument> ArangoDatabase::createDocumentQuery(DocumentType type, const QString &document_schema_name, const jsonio::DBQueryBase &query, const std::vector<std::string> &query_fields)
{

}

QString ArangoDatabase::dbConnect()
{
    return impl_func()->db_connect_current;
}

QString ArangoDatabase::dbUrl()
{
    return impl_func()->db_url;
}

QString ArangoDatabase::dbName()
{
    return impl_func()->db_name;
}

QString ArangoDatabase::dbUser()
{
    return impl_func()->db_user;
}

QString ArangoDatabase::dbUserPassword()
{
    return impl_func()->db_user_password;
}

bool ArangoDatabase::dbAccess()
{
    return impl_func()->db_access;
}

bool ArangoDatabase::dbCreate()
{
    return impl_func()->db_create;
}

void ArangoDatabase::getRootLists(const std::string &db_group, QStringList &db_all_databases, QStringList &db_all_users)
{
    return impl_func()->get_system_lists(db_group, db_all_databases, db_all_users);
}


//void ArangoDatabase::changeDBConnect(const QString &db_group)
//{
//    setError(QString());
//    try {
//        if(Preferences::use_database) {
//            db_connect_current = db_group;
//            auto dbconnection = db_group.toStdString();
//            impl_func()->read_db_settings(dbconnection);
//            emit dbConnectChanged();
//        }
//    }
//    catch(std::exception& e) {
//        setError(e.what());
//    }
//}

//------------------------------------------------------------------

void ArangoDatabase::resetCollectionsList()
{
    defined_collections = user_defined_collections;
    for(const auto& collection: jsonio::DataBase::usedVertexCollections()) {
        defined_collections[collection.second] = {collection.first, make_query_fields(collection.first)};
    }
    for(const auto& collection: jsonio::DataBase::usedEdgeCollections()) {
        defined_collections[collection.second] = {collection.first, make_query_fields(collection.first)};
    }
}

jsonio::values_t ArangoDatabase::make_query_fields(std::string schema_name) const
{
    jsonio::values_t key_fields = {"_label", "_id"};
    const jsonio::StructDef* schema_struct=jsonio::ioSettings().Schema().getStruct(schema_name);

    if(schema_struct != nullptr) {
        auto ids_or_names = schema_struct->getSelectedList();
        if(!ids_or_names.empty()) {
            key_fields.clear();
            key_fields.push_back("_label");
            for(const auto& id_or_key: ids_or_names) {
                auto pos = id_or_key.find_first_not_of("0123456789.");
                if(pos == std::string::npos)  {
                    auto ids = jsonio::split_int(id_or_key, ".");
                    auto the_name = schema_struct->getPathFromIds(ids);
                    key_fields.push_back(the_name);
                }
                else  {
                    key_fields.push_back( id_or_key );
                }
            }
        }
    }
    return key_fields;
}

QStringList ArangoDatabase::getEdgesList()
{
    auto std_schema_list = jsonio::DataBase::getEdgesList();
    QStringList new_list;
    //new_list.append(no_schema_name);
    std::transform(std_schema_list.begin(), std_schema_list.end(),
                   std::back_inserter(new_list),
                   [](const std::string &v){ return QString::fromStdString(v); });
    return new_list;
}

QStringList ArangoDatabase::getVertexesList()
{
    auto std_schema_list = jsonio::DataBase::getVertexesList();
    QStringList new_list;
    //new_list.append(no_schema_name);
    std::transform(std_schema_list.begin(), std_schema_list.end(),
                   std::back_inserter(new_list),
                   [](const std::string &v){ return QString::fromStdString(v); });
    return new_list;
}

std::string ArangoDatabase::collectionFromSchema(const std::string &schema_name)
{
    for(const auto& item: defined_collections) {
        if(item.second.schema_name==schema_name) {
            return item.first;
        }
    }
    return "";
}

std::string ArangoDatabase::schemaFromCollection(const std::string &collection_name)
{
    auto it = defined_collections.find(collection_name);
    if(it==defined_collections.end()) {
        return "";
    }
    else {
        return it->second.schema_name;
    }
}

jsonio::values_t ArangoDatabase::fieldsFromCollection(const std::string &collection_name)
{
    auto it = defined_collections.find(collection_name);
    if(it==defined_collections.end()) {
        return {};
    }
    else {
        return it->second.default_query_fields;
    }
}

}
