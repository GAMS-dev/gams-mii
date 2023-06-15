#include "postopttreemodel.h"
#include "postopttreeitem.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio {
namespace modelinspector {

PostoptTreeModel::PostoptTreeModel(int view,
                                   QSharedPointer<AbstractModelInstance> modelInstance,
                                   QObject *parent)
    : QAbstractItemModel(parent)
    , mModelInstance(modelInstance)
    , mRootItem(mModelInstance->dataTree(view))
    , mView(view)
{

}

PostoptTreeModel::~PostoptTreeModel()
{

}

QVariant PostoptTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Qt::DisplayRole) {
        auto *item = static_cast<PostoptTreeItem*>(index.internalPointer());
        return item->data(index.column());
    }
    if (role == Qt::TextAlignmentRole && index.column() > 0) {
        return Qt::AlignRight;
    }
    return QVariant();
}

Qt::ItemFlags PostoptTreeModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return Qt::NoItemFlags;
}

QVariant PostoptTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return mRootItem->data(section);
    return QVariant();
}

QModelIndex PostoptTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    PostoptTreeItem *parentItem;
    if (!parent.isValid())
        parentItem = mRootItem.get();
    else
        parentItem = static_cast<PostoptTreeItem*>(parent.internalPointer());
    PostoptTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex PostoptTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    auto *childItem = static_cast<PostoptTreeItem*>(index.internalPointer());
    auto *parentItem = childItem->parent();
    if (parentItem == mRootItem)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}

int PostoptTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;
    PostoptTreeItem *parentItem;
    if (!parent.isValid())
        parentItem = mRootItem.get();
    else
        parentItem = static_cast<PostoptTreeItem*>(parent.internalPointer());
    return parentItem->rowCount();
}

int PostoptTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->columnCount(mView);
}

}
}
}
