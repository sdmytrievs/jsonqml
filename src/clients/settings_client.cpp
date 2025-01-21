#include <QUrl>
#include <QDir>

#include "jsonqml/clients/settings_client.h"
#include "jsonqml/models/schema_model.h"
#include "jsonqml/arango_database.h"
#include "arango-cpp/arangoconnect.h"
#include "jsonio/io_settings.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace jsonqml {

std::string Preferences::jsonui_section_name = "jsonui";
bool Preferences::use_database = true;
bool Preferences::use_schemas = true;

// Thread-safe logger to stdout with colors
std::shared_ptr<spdlog::logger> ui_logger = spdlog::stdout_color_mt("jsonqml");


Preferences& uiSettings()
{
    static  Preferences data;
    return data;
}

/// Private \class Preferences - monitoring preferences into JSONIO and database
class PreferencesPrivate
{
    Q_DISABLE_COPY_MOVE(PreferencesPrivate)

public:

    /// Constructor
    explicit PreferencesPrivate(Preferences* );

    /// Destructor
    ~PreferencesPrivate(){}

    void init();
    void save_db_settings(const std::string& db_group);
    void read_db_settings(const std::string& db_group);
    void save_other_settings();
    void read_other_settings();
    bool change_schemas_path(const std::string& path);
    void set_user_dir(const std::string& path);
    bool update_database();

protected:

    Preferences* base_ptr;
    Preferences* base_func() { return static_cast<Preferences*>(base_ptr); }
    const Preferences* base_func() const { return static_cast<const Preferences *>(base_ptr); }

    /// Link to JsonioSettings - storing preferences to JSONIO
    jsonio::JsonioSettings& jsonio_settings;
    /// Link to jsonui settings in configuration data
    jsonio::SectionSettings jsonui_group;

    void update_system_lists();
    void apply_changes_to_static();
};

PreferencesPrivate::PreferencesPrivate(Preferences* parent):
    base_ptr(parent),
    jsonio_settings(jsonio::ioSettings()),
    jsonui_group(jsonio_settings.section(Preferences::jsonui_section_name))
{
    jsonio_settings.set_module_level("jsonqml", "debug");
    init();

}

void PreferencesPrivate::init()
{
    if(Preferences::use_schemas) {
        read_other_settings();
    }
    if(Preferences::use_database) {
        auto dbconnection = jsonui_group.value<std::string>("CurrentDBConnection","ArangoDBLocal");
        base_func()->db_connect_current = QString::fromStdString(dbconnection);
        read_db_settings(dbconnection);
        //update_database();
    }
}

bool PreferencesPrivate::change_schemas_path(const std::string& path)
{
    jsonio_settings.setValue<std::string>(jsonio::common_section("SchemasDirectory"), path);
    if(jsonio_settings.updateSchemaDir()) {
        ui_logger->debug("Changed schemas path to: {}", path);
        return true;
    }
    return false;
}

void PreferencesPrivate::set_user_dir(const std::string& path)
{
    jsonio_settings.setUserDir(path);
}

void PreferencesPrivate::apply_changes_to_static()
{
    // load main programm settingth
    JsonSchemaModel::showComments = jsonui_group.value("ShowComments", JsonSchemaModel::showComments);
    JsonSchemaModel::useEnumNames = jsonui_group.value("ShowEnumNames", JsonSchemaModel::useEnumNames);
    JsonSchemaModel::editID = jsonui_group.value("CanEdit_id", JsonSchemaModel::editID);
    //JsonView::expandedFields = jsonui_group.value("KeepExpanded", JsonView::expandedFields);
    //HelpMainWindow::editHelp = jsonui_group.value( "CanEditDocPages", HelpMainWindow::editHelp );
    ui_logger->debug("Applied changes to static");
}

void PreferencesPrivate::save_other_settings()
{
    const auto q = base_func();
    jsonio_settings.setValue(jsonio::common_section("ResourcesDirectory"), q->resources_directory.toStdString());
    jsonio_settings.setValue(jsonio::common_section("SchemasDirectory"), q->schemas_directory.toStdString());
    jsonio_settings.setUserDir(q->work_directory.toStdString());

    jsonui_group.setValue("ShowComments", q->show_comments);
    jsonui_group.setValue("ShowEnumNames", q->show_enumnames);
    jsonui_group.setValue("CanEdit_id", q->can_edit_id);
    jsonui_group.setValue("KeepExpanded", q->keep_expanded);
    jsonui_group.setValue("CanEditDocPages", q->can_edit_doc_pages);

    apply_changes_to_static();
}

void PreferencesPrivate::save_db_settings(const std::string& db_group)
{
    const auto q = base_func();
    auto db_section =jsonio_settings.section(jsonio::arangodb_section(db_group));
    db_section.setValue("DB_URL", q->db_url.toStdString());
    db_section.setValue("DBName", q->db_name.toStdString());
    db_section.setValue("DBUser", q->db_user.toStdString());
    db_section.setValue("DBUserPassword", q->db_user_password.toStdString());
    db_section.setValue("DBAccess", (q->db_access? "ro" : "rw"));
    db_section.setValue("DBCreate", q->db_create);

    jsonio_settings.setValue(jsonio::arangodb_section("UseArangoDBInstance"), db_group);
    jsonui_group.setValue("CurrentDBConnection", db_group);
}

void PreferencesPrivate::read_other_settings()
{
    auto q = base_func();
    q->resources_directory = QString::fromStdString(jsonio_settings.value<std::string>(jsonio::common_section("ResourcesDirectory"), ""));
    q->schemas_directory = QString::fromStdString(jsonio_settings.value<std::string>(jsonio::common_section("SchemasDirectory" ), ""));
    q->work_directory = QString::fromStdString(jsonio_settings.value<std::string>(jsonio::common_section("WorkDirectoryPath" ), "."));

    q->show_comments = jsonui_group.value("ShowComments", JsonSchemaModel::showComments);
    q->show_enumnames = jsonui_group.value("ShowEnumNames", JsonSchemaModel::useEnumNames);
    q->can_edit_id = jsonui_group.value("CanEdit_id", JsonSchemaModel::editID);
    q->keep_expanded = jsonui_group.value("KeepExpanded", true); // JsonView::expandedFields
    q->can_edit_doc_pages = jsonui_group.value("CanEditDocPages", false); // HelpMainWindow::editHelp

    apply_changes_to_static();
}

void PreferencesPrivate::read_db_settings(const std::string& db_group)
{
    auto q = base_func();
    auto db_section =jsonio_settings.section(jsonio::arangodb_section(db_group));

    if(db_group.find("Local")!=std::string::npos) {
        q->db_url = QString::fromStdString(db_section.value("DB_URL", std::string(arangocpp::ArangoDBConnection::local_server_endpoint)));
        q->db_name = QString::fromStdString(db_section.value("DBName", std::string(arangocpp::ArangoDBConnection::local_server_database)));
        q->db_user = QString::fromStdString(db_section.value("DBUser", std::string(arangocpp::ArangoDBConnection::local_server_username)));
        q->db_user_password = QString::fromStdString(db_section.value("DBUserPassword", std::string(arangocpp::ArangoDBConnection::local_server_password)));
        auto access = db_section.value("DBAccess", std::string("rw"));
        q->db_access = (access=="ro");
        q->db_create = db_section.value("DBCreate", true);
    }
    else {
        q->db_url = QString::fromStdString(db_section.value("DB_URL", std::string(arangocpp::ArangoDBConnection::remote_server_endpoint)));
        q->db_name = QString::fromStdString(db_section.value("DBName", std::string(arangocpp::ArangoDBConnection::remote_server_database)));
        q->db_user = QString::fromStdString(db_section.value("DBUser", std::string(arangocpp::ArangoDBConnection::remote_server_username)));
        q->db_user_password = QString::fromStdString(db_section.value("DBUserPassword", std::string(arangocpp::ArangoDBConnection::remote_server_password)));
        auto access = db_section.value("DBAccess", std::string("ro"));
        q->db_access = (access=="ro");
        q->db_create = db_section.value("DBCreate", false);
    }

    // ask root client arango_db to refresh lists for new settings group,
    try {  // try generate list of all databases
        q->db_all_databases.clear();
        q->db_all_users.clear();
        if(q->db_create) {
            arango_db.getRootLists(db_group, q->db_all_databases, q->db_all_users);
        }
    }
    catch(std::exception& e) {
        ui_logger->warn("Error connection as root to host: {}", e.what());
    }

    if(q->db_all_databases.indexOf(q->db_name)<0) {
        q->db_all_databases.append(q->db_name);
    }
    if(q->db_all_users.indexOf(q->db_user)<0) {
        q->db_all_users.append(q->db_user);
    }
    emit q->dbNamesListChanged();
    emit q->dbUsersListChanged();
    ui_logger->debug("Changed db credentials to: {}", db_group);
}


//----------------------------------------------------------------------------

Preferences::Preferences()
{
    setDBConnectList();
    impl_ptr.reset(new PreferencesPrivate(this));
    qDebug() << "Preferences construct";
}

Preferences::~Preferences()
{
    qDebug() << "Preferences destruct";
}

void Preferences::setDBConnectList()
{
    db_connect_list = {"ArangoDBLocal", "ArangoDBRemote"};
    // To do: restore list from settings
}

bool Preferences::applyChanges()
{
    auto d = impl_func();
    setError(QString());
    try {
        emit settingsChanged();
        if(Preferences::use_schemas) {
            d->save_other_settings();
            if(d->change_schemas_path(schemas_directory.toStdString())) {
                emit scemasPathChanged();
            }
        }
        if(Preferences::use_database) {
            auto dbconnection = db_connect_current.toStdString();
            d->save_db_settings(dbconnection);
            signal to DB to update from settings
            if(d->update_database()) {

                emit dbdriveChanged();
            }
            return dbConnected();
        }
        return true;
    }
    catch(std::exception& e) {
        setError(e.what());
    }
    return false;
}

void Preferences::addDBName(const QString &new_name)
{
    db_all_databases.append(new_name);
    emit dbNamesListChanged();
}

void Preferences::addDBUser(const QString &new_user)
{
    db_all_users.append(new_user);
    emit dbUsersListChanged();
}

QString Preferences::lastError() const
{
    return err_message;
}

/*!
   Set the value of the last error that occurred end emit signal about error.
*/
void Preferences::setError(const QString& error)
{
    qDebug() << "Error " << error;
    err_message = error;
    emit errorChanged();
}

void Preferences::changeDBConnect(const QString &db_group)
{
    setError(QString());
    try {
        if(Preferences::use_database) {
            db_connect_current = db_group;
            auto dbconnection = db_group.toStdString();
            impl_func()->read_db_settings(dbconnection);
            emit dbConnectChanged();
        }
    }
    catch(std::exception& e) {
        setError(e.what());
    }
}

void Preferences::changeScemasPath(const QString &url)
{
    if(url.isEmpty()) {
        return;
    }
    setError(QString());
    try {
        auto path = handleFileChosen(url);
        if(impl_func()->change_schemas_path(path.toStdString())) {
            schemas_directory = path;
            emit scemasPathChanged();
        }
    }
    catch(std::exception& e) {
        setError(e.what());
    }
}

QString Preferences::handleFileChosen(const QString &urls)
{
    QString abs_path;
    const QUrl url(urls);
    if (url.isLocalFile()) {
        abs_path = QDir::toNativeSeparators(url.toLocalFile());
    } else {
        abs_path = urls;
    }
    if(url.isLocalFile() || url.isRelative()) {
        setWorkPath(abs_path); // save last used dir
    }
    return abs_path;
}

QUrl Preferences::workDir() const
{
    return QUrl::fromLocalFile(work_directory);
}

void Preferences::setWorkPath(const QString &path)
{
    QFileInfo flinfo(path);
    work_directory = flinfo.dir().path();
    impl_func()->set_user_dir(work_directory.toStdString());
    emit workDirChanged();
}

}
