
#ifndef DKEYSMODEL_H
#define DKEYSMODEL_H

#include "jsonqml/models/db_query_model.h"
#include "jsonio/dbedgedoc.h"

namespace jsonqml {

/// \class DBKeysModel
/// \brief The DBKeysModel class provides a read-only data model for ArangoDB(noSQL) collection data sets.
///
/// DBKeysModel is a high-level interface for executing selection statements and traversing the result set.
/// It is built on top of the lower-level jsonio and can provide data to view classes such as QTableView.
class DBKeysModel: public DBQueryModel
{
    Q_OBJECT

signals:
   // need for vertexes
   void deletedDocument(std::string schema_name, std::string doc_id);
   void updatedDocument(std::string schema_name, std::string doc_id);
   void updatedOid(std::string doc_id);

public slots:
    /// Refresh  documents keys list for table
    void updateKeyList();
    void updateQuery();
    void resetSchema(QString new_schema_name);

public:
    explicit DBKeysModel(std::shared_ptr<jsonio::DBSchemaDocument> db_client,
                         QObject *parent = nullptr);
    /// Destroys the object and frees any allocated resources.
    virtual ~DBKeysModel();

    Q_INVOKABLE void updateQuery(const QString& query);
    Q_INVOKABLE void updateFields(const QStringList& query_fields);
    void updateQuery(const jsonio::DBQueryBase& query);
    void updateFields(const std::vector<std::string>& query_fields);
    void setQuery(const jsonio::DBQueryBase& query,
                  const std::vector<std::string>& query_fields,
                  const jsonio::DBSchemaDocument* dbclient=nullptr) override;

    Q_INVOKABLE QString get_schema() const;
    Q_INVOKABLE std::string get_id(size_t row) const;
    Q_INVOKABLE std::string read(size_t row) const;
    Q_INVOKABLE void save(const std::string& json_data);
    Q_INVOKABLE void remove(size_t row);

protected:
    std::shared_ptr<jsonio::DBSchemaDocument> dbclient;

    void build_table(std::vector<std::vector<std::string>>& table_data,
                     std::vector<std::string>& header_data);

};

}

#endif // DKEYSMODEL_H
