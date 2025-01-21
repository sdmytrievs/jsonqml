#ifndef ARANGODB_H
#define ARANGODB_H

#include <string>
#include <map>
#include <QObject>
#include <QQmlEngine>

#include "jsonio/dbquerybase.h"


namespace jsonqml {


class ArangoDocument;
class ArangoDatabasePrivate;

enum DocumentType { Json, Schema, Vertex, Edge, Resource };

class ArangoDatabase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString dbConnect READ dbConnect NOTIFY dbConnectChanged)
    Q_PROPERTY(bool dbConnected READ dbConnected NOTIFY dbdriveChanged)

    Q_PROPERTY(QString dbUrl READ dbUrl NOTIFY dbConnectChanged)
    Q_PROPERTY(QString dbName READ dbName NOTIFY dbConnectChanged)
    Q_PROPERTY(QString dbUser READ dbUser NOTIFY dbConnectChanged)
    Q_PROPERTY(QString dbUserPassword READ dbUserPassword NOTIFY dbConnectChanged)
    Q_PROPERTY(bool    dbAccess READ dbAccess NOTIFY dbConnectChanged)
    Q_PROPERTY(bool    dbCreate READ dbCreate NOTIFY dbConnectChanged)

signals:
    void dbdriveChanged();
    void dbConnectListChanged();
    void dbConnectChanged();
    void dbNamesListChanged();
    void dbUsersListChanged();

public slots:
    /// Notify about updating the document (may require updating of related ones)
    virtual void afterUpdatedDocument(std::string schema_name, std::string doc_id);
    /// Notify about document deletion (may require deletion of related ones)
    virtual void afterDeletedDocument(std::string schema_name, std::string doc_id);
    /// Restore collection descriptors after scemasPathChanged
    void resetCollectionsList();

private slots:
    void ConnectTo(const QString& url, const QString& name,
                   const QString& user, const QString& user_passwd);
    void ConnectFromSettings();

public:

    /// Default resourse Database name
    static std::string resources_database_name;

    /// Constructor
    explicit ArangoDatabase();

    /// Destructor
    ~ArangoDatabase();


    QString dbConnect();
    bool dbConnected();

    QString dbUrl();
    QString dbName();
    QString dbUser();
    QString dbUserPassword();
    bool    dbAccess();
    bool    dbCreate();

    void getRootLists(const std::string& db_group,
                          QStringList& db_all_databases,
                          QStringList& db_all_users);

    /// The current worker Database exists and checked connection for the current credentials group
    bool dbConnected() const;


    QScopedPointer<ArangoDocument> createDocument(DocumentType type, const QString& document_schema_name="");
    QScopedPointer<ArangoDocument> createDocumentQuery(DocumentType type,
                                                       const QString& document_schema_name="",
                                                       const jsonio::DBQueryBase& query=jsonio::DBQueryBase::emptyQuery(),
                                                       const jsonio::values_t& query_fields={});

    static void defineCollection(const std::string& collection_name,
                                 const std::string& schema_name,
                                 jsonio::values_t&& query_fields)
    {
        user_defined_collections[collection_name]= {schema_name, std::move(query_fields)};
    }
    static QStringList getEdgesList();
    static QStringList getVertexesList();
    static std::string collectionFromSchema(const std::string& schema_name);
    static std::string schemaFromCollection(const std::string& collection_name);
    static jsonio::values_t fieldsFromCollection(const std::string& collection_name);

protected:

    struct CollectionData {
        std::string schema_name;
        jsonio::values_t default_query_fields;
    };
    static std::map<std::string, CollectionData> user_defined_collections;
    static std::map<std::string, CollectionData> defined_collections;
    jsonio::values_t make_query_fields(std::string schema_name) const;


    friend ArangoDatabase& arango_db();
    friend class ArangoDatabasePrivate;

    QScopedPointer<ArangoDatabasePrivate> impl_ptr;
    ArangoDatabasePrivate* impl_func()
              { return reinterpret_cast<ArangoDatabasePrivate *>(impl_ptr.get()); }
    const ArangoDatabasePrivate* impl_func() const
              { return reinterpret_cast<const ArangoDatabasePrivate *>(impl_ptr.get()); }

};

/// Function to connect to default ArangoDatabase object
extern ArangoDatabase& arango_db();
/// Function to connect to default resource ArangoDatabase object
extern ArangoDatabase& resource_db();

}

#endif // ARANGODB_H
