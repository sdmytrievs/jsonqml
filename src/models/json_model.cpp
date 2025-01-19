
#include <QColor>
#include "jsonqml/models/json_model.h"


namespace jsonqml {


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
    catch(...) {
        endResetModel();
        throw;
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
    auto parentItem = lineFromIndex(parent);
    if(parentItem->size()>0) {
        return createIndex(row, column, parentItem->getChild(row));
    }
    else {
        return QModelIndex();
    }
}

QModelIndex JsonFreeModel::parent(const QModelIndex& child) const
{
    if(!child.isValid()) {
        return QModelIndex();
    }

    auto childItem = lineFromIndex(child);
    auto parentItem = childItem->getParent();
    if( parentItem == &root_node ) {
        return QModelIndex();
    }
    return createIndex(parentItem->getNdx(), 0, parentItem);
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
