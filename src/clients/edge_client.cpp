
#include "edge_client_p.h"
#include "jsonqml/clients/settings_client.h"
#include "jsonqml/models/schema_model.h"
#include "jsonqml/models/db_keys_model.h"
#include "jsonio/io_settings.h"

namespace jsonqml {

EdgeClientPrivate::EdgeClientPrivate():
    VertexClientPrivate(),
    dbedge_collection(nullptr),
    in_model(nullptr),
    out_model(nullptr)
{

}

void EdgeClientPrivate::init()
{
    uiSettings();
    qDebug() << "EdgeClientPrivate::init";
    schema_names_list = gen_schema_list();
    if(schema_names_list.empty()) { // not defined schema data
        return;
    }
    // set first schema as inital
    update_schema(new_list_default_schema());

    // alloc editor model
    json_tree_model.reset(new JsonSchemaModel(current_schema_name, header_names));
    in_model.reset(new DBQueryModel());
    out_model.reset(new DBQueryModel());
    if(uiSettings().dbConnected()) {
        jsonio::DBVertexDocument* new_vclient = jsonio::DBVertexDocument::newVertexDocument(
                    uiSettings().database(), jsonio::DataBase::getVertexesList()[0]);  //no query query
        dbcollection.reset(new_vclient);
    }

    // define keys model
    update_keysmodel();

    // try read first record to editor
    read_editor_data(0);
}

QStringList EdgeClientPrivate::gen_schema_list() const
{
    auto std_schema_list = jsonio::DataBase::getEdgesList();
    QStringList new_list;
    //new_list.append(no_schema_name);
    std::transform(std_schema_list.begin(), std_schema_list.end(),
                   std::back_inserter(new_list),
                   [](const std::string &v){ return QString::fromStdString(v); });
    return new_list;
}

bool EdgeClientPrivate::update_schema(QString new_schema)
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

void EdgeClientPrivate::update_jsonmodel()
{
    if(json_tree_model) {
        json_tree_model->setupModelData("", current_schema_name);
    }
}

void EdgeClientPrivate::update_keysmodel()
{
    change_schema_mode = false;
    if(!uiSettings().dbConnected()) {
        dbedge_collection.reset();
        sort_proxy_model.reset();
        keys_model.reset();
        qDebug() << "update_keysmodel no connection";
    }
    else {
        if(keys_model.get()==nullptr || dbedge_collection.get()==nullptr) {
            auto document_schema_name = current_schema_name.toStdString();
            jsonio::DBEdgeDocument* new_client = jsonio::DBEdgeDocument::newEdgeDocumentQuery(
                        uiSettings().database(), document_schema_name);  //default query
            dbedge_collection.reset(new_client);
            keys_model.reset(new DBKeysModel(dbedge_collection));
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

void EdgeClientPrivate::read_editor_data(int row)
{
    if(row>=0 && sorting_enabled() && sort_proxy_model) {
        row = sort_proxy_model->mapToSource(sort_proxy_model->index(row,0)).row();
    }

    if(row>=0 && keys_model && keys_model->rowCount()>row) {
        auto jsondata = keys_model->read(row);
        auto edge_schema_from_id = dbedge_collection->extractSchemaFromId(keys_model->get_id(row));
        // if change_schema_mode true could be different schema name
        set_json(std::move(jsondata), QString::fromStdString(edge_schema_from_id));
    }
    else {
        set_json("", current_schema_name);
    }
}

void EdgeClientPrivate::set_edges_for(jsonio::DBQueryBase &&query)
{
    if(!dbedge_collection || !keys_model) {
       return;
    }
    change_schema_mode = true;
    dbedge_collection->setMode(change_schema_mode);
    keys_model->setQuery(query, all_edges_fields);
}

jsonio::DBQueryBase EdgeClientPrivate::make_vertex_query(std::string vertex_id) const
{
    return jsonio::DBQueryBase("RETURN DOCUMENT(\"" + vertex_id +"\")", jsonio::DBQueryBase::qAQL);
}

std::vector<std::string> EdgeClientPrivate::make_vertex_query_fields(std::string vertex_id) const
{
    std::vector<std::string> key_fields = all_vertex_fields;
    auto schema_from_id = dbcollection->extractSchemaFromId(vertex_id);
    const jsonio::StructDef* schema_struct=jsonio::ioSettings().Schema().getStruct(schema_from_id);

    if(schema_struct != nullptr) {
        std::vector<std::string> ids_or_names = schema_struct->getSelectedList();
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

bool EdgeClientPrivate::set_json(const std::string& json_string, const QString& schema_name)
{
    if(json_tree_model) {
        json_tree_model->setupModelData(json_string, schema_name);
        json_tree_model->current_object().get_value_via_path<std::string>("_to", out_vertex_id, "");
        json_tree_model->current_object().get_value_via_path<std::string>("_from", in_vertex_id, "");
        if(dbcollection) {
            in_model->setQuery(make_vertex_query(in_vertex_id), make_vertex_query_fields(in_vertex_id), dbcollection.get());
            out_model->setQuery(make_vertex_query(out_vertex_id), make_vertex_query_fields(out_vertex_id), dbcollection.get());
        }
    }
    return true;
}


//---------------------------------------------------------------------

EdgeClient::EdgeClient(EdgeClientPrivate *impl, QObject *parent):
    VertexClient(impl, parent)
{
}

EdgeClient::EdgeClient(QObject *parent):
    EdgeClient(new EdgeClientPrivate(), parent)
{
    // virtual init into JsonClient
}

QAbstractItemModel *EdgeClient::inmodel()
{
    return impl_func()->in_model.get();
}

QAbstractItemModel *EdgeClient::outmodel()
{
    return impl_func()->out_model.get();
}

void jsonqml::EdgeClient::setIncomingEdges(QString vertex_id)
{
    if(!vertex_id.isEmpty()) {
        impl_func()->set_edges_for(jsonio::DBVertexDocument::inEdgesQuery(vertex_id.toStdString(), ""));
        emit keysModelChanged();
    }
}

void jsonqml::EdgeClient::setOutgoingEdges(QString vertex_id)
{
    if(!vertex_id.isEmpty()) {
        impl_func()->set_edges_for(jsonio::DBVertexDocument::outEdgesQuery(vertex_id.toStdString(), ""));
        emit keysModelChanged();
    }
}

QString EdgeClient::incomingVertex() const
{
    return QString::fromStdString(impl_func()->in_vertex_id);
}

QString EdgeClient::outgoingVertex() const
{
    return QString::fromStdString(impl_func()->out_vertex_id);
}

}
