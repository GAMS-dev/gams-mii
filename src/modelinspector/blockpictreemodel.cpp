#include "blockpictreemodel.h"
#include "modelinstance.h"
#include "blockpictreeitem.h"

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

BlockpicTreeModel::BlockpicTreeModel(BlockpicTreeItem *rootItem, QObject *parent)
    : QAbstractItemModel(parent)
    , mRootItem(rootItem)
{
}

BlockpicTreeModel::~BlockpicTreeModel()
{
    delete mRootItem;
}

QVariant BlockpicTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        auto* item = static_cast<BlockpicTreeItem*>(index.internalPointer());
        return item->data(index.column());
    }
    if (role == Qt::TextAlignmentRole) { // TODO move to delegate... if there is one
        QRegExp regex("^\\D+");
        if (regex.exactMatch(index.data().toString())) {
            return Qt::AlignLeft;
        } else {
            return Qt::AlignRight;
        }
    }
    return QVariant();
}

Qt::ItemFlags BlockpicTreeModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractItemModel::flags(index);
    return Qt::NoItemFlags;
}

QVariant BlockpicTreeModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return mRootItem->data(section);
    return QVariant();
}

QModelIndex BlockpicTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    BlockpicTreeItem *parentItem;
    if (parent.isValid())
        parentItem = static_cast<BlockpicTreeItem*>(parent.internalPointer());
    else
        parentItem = mRootItem;

    BlockpicTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex BlockpicTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto* childItem = static_cast<BlockpicTreeItem*>(index.internalPointer());
    auto* parentItem = childItem->parent();

    if (parentItem == mRootItem)
        return QModelIndex();

    return createIndex(parentItem->rows(), 0, parentItem);
}

int BlockpicTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    BlockpicTreeItem *parentItem;
    if (parent.isValid())
        parentItem = static_cast<BlockpicTreeItem*>(parent.internalPointer());
    else
        parentItem = mRootItem;
    return parentItem->childCount();
}

int BlockpicTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<BlockpicTreeItem*>(parent.internalPointer())->columns();
    return mRootItem->columns();
}

}
}
}
