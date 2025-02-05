#include <QColor>
#include "jsonqml/models/schema_model.h"
#include "jsonqml/clients/settings_client.h"
#include "jsonio/io_settings.h"

namespace jsonqml {


JsonSchemaModel::JsonSchemaModel(const QString& schema_name,
                                 const QStringList& header_names,
                                 QObject* parent):
    JsonBaseModel(parent),
    header_data(header_names),
    current_schema(schema_name),
    root_node(jsonio::JsonSchema::object(current_schema.toStdString()))
{
    setupModelData("", schema_name);
}

jsonio::JsonBase* JsonSchemaModel::lineFromIndex(const QModelIndex &index ) const
{
    if( index.isValid() )  {
        return static_cast<jsonio::JsonSchema*>(index.internalPointer());
    }
    else {
        return const_cast<jsonio::JsonSchema*>(&root_node);
    }
}

void JsonSchemaModel::setupModelData(const std::string& json_string, const QString& schema)
{
    try {
        beginResetModel();
        if(current_schema != schema) {
            current_schema = schema;
            root_node = jsonio::JsonSchema::object(current_schema.toStdString());
        }
        if(json_string.empty()) {
            root_node.clear();
        }
        else {
            root_node.loads(json_string);
        }
        endResetModel();
    }
    catch(...) {
        endResetModel();
        throw;
    }
    emit modelExpand();
}

QModelIndex JsonSchemaModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex{};
    auto *parent_item = lineFromIndex(parent);
    if(parent_item->size()>row) {
        return createIndex(row, column, parent_item->getChild(row));
    }
    else {
        return QModelIndex{};
    }
}

QModelIndex JsonSchemaModel::parent(const QModelIndex& child) const
{
    if(!child.isValid()) {
        return QModelIndex{};
    }
    auto child_item = lineFromIndex(child);
    auto parent_item = child_item->getParent();
    if(parent_item == &root_node) {
        return QModelIndex{};
    }
    return createIndex(parent_item->getNdx(), 0, parent_item);
}

int JsonSchemaModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid()) {
        return root_node.size();
    }
    auto parentItem = lineFromIndex( parent );
    return parentItem->size();
}

int JsonSchemaModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED( parent );
    if(showComments) {
        return 3;
    }
    return 2;
}

Qt::ItemFlags JsonSchemaModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    auto item = lineFromIndex( index );

    if(!editID && item->getKey()[0]=='_') {
        return (flags & ~Qt::ItemIsEditable);
    }

    if(index.column()==1 && !(item->isObject()||item->isArray()||get_map_enumdef(index)!= nullptr) )  {
        flags |= Qt::ItemIsEditable;
        return flags;
    }

    if(index.column()==0 && !item->isTop() && schemajs(item->getParent())->isMap()) {
        flags |= Qt::ItemIsEditable;
        return flags;
    }

    return (flags & ~Qt::ItemIsEditable);
}

QVariant JsonSchemaModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal && section<header_data.size()) {
        return header_data[section];
    }
    return QVariant();
}


QVariant JsonSchemaModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    auto node = lineFromIndex( index );
    switch(role)  {
    case Qt::DisplayRole:
        if(useEnumNames) {
            auto enumdef = get_i32_enumdef(index);
            if(enumdef != nullptr) {
                int idx;
                node->get_to(idx);
                std::string name = enumdef->value2name(idx);

                if(index.column()==1) {
                    return  QString::fromStdString(name);
                }
                if(index.column()==2) {
                    return  QString::fromStdString(enumdef->name2description(name));
                }
            }
        }
        if(index.column()==2) {
            auto mapenum = get_map_enumdef( index );
            if(mapenum != nullptr) {
                std::string name;
                node->get_to(name);
                return  QString::fromStdString(mapenum->name2description(name));
            }
        }
        return get_value(index.column(), node);
    case Qt::EditRole:
        return get_value(index.column(), node);
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
        return  QString::fromStdString(schemajs(node)->getFullDescription());
    case Qt::ForegroundRole:  {
        if(index.column()==0 && !node->isTop() && !schemajs(node->getParent())->isMap()) {
            return QVariant(QColor(Qt::darkCyan));
        }
    }
        break;
    default: break;
    }
    return QVariant();
}

bool JsonSchemaModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(index.isValid() && role==Qt::EditRole) {
        auto line = lineFromIndex(index);
        switch(index.column()) {
        case 0: {
            // ?????????????
            auto lineschema = schemajs(line);
            lineschema->setMapKey(value.toString().toStdString());
        }
            break;
        case 1:
            set_value_via_type(line, "", line->type(), value);
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}

bool JsonSchemaModel::canBeResized(const QModelIndex &index) const
{
    auto item = schemajs(lineFromIndex(index));
    return  item->isArray() || item->isMap();
}

bool JsonSchemaModel::canBeAdd(const QModelIndex &index) const
{
    auto line = schemajs(lineFromIndex(index));
    if(line->isTop() || (line->isStruct() && line->size()<1)) {
        return true;
    }
    auto parent_item = schemajs(lineFromIndex(parent(index)));
    return parent_item->isStruct();
}

bool JsonSchemaModel::isUnion(const QModelIndex &index) const
{
    auto item = schemajs(lineFromIndex(index));
    return  item->isUnion();
}

bool JsonSchemaModel::canBeRemoved(const QModelIndex &index) const
{
    auto line = lineFromIndex(index);
    return  !line->isTop() && line->getParent()->isObject();
}

bool JsonSchemaModel::canBeCloned(const QModelIndex &index) const
{
    auto line = lineFromIndex(index);
    if(line->isTop()) {
        return false;
    }
    auto parentIndex = parent(index);
    auto parent_item = schemajs(lineFromIndex(parentIndex));
    return  (parent_item->isArray() || parent_item->isMap());
}

void  JsonSchemaModel::setFieldData(const QModelIndex& index, const QString& data)
{
    auto line =  lineFromIndex(index);
    if(!editID && line->getKey()[0] == '_') {
        return;
    }
    try {
        beginResetModel();
        if(data.isEmpty()) {
            line->clear();
        }
        else {
            line->loads(data.toStdString());
        }
        endResetModel();
    }
    catch(std::exception& e) {
        endResetModel();
        uiSettings().setError(e.what());
    }

}

//-------------------------------------------------------------------------------------

QString JsonSchemaModel::get_value(int column, const jsonio::JsonBase *object) const
{
    switch(column) {
    case 0: return  QString::fromStdString(object->getKey());
    case 2: return  QString::fromStdString(object->getDescription());
    case 1: return  QString::fromStdString(object->getFieldValue());
    }
    return QString();
}

const jsonio::EnumDef* JsonSchemaModel::get_map_enumdef(const QModelIndex& index) const
{
    if(!index.isValid()) { // isTop
        return nullptr;
    }

    auto parentIndex = parent(index);
    auto parent_item = schemajs(lineFromIndex(parentIndex));

    if(parent_item->isMap()) {
        size_t level = 0;
        auto field_data = parent_item->fieldDescription(level);
        std::string enumName = field_data->className();
        if(!enumName.empty()) {
            return jsonio::ioSettings().Schema().getEnum(enumName);
        }
    }
    return nullptr;
}

const jsonio::EnumDef* JsonSchemaModel::get_i32_enumdef(const QModelIndex& index) const
{
    if(!index.isValid()) { // isTop
        return nullptr;
    }

    auto item = schemajs(lineFromIndex(index));
    if(item->fieldType() != jsonio::FieldDef::T_I32) {
        return nullptr;
    }
    size_t level = 0;
    auto field_data = item->fieldDescription(level);
    std::string enumName = field_data->className();
    if(!enumName.empty()) {
        return jsonio::ioSettings().Schema().getEnum(enumName);
    }
    return nullptr;
}

} // namespace jsonqml
