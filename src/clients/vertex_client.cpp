
#include "vertex_client_p.h"
#include "jsonqml/clients/settings_client.h"
#include "jsonqml/models/schema_model.h"
#include "jsonqml/models/db_keys_model.h"


namespace jsonqml {


VertexClientPrivate::VertexClientPrivate():
    JsonClientPrivate(),
    dbcollection(nullptr),
    keys_model(nullptr),
    sort_proxy_model(nullptr)
{

}

void VertexClientPrivate::init()
{
    uiSettings();
    qDebug() << "VertexClientPrivate::init";
    schema_names_list = gen_schema_list();
    if(schema_names_list.empty()) { // not defined schema data
        // try use  DBJsonDocument and JsonFree editor
        return;
    }
    // set first schema as inital
    update_schema(new_list_default_schema());

    // alloc editor model
    json_tree_model.reset(new JsonSchemaModel(current_schema_name, header_names));

    // define keys model
    update_keysmodel();

    // try read first record to editor
    read_editor_data(0);
}

QStringList VertexClientPrivate::gen_schema_list() const
{
    auto std_schema_list = jsonio::DataBase::getVertexesList();
    QStringList new_list;
    //new_list.append(no_schema_name);
    std::transform(std_schema_list.begin(), std_schema_list.end(),
                   std::back_inserter(new_list),
                   [](const std::string &v){ return QString::fromStdString(v); });
    return new_list;
}

QAbstractItemModel *VertexClientPrivate::keys_list_model() const
{
    if(sorting_enabled()) {
        return sort_proxy_model.get();
    }
    return keys_model.get();
}

bool VertexClientPrivate::sorting_enabled() const
{
    return keys_table_mode&RowSortingEnabled;
}

bool VertexClientPrivate::update_schema(QString new_schema)
{
    if(!schema_names_list.contains(new_schema)) {
        new_schema = no_schema_name;
    }
    if(current_schema_name != new_schema) {
        current_schema_name = new_schema;
        return true;
    }
    return false;
}

void VertexClientPrivate::update_jsonmodel()
{
    if(json_tree_model) {
        json_tree_model->setupModelData("", current_schema_name);
    }
}

void VertexClientPrivate::update_keysmodel()
{
    if(!uiSettings().dbConnected()) {
        dbcollection.reset();
        sort_proxy_model.reset();
        keys_model.reset();
        qDebug() << "update_keysmodel no connection";
    }
    else {
        if(keys_model.get()==nullptr || dbcollection.get()==nullptr) {
            auto document_schema_name = current_schema_name.toStdString();
            jsonio::DBVertexDocument* new_client = jsonio::DBVertexDocument::newVertexDocumentQuery(
                        uiSettings().database(), document_schema_name);  //default query
            dbcollection.reset(new_client);
            keys_model.reset(new DBKeysModel(dbcollection));
        }
        else {
            if(keys_model->get_schema()!=current_schema_name) {
                keys_model->resetSchema(current_schema_name);
            }
            else {
                keys_model->updateQuery();
            }
        }
        if(sorting_enabled()) {
            sort_proxy_model.reset(new SortFilterProxyModel());
            sort_proxy_model->setSourceModel(keys_model.get());
        }
        qDebug() << "update_keysmodel" << keys_model->rowCount() << " " << keys_model->columnCount();
    }
}

void VertexClientPrivate::read_editor_data(int row)
{
    if(row>=0 && sorting_enabled() && sort_proxy_model) {
        row = sort_proxy_model->mapToSource(sort_proxy_model->index(row,0)).row();
    }

    if(row>=0 && keys_model && keys_model->rowCount()>row) {
        auto jsondata = keys_model->read(row);
        set_json(std::move(jsondata), keys_model->get_schema());
    }
    else {
        set_json("", current_schema_name);
    }
}

void VertexClientPrivate::read_editor_id(const std::string& vertex_id)
{
    if(!dbcollection) {
       return;
    }
    // could move for model or db
    // if use query do not need change current schema
    uiSettings().setError(QString());
    try  {
        auto schema_from_id = dbcollection->extractSchemaFromId(vertex_id);
        auto id_query = jsonio::DBQueryBase("RETURN DOCUMENT(\"" + vertex_id +"\")", jsonio::DBQueryBase::qAQL);
        auto result = dbcollection->selectQuery(id_query);
        if(result.size()>0) {
            if(json_tree_model) {
                json_tree_model->setupModelData(result[0], QString::fromStdString(schema_from_id));
            }
        }
    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
    }
}

std::string VertexClientPrivate::id_from_editor()
{
    std::string vertex_id;
    json_tree_model->current_object().get_value_via_path("_id", vertex_id, vertex_id);
    return vertex_id;
}

bool VertexClientPrivate::set_json(const std::string& json_string, const QString& schema_name)
{
    if(!no_schema_model(current_schema_name) &&
            !no_schema_model(schema_name) &&
            schema_name!=current_schema_name) {
        return false;
    }
    update_schema(schema_name);
    if(json_tree_model) {
        json_tree_model->setupModelData(json_string, current_schema_name);
    }
    return true;
}


//-------------------------------------------------------------------------

VertexClient::VertexClient(VertexClientPrivate *impl, QObject *parent):
    JsonClient(impl, parent)
{
}

VertexClient::VertexClient(QObject *parent):
    VertexClient(new VertexClientPrivate(), parent)
{
    // virtual init into JsonClient
    QObject::connect(&uiSettings(), &Preferences::dbdriveChanged, this, &VertexClient::changeDBConnection);
    connect(this, &VertexClient::keysModelChanged, this, [this]{ readEditorData(0); });
}

void VertexClient::setModelSchema()
{
    impl_func()->update_jsonmodel();
    emit jsonModelChanged();

    impl_func()->update_keysmodel();
    emit keysModelChanged();
}

void VertexClient::changeDBConnection()
{
    impl_func()->update_keysmodel();
    emit keysModelChanged();
}

void VertexClient::readEditorData(int row)
{
    impl_func()->read_editor_data(row);
}

void VertexClient::readEditorId(QString vertex_id)
{
    impl_func()->read_editor_id(vertex_id.toStdString());
}

QString VertexClient::editorId() {
   return QString::fromStdString(impl_func()->id_from_editor());
}

QAbstractItemModel *VertexClient::keysmodel()
{
    return impl_func()->keys_list_model();
}

bool VertexClient::sortingEnabled()
{
    return impl_func()->sorting_enabled();
}

}
