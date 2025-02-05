
#include <QColor>
#include "jsonqml/models/json_model.h"
#include "jsonqml/clients/settings_client.h"
#include "jsonio/service.h"


namespace jsonqml {


const QStringList JsonBaseModel::type_names= {"string", "bool", "int", "double", "object", "array"};


bool JsonBaseModel::set_value_via_type(jsonio::JsonBase* object, const std::string& add_key,
                                       jsonio::JsonBase::Type add_type, const QVariant& add_value)
{
    if(!object) {
        return false;
    }

    switch(add_type) {
    case jsonio::JsonBase::Null:
    case jsonio::JsonBase::Bool:
    case jsonio::JsonBase::Int:
    case jsonio::JsonBase::Double:
    case jsonio::JsonBase::String:
        object->set_scalar_via_path(add_key, add_value.toString().toStdString());
        break;
    case jsonio::JsonBase::Object:
        object->add_object_via_path(add_key);
        break;
    case jsonio::JsonBase::Array:
        /*auto& new_object = */object->add_array_via_path(add_key);
        //if( !add_value.toString().isEmpty() )
        //    new_object.loads(add_value.toString().toStdString());
        break;
    }
    return true;
}

jsonio::JsonBase::Type JsonBaseModel::type_from(const QString &field_type, std::string& def_value)
{
    jsonio::JsonBase::Type type = jsonio::JsonBase::Null;
    def_value.clear();

    auto index = type_names.indexOf(field_type, 0, Qt::CaseInsensitive);
    switch(index) {
    case -1: type = jsonio::JsonBase::Null; break;
    case 1:  type = jsonio::JsonBase::Bool;  def_value ="false"; break;
    case 2: type = jsonio::JsonBase::Int;    def_value ="0";  break;
    case 3: type = jsonio::JsonBase::Double; def_value ="0"; break;
    case 0: type = jsonio::JsonBase::String; break;
    case 4: type = jsonio::JsonBase::Object; break;
    case 5: type = jsonio::JsonBase::Array; break;
    }
    return type;
}

const QModelIndex JsonBaseModel::addObject(const QModelIndex &cindex, const QString &field_type, const QString &field_name)
{
    int row;
    std::string new_object_key = field_name.toStdString();
    std::string def_value;
    jsonio::JsonBase::Type new_object_type = type_from(field_type, def_value);
    jsonio::trim(new_object_key);

    QModelIndex parent_index;
    auto item = lineFromIndex(cindex);
    if(item->isObject() && item->size()<1) {
        parent_index = cindex.siblingAtColumn(0);
        row = 0;
    }
    else {
        parent_index = parent(cindex).siblingAtColumn(0);
        row = cindex.row(); // rowCount(parentIndex);
    }
    auto parent_item = lineFromIndex(parent_index);

    try {
        beginInsertRows(parent_index, row, row);
        set_value_via_type(parent_item, new_object_key, new_object_type, QString::fromStdString(def_value));
        endInsertRows();
    }
    catch(std::exception& e) {
        endInsertRows();
        uiSettings().setError(e.what());
    }
    return index(row, 0, parent_index);
}

void JsonBaseModel::resizeArray(const QModelIndex &index, int new_size)
{
    auto item = lineFromIndex(index);
    if(rowCount(index)) {
        beginRemoveRows(index.siblingAtColumn(0), 0, rowCount(index));
        endRemoveRows();
    }
    item->array_resize(new_size, "");
    if(new_size>0) {
        beginInsertRows(index.siblingAtColumn(0), 0, new_size-1);
        endInsertRows();
    }
}

const QModelIndex JsonBaseModel::cloneObject(const QModelIndex &cindex)
{
    auto item = lineFromIndex(cindex);
    QModelIndex parent_index = parent(cindex);
    auto parent_item = lineFromIndex(parent_index);
    if(!parent_item->isArray()) {
        return cindex;
    }
    int row =  cindex.row();
    auto data = item->dump();
    try{
        beginInsertRows(parent_index, row, row);
        if(set_value_via_type(parent_item, std::to_string(row), item->type(), QString("0")) && !data.empty() ) {
            parent_item->getChild(row)->loads(data);
        }
        endInsertRows();
    }
    catch(std::exception& e) {
        endInsertRows();
        uiSettings().setError(e.what());
    }
    return index( row, 0, parent_index);
}

void JsonBaseModel::removeObject(const QModelIndex &index)
{
    auto line = lineFromIndex(index);
    beginRemoveRows(parent(index), index.row(), index.row());
    line->remove();
    endRemoveRows();
}

void  JsonBaseModel::setFieldData(const QModelIndex& index, const QString& data)
{
    auto line =  lineFromIndex(index);
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

//--------------------------------------------------------------------------------------

JsonFreeModel::JsonFreeModel(const QStringList& header_names, QObject* parent):
    JsonBaseModel(parent),
    header_data(header_names),
    root_node(jsonio::JsonFree::object())
{
    setupModelData("", "");
}


jsonio::JsonBase* JsonFreeModel::lineFromIndex(const QModelIndex &index) const
{
    if(index.isValid())  {
        return static_cast<jsonio::JsonFree*>(index.internalPointer());
    }
    else {
        return const_cast<jsonio::JsonFree*>(&root_node);
    }
}

void JsonFreeModel::setupModelData(const std::string& json_string, const QString&)
{
    try {
        beginResetModel();
        if(json_string.empty()) {
            root_node.clear();
        }
        else {
            root_node.loads(json_string);
        }
        endResetModel();
    }
    catch(...)
    {
        endResetModel();
        throw;
    }
    emit modelExpand();
}

QModelIndex JsonFreeModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex JsonFreeModel::parent(const QModelIndex& child) const
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

int JsonFreeModel::rowCount(const QModelIndex& parent) const
{
    if( !parent.isValid() ) {
        return root_node.size();
    }
    auto parentItem = lineFromIndex(parent);
    return parentItem->size();
}

int JsonFreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED( parent );
    return 2;
}

Qt::ItemFlags JsonFreeModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    auto item = lineFromIndex( index );
    if(index.column() == 1 && (!item->isObject() && !item->isArray())) {
        flags |= Qt::ItemIsEditable;
        return flags;
    }
    else {
        return (flags & ~Qt::ItemIsEditable);
    }
}

QVariant JsonFreeModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal && section<header_data.size()) {
        return header_data[section];
    }
    return QVariant();
}

QVariant JsonFreeModel::data( const QModelIndex& index, int role ) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:  {
        auto item = lineFromIndex(index);
        if(index.column() == 0) {
            return QString::fromStdString(item->getKey());
        }
        else if(index.column() == 1) {
            return QString::fromStdString(item->getFieldValue());
        }
    }
        break;
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
        return QString::fromStdString(lineFromIndex(index)->getDescription());
    case Qt::ForegroundRole:
        if(index.column() == 0) {
            return QVariant(QColor(Qt::darkCyan));
        }
        break;
    default: break;
    }
    return QVariant();
}

bool JsonFreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(index.isValid() && (role == Qt::EditRole)) {
        if(index.column() == 1) {
            auto line = lineFromIndex( index );
            set_value_via_type(line, "",  line->type(), value);
        }
        return true;
    }
    return false;
}



} // namespace jsonqml
