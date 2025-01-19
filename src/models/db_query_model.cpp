
#include "jsonqml/models/db_query_model.h"
#include "jsonqml/clients/settings_client.h"
#include "jsonio/dbschemadoc.h"

namespace jsonqml {


DBQueryModel::DBQueryModel(QObject *parent)
    : SelectModel(parent)
{
}

DBQueryModel::~DBQueryModel()
{
}

/*!
    This virtual slot is called whenever the query changes. The
    default implementation does nothing.
 */
void DBQueryModel::queryChange()
{
    emit queryChanged();
}

const jsonio::DBQueryBase &DBQueryModel::query() const
{
    return  data_query;
}

const jsonio::values_t &DBQueryModel::queryFields() const
{
    return header;
}

QString DBQueryModel::lastQuery() const
{
    return QString::fromStdString(data_query.queryString());
}

QStringList DBQueryModel::lastQueryFields() const
{
    QStringList new_list;
    std::transform(header.begin(), header.end(),
                   std::back_inserter(new_list),
                   [](const std::string &v){ return QString::fromStdString(v); });
    return new_list;
}

void DBQueryModel::setQuery(const jsonio::DBQueryBase& query,
                            const std::vector<std::string>& query_fields,
                            const jsonio::DBSchemaDocument* dbclient)
{
    if(dbclient==nullptr) {
        uiSettings().setError(QString("Do not defined database client"));
        return;
    }

    uiSettings().setError(QString());
    jsonio::values_table_t new_data_table;

    try {
        new_data_table = dbclient->downloadDocuments(query, query_fields);
        data_query = query;
        resetTable(std::move(new_data_table), query_fields);
        queryChange();
    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
    }
}


void DBQueryModel::resetTable(std::vector<std::vector<std::string>>&& table_data,
                              const std::vector<std::string>& header_data)
{
    beginResetModel();
    table = std::move(table_data);
    header = header_data;
    endResetModel();
}

}
