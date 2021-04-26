#include "modelinstancetablemodel.h"
#include "modelinstance.h"
#include "hierarchicalheaderview.h"

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

// TODO
//  o introduce delegate?

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
    mHorizontalHeaderModel.clear();
    mVerticalHeaderModel.clear();
    mModelInstance = modelInstance;
    mModelInstance->horizontalHeaderData(mHorizontalHeaderModel);
    mModelInstance->verticalHeaderData(mVerticalHeaderModel);
    endResetModel();
}

QVariant ModelInstanceTableModel::data(const QModelIndex &index, int role) const
{
    if (role == HierarchicalHeaderView::HorizontalHeaderDataRole) {
        QVariant headerModel;
        headerModel.setValue((QObject*)&mHorizontalHeaderModel);
        return headerModel;
    }

    if (role == HierarchicalHeaderView::VerticalHeaderDataRole) {
        QVariant variant;
        variant.setValue((QObject*)&mVerticalHeaderModel);
        return variant;
    }

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
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex ModelInstanceTableModel::index(int row, int column,
                                           const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex ModelInstanceTableModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
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
