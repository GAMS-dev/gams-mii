#include "modelinstancetablemodel.h"
#include "modelinstance.h"

namespace gams {
namespace studio{
namespace modelinspector {

ModelInstanceTableModel::ModelInstanceTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

ModelInstanceTableModel::~ModelInstanceTableModel()
{
}

void ModelInstanceTableModel::setModelInstance(const QSharedPointer<ModelInstance> &modelInstance)
{
    beginResetModel();
    mModelInstance = modelInstance;
    endResetModel();
}

QVariant ModelInstanceTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }
    if (index.isValid() && role == Qt::DisplayRole) {
        return mModelInstance->data(index.row(), index.column());
    }

    return QVariant();
}

Qt::ItemFlags ModelInstanceTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractTableModel::flags(index) |
                Qt::ItemIsEnabled |
                Qt::ItemIsSelectable;
    return Qt::NoItemFlags;
}

QVariant ModelInstanceTableModel::headerData(int section,
                                             Qt::Orientation orientation,
                                             int role) const
{
    if (role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);
    return section;
}

QModelIndex ModelInstanceTableModel::index(int row, int column,
                                           const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
        return createIndex(row, column);
    return QModelIndex();
}

int ModelInstanceTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->rowCount();
}

int ModelInstanceTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->columnCount();
}

}
}
}
