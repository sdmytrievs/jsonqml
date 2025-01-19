#ifndef VERTEXCLIENT_P_H
#define VERTEXCLIENT_P_H

#include "jsonqml/clients/vertex_client.h"
#include "jsonqml/models/db_keys_model.h"
#include "json_client_p.h"

namespace jsonqml {

class VertexClientPrivate : public JsonClientPrivate
{
    Q_DISABLE_COPY_MOVE(VertexClientPrivate)

public:

    explicit VertexClientPrivate();
    virtual ~VertexClientPrivate() {}

    void init() override;

    QStringList gen_schema_list() const override;
    QString new_list_default_schema() const  override
    {
      return (schema_names_list.empty()? "": schema_names_list[0]);
    }

    bool sorting_enabled() const;
    virtual QAbstractItemModel* keys_list_model() const;

    bool update_schema(QString new_schema) override;
    void update_jsonmodel() override;
    virtual void update_keysmodel();

    virtual void read_editor_data(int row);
    // read vertex without changes current_schema
    void read_editor_id(const std::string& vertex_id);
    std::string id_from_editor();

    virtual bool set_json(const std::string& json_string, const QString& schema_name="") override;

protected:
    int keys_table_mode = RowSortingEnabled;
    std::shared_ptr<jsonio::DBVertexDocument> dbcollection;
    QSharedPointer<DBKeysModel> keys_model;
    QSharedPointer<SortFilterProxyModel> sort_proxy_model;
};

}

#endif // VERTEXCLIENT_P_H
