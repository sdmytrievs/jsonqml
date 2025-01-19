#pragma once

#include <QAbstractItemModel>
#include "jsonio/jsonbase.h"


namespace jsonqml {

enum TableFlag {
    RowSortingEnabled =   0x0010,///< Added sorting into colums
    GraphDataEnabled =  0x0004,  ///< Connect 2d graphic for columns
    TableIsEditable =   0x0002,  ///< Enable editing
    MenuEnabled = 0x0001,        ///< Disable context menu
    NoTableFlags = 0             ///< Use only show mode
};
Q_DECLARE_FLAGS(TableFlags, TableFlag)
//Q_DECLARE_OPERATORS_FOR_FLAGS(TableFlags)
//Q_FLAGS(TableFlags)

/// \class JsonBaseModel
/// Abstract class for represents the data set and is responsible for fetching the data
/// is needed for viewing and for writing back any changes.
/// Reading/writing data from/to json type objects
class JsonBaseModel: public QAbstractItemModel
{
    Q_OBJECT

signals:

  void modelExpand();

public:

    JsonBaseModel(QObject* parent = nullptr):
        QAbstractItemModel(parent)
    {}
    ~JsonBaseModel() {}

    /// Return internal data to const link
    virtual const jsonio::JsonBase& current_object() const = 0;

    /// Extern update data
    virtual void setupModelData(const std::string& json_string, const QString& schema) = 0;

private:

    virtual bool isNumber(const QModelIndex& index) const
    {
        return lineFromIndex(index)->isNumber();
    }

    virtual bool isCanBeResized(const QModelIndex&) const
    {
        return false;
    }

    virtual bool isCanBeAdd(const QModelIndex&) const
    {
        return false;
    }

    virtual bool isUnion(const QModelIndex&) const
    {
        return false;
    }

    virtual bool isCanBeRemoved(const QModelIndex&) const
    {
        return false;
    }

    virtual bool isCanBeCloned(const QModelIndex&) const
    {
        return false;
    }

    virtual QString helpName(const QModelIndex& index) const
    {
        return QString::fromStdString(lineFromIndex(index)->getHelpName());
    }

    virtual QString getFieldPath(const QModelIndex& index) const
    {
        return QString::fromStdString(lineFromIndex(index)->get_path());
    }

    virtual QString getFieldData(const QModelIndex& index) const
    {
        return QString::fromStdString(lineFromIndex(index)->dump());
    }

    virtual void setFieldData(const QModelIndex& index, const QString& data);

protected:

    virtual jsonio::JsonBase& current_data() const = 0;
    virtual jsonio::JsonBase* lineFromIndex(const QModelIndex& index) const = 0;

    virtual bool set_value_via_type(jsonio::JsonBase* object, const std::string& add_key,
                                    jsonio::JsonBase::Type add_type, const QVariant& add_value);

};


} // namespace jsonqml

