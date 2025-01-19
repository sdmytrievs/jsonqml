#ifndef EDGECLIENT_P_H
#define EDGECLIENT_P_H

#include "jsonqml/clients/edge_client.h"
#include "vertex_client_p.h"

namespace jsonqml {

class EdgeClientPrivate : public VertexClientPrivate
{
    Q_DISABLE_COPY_MOVE(EdgeClientPrivate)

public:

    explicit EdgeClientPrivate();
    virtual ~EdgeClientPrivate() {}

    void init() override;

    QStringList gen_schema_list() const override;
    QString new_list_default_schema() const  override
    {
        return (schema_names_list.empty()? "": schema_names_list[0]);
    }

    QAbstractItemModel* in_key_model() const
    {
        return in_model.get();
    }
    QAbstractItemModel* out_key_model() const
    {
        return out_model.get();
    }

    bool update_schema(QString new_schema) override;
    void update_jsonmodel() override;
    void update_keysmodel() override;
    void read_editor_data(int row) override;
    void set_edges_for(jsonio::DBQueryBase&& query);
    std::vector<std::string> make_vertex_query_fields(std::string vertex_id) const;
    jsonio::DBQueryBase make_vertex_query(std::string vertex_id) const;

    bool set_json(const std::string& json_string, const QString& schema_name="") override;

protected:
    friend class EdgeClient;

    std::shared_ptr<jsonio::DBEdgeDocument> dbedge_collection;
    QSharedPointer<DBQueryModel> in_model;
    QSharedPointer<DBQueryModel> out_model;
    std::string in_vertex_id;
    std::string out_vertex_id;
    /// Edge query could be changed
    bool change_schema_mode = false;
    const std::vector<std::string> all_edges_fields = {"_label", "_from", "_to", "_id"};
    const std::vector<std::string> all_vertex_fields = {"_label", "_id"};
};

}

#endif // EDGECLIENT_P_H
