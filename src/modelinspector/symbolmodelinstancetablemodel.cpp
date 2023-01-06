#include "symbolmodelinstancetablemodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio{
namespace modelinspector {

SymbolModelInstanceTableModel::SymbolModelInstanceTableModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                                               QObject *parent)
    : QAbstractTableModel(parent)
    , mModelInstance(modelInstance)
{

}

void SymbolModelInstanceTableModel::setModelInstance(const QSharedPointer<AbstractModelInstance> &modelInstance)
{
    beginResetModel();
    mModelInstance = modelInstance;
    endResetModel();
}

QVariant SymbolModelInstanceTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }
    if (role == Qt::DisplayRole && index.isValid()) {
        return mModelInstance->data(index.row(), index.column(), mView);
    }
    if (role == ColumnEntryRole) {
        return mModelInstance->columnEntries(index.column(), mView);
    }
    if (role == RowEntryRole) {
        return mModelInstance->rowEntries(index.row(), mView);
    }
    return QVariant();
}

Qt::ItemFlags SymbolModelInstanceTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractTableModel::flags(index) |
                Qt::ItemIsEnabled |
                Qt::ItemIsSelectable;
    return Qt::NoItemFlags;
}

QVariant SymbolModelInstanceTableModel::headerData(int section,
                                                    Qt::Orientation orientation,
                                                    int role) const
{
    if (role == Qt::DisplayRole) {
        auto realIndex = mModelInstance->headerData(section, orientation, mView);
        return realIndex < 0 ? QVariant() : realIndex;
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex SymbolModelInstanceTableModel::index(int row,
                                                  int column,
                                                  const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
        return createIndex(row, column);
    return QModelIndex();
}

int SymbolModelInstanceTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->rowCount(mView);
}

int SymbolModelInstanceTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->columnCount(mView);
}

QHash<int, QByteArray> SymbolModelInstanceTableModel::roleNames() const
{
    static QHash<int, QByteArray> mapping {
        {RowEntryRole, "rowentry"},
        {ColumnEntryRole, "columnentry"}
    };
    return mapping;
}

int SymbolModelInstanceTableModel::view() const
{
    return mView;
}

void SymbolModelInstanceTableModel::setView(int view)
{
    mView = view;
}

}
}
}
