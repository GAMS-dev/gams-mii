#include "attributetablemodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio{
namespace modelinspector {

AttributeTableModel::AttributeTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

AttributeTableModel::~AttributeTableModel()
{

}

void AttributeTableModel::setModelInstance(const QSharedPointer<AbstractModelInstance> &modelInstance)
{
    beginResetModel();
    mModelInstance = modelInstance;
    endResetModel();
}

QVariant AttributeTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }
    if (index.isValid() && role == Qt::DisplayRole) {
    //    auto value = mModelInstance->data(index.row(), index.column(), mView);
    //    return value == 0.0 ? QVariant() : value;
    }
    return QVariant();
}

Qt::ItemFlags AttributeTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractTableModel::flags(index) |
                Qt::ItemIsEnabled |
                Qt::ItemIsSelectable;
    return Qt::NoItemFlags;
}

QVariant AttributeTableModel::headerData(int section,
                                         Qt::Orientation orientation,
                                         int role) const
{
    if (role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);
    return section;
}

QModelIndex AttributeTableModel::index(int row, int column,
                                           const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
        return createIndex(row, column);
    return QModelIndex();
}

int AttributeTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return constant->PredefinedHeaderLength;
}

EquationAttributeTableModel::EquationAttributeTableModel(QObject *parent)
    : AttributeTableModel(parent)
{

}

EquationAttributeTableModel::~EquationAttributeTableModel()
{

}

QVariant EquationAttributeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignLeft;
    }
    if (role != Qt::DisplayRole) {
        return QAbstractItemModel::headerData(section, orientation, role);
    }
    if (orientation == Qt::Horizontal && section < constant->PredefinedHeaderLength) {
        return constant->PredefinedHeader[section];
    }
    return section;
}

int EquationAttributeTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->columnCount(ViewDataType::EqnAttributes);
}

int EquationAttributeTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->rowCount(ViewDataType::EqnAttributes);
}

VariableAttributeTableModel::VariableAttributeTableModel(QObject *parent)
    : AttributeTableModel(parent)
{

}

VariableAttributeTableModel::~VariableAttributeTableModel()
{

}

QVariant VariableAttributeTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }
    if (index.isValid() && role == Qt::DisplayRole) {
    //    auto value = mModelInstance->data(index.row(), index.column(), mView);
    //    return value == 0.0 ? QVariant() : value;
    }
    return QVariant();
}

QVariant VariableAttributeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QAbstractItemModel::headerData(section, orientation, role);
    }
    if (orientation == Qt::Vertical && section < constant->PredefinedHeaderLength) {
        return constant->PredefinedHeader[section];
    }
    return section;
}

int VariableAttributeTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->columnCount(ViewDataType::VarAttributes);
}

int VariableAttributeTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->rowCount(ViewDataType::VarAttributes);
}

}
}
}
