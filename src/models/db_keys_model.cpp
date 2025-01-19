
#include "jsonqml/models/db_keys_model.h"
#include "jsonqml/clients/settings_client.h"


namespace jsonqml {


/*!
    Creates an empty DBKeysModel with the given \a parent.
 */
DBKeysModel::DBKeysModel(std::shared_ptr<jsonio::DBSchemaDocument> db_client,
                         QObject *parent)
     :DBQueryModel(parent),
      dbclient(std::move(db_client))
{
    build_table(table, header);
}


DBKeysModel::~DBKeysModel()
{
}

void DBKeysModel::updateKeyList()
{
    std::vector<std::string> new_query_fields;
    jsonio::values_table_t new_data_table;

    build_table(new_data_table, new_query_fields);
    resetTable(std::move(new_data_table), std::move(new_query_fields));
}

void DBKeysModel::updateQuery()
{
    uiSettings().setError(QString());
    try {
        dbclient->updateQuery();
        updateKeyList();
    }
    catch(std::exception& e) {
       uiSettings().setError(e.what());
    }
}

void DBKeysModel::resetSchema(QString new_schema_name)
{
    uiSettings().setError(QString());
    try {
        std::string new_schema = new_schema_name.toStdString();
        if(new_schema!=dbclient->getSchemaName())  {
            dbclient->resetSchema(new_schema, true);
            // updateQuery(); into reset schema
            updateKeyList();
            queryChange();
        }
    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
    }
}

/*!
    Execute the query \a query for the given database connection \a
    db.

    lastError() can be used to retrieve verbose information if there
    was an error setting the query.
*/
void DBKeysModel::setQuery(const jsonio::DBQueryBase& query,
                           const std::vector<std::string>& query_fields,
                           const jsonio::DBSchemaDocument*)
{
    uiSettings().setError(QString());
    try {
        dbclient->setQuery(query, query_fields);
        updateKeyList();
        queryChange();
    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
    }
}

QString DBKeysModel::get_schema() const
{
    return QString::fromStdString(dbclient->getSchemaName());
}

void DBKeysModel::updateQuery(const jsonio::DBQueryBase &aquery)
{
    setQuery(aquery, queryFields(), nullptr);
}

void DBKeysModel::updateFields(const std::vector<std::string> &query_fields)
{
    setQuery(query(), query_fields, nullptr);
}

void DBKeysModel::updateQuery(const QString &query)
{
    updateQuery(jsonio::DBQueryBase(query.toStdString(), jsonio::DBQueryBase::qAQL));
}

void DBKeysModel::updateFields(const QStringList &query_fields)
{
    std::vector<std::string> new_list;
    std::transform(query_fields.begin(), query_fields.end(),
                   std::back_inserter(new_list),
                   [](const QString &v){ return v.toStdString(); });
    updateFields(std::move(new_list));
}


void DBKeysModel::build_table(std::vector<std::vector<std::string>>& table_data,
                              std::vector<std::string>& header_data)
{
    data_query = dbclient->currentQueryResult().condition();
    header_data = dbclient->currentQueryResult().query().fields();
    header_data.insert(header_data.begin(), "key");

    for(const auto& line: dbclient->currentQueryResult().queryResult())  {
        table_data.push_back(line.second);
        table_data.back().insert(table_data.back().begin(), line.first);
    }
}

std::string DBKeysModel::get_id(size_t row) const
{
    if(row<table.size()) {
        return table[row][0];
    }
    return {};
}

std::string DBKeysModel::read(size_t row) const
{
    auto doc_id = get_id(row);
    if(doc_id.empty()) {
        return {};
    }
    uiSettings().setError(QString());
    try{
        dbclient->readDocument(doc_id);
        return dbclient->getJson();
    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
    }
}

void DBKeysModel::save(const std::string &json_data)
{
    uiSettings().setError(QString());
    try  {
        std::string  key = dbclient->recFromJson(json_data, false);
        if(!dbclient->existsDocument(key)) {
            key = dbclient->createDocument(key);
            // ask editor to set new id
            emit updatedOid(key);
        }
        else   {
            dbclient->updateDocument(key);
            // ask other models to update line with key
            // all was did by dbclient, but need refresh models
            emit updatedDocument(dbclient->getSchemaName(), key);
        }
        updateKeyList(); // !!! now reset all model, posible check only one row

    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
    }
}

void DBKeysModel::remove(size_t row)
{
    uiSettings().setError(QString());
    auto doc_id = get_id(row);
    if(doc_id.empty()) {
        return;
    }
    try{
        dbclient->deleteDocument(doc_id);
        // ask other models to remove line, and to remove deleted edges
        // all was did by dbclient, but need refresh models
        emit deletedDocument(dbclient->getSchemaName(), doc_id);
        beginRemoveRows(QModelIndex(), row, row);
        table.erase(table.begin()+row);
        endRemoveRows();
    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
    }
}


}
