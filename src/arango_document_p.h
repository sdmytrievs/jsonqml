#ifndef ARANGODOC_P_H
#define ARANGODOC_P_H

#include "jsonqml/arango_document.h"
#include "arango_database_p.h"
#include "jsonio/dbedgedoc.h"

namespace jsonqml {

/// \class ArangoDBDocument
/// \brief The ArangoDBDocument class object to work with database document.
class ArangoDBDocumentPrivate
{
public:
    explicit ArangoDBDocumentPrivate(DocumentType type,
                                     const QString& schema,
                                     ArangoDatabasePrivate* parent_database);

    /// Destroys the object and frees any allocated resources.
    virtual ~ArangoDBDocumentPrivate(){}

    virtual void set_query(const jsonio::DBQueryBase& query,
                           const jsonio::values_t& query_fields);
    virtual void set_default_query();
    virtual void update_query();
    virtual bool update_schema(const std::string& new_schema);
    void execute_query(const jsonio::DBQueryBase &query,
                       const jsonio::values_t &query_fields);
    bool remove_document();

    virtual std::string read(const std::string& doc_id, std::string &doc_schema);
    virtual std::string read_query(const std::string& doc_id, std::string& doc_schema);
    // true if new doc_id
    virtual bool save(const std::string &json_data, std::string& doc_id);
    virtual void remove(const std::string& doc_id);

protected:
    friend class ArangoDBDocument;


    ArangoDatabasePrivate* database;
    //const jsonio::DataBase &database;
    DocumentType doc_type; // json not implemented to do
    std::string document_schema_name;
    bool query_exist;
    std::shared_ptr<jsonio::DBSchemaDocument> dbdocument;

    jsonio::DBQueryBase last_good_query;
    jsonio::values_t last_fields;
    jsonio::values_t new_header;
    jsonio::values_table_t new_table;

    bool is_dbdocument();
    void build_table();
};

}

#endif // ARANGODOC_P_H
