
#ifndef DBQUERYMODEL_H
#define DBQUERYMODEL_H


#include "jsonqml/models/select_model.h"
#include "jsonio/dbquerybase.h"

namespace jsonio {
  class DBSchemaDocument;
}

namespace jsonqml {


/// \class DBQueryModel
/// \brief The DBQueryModel class provides a read-only data model for ArangoDB(noSQL) result sets.
///
/// DBQueryModel is a high-level interface for executing selection statements and traversing the result set.
/// It is built on top of the lower-level jsonio and can provide data to view classes such as QTableView.
class DBQueryModel: public SelectModel
{
    Q_OBJECT

signals:
    void queryChanged();


public slots:
    virtual void queryChange();

public:
    explicit DBQueryModel(QObject *parent = nullptr);
    virtual ~DBQueryModel();


    /// Execute the query \a query for the given database connection.
    /// uiSettings().lastError() can retrieve verbose information if there is an error in the query.
    virtual void setQuery(const jsonio::DBQueryBase& query,
                          const std::vector<std::string>& query_fields,
                          const jsonio::DBSchemaDocument* dbclient);

    /// Returns a reference to the const jsonio::DBQueryBase object associated with this model.
    const jsonio::DBQueryBase& query() const;
    /// Returns a reference to the query fields associated with this model.
    const jsonio::values_t& queryFields() const;


    Q_INVOKABLE QString lastQuery() const;
    Q_INVOKABLE QStringList lastQueryFields() const;

protected:
    jsonio::DBQueryBase data_query;

    void resetTable(std::vector<std::vector<std::string>>&& table_data,
                const std::vector<std::string>& header_data) override;

};

}

#endif // DBQUERYMODEL_H
