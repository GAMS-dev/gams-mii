#include "jaccobiantablemodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio{
namespace modelinspector {

JaccobianTableModel::JaccobianTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , mViewType(ViewDataType::Jaccobian)
{

}

JaccobianTableModel::~JaccobianTableModel()
{
}

void JaccobianTableModel::setModelInstance(const QSharedPointer<AbstractModelInstance> &modelInstance)
{
    beginResetModel();
    mModelInstance = modelInstance;
    endResetModel();
}

QVariant JaccobianTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }
    if (role == Qt::DisplayRole && index.isValid()) {
        //return mModelInstance->data(index.row(), index.column());
    }
    return QVariant();
}

Qt::ItemFlags JaccobianTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractTableModel::flags(index) |
                Qt::ItemIsEnabled |
                Qt::ItemIsSelectable;
    return Qt::NoItemFlags;
}

QVariant JaccobianTableModel::headerData(int section,
                                         Qt::Orientation orientation,
                                         int role) const
{
    if (role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);
    return section;
}

QModelIndex JaccobianTableModel::index(int row, int column,
                                       const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
        return createIndex(row, column);
    return QModelIndex();
}

int JaccobianTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->rowCount(mViewType);
}

int JaccobianTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->columnCount(mViewType);
}

}
}
}
