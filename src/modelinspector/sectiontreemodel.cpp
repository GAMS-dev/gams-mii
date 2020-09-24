#include "sectiontreemodel.h"

namespace gams {
namespace studio {
namespace modelinspector {

TreeItem::TreeItem(const QVector<QVariant> &data,
                   int page, TreeItem *parent)
    : mParent(parent)
    , mData(data)
    , mPage(page)
{

}

TreeItem::~TreeItem()
{
    qDeleteAll(mChilds);
}

void TreeItem::appendChild(TreeItem *child)
{
    mChilds.append(child);
}

TreeItem *TreeItem::child(int row)
{
    if (row < 0 || row >= mChilds.size())
        return nullptr;
    return mChilds.at(row);
}

int TreeItem::childCount() const
{
    return mChilds.count();
}

int TreeItem::columnCount() const
{
    return mData.count();
}

QVariant TreeItem::data(int column) const
{
    if (column < 0 || column >= mData.size())
        return QVariant();
    return mData.at(column);
}

int TreeItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

TreeItem *TreeItem::parent()
{
    return mParent;
}

SectionTreeModel::SectionTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , mRoot(new TreeItem({"Model Instance"}, -1))
{

}

SectionTreeModel::~SectionTreeModel()
{
    delete mRoot;
}

QVariant SectionTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    auto *item = static_cast<TreeItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags SectionTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

QVariant SectionTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{// TODO show model name in header
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return mRoot->data(section);
    return QVariant();
}

QModelIndex SectionTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex SectionTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto *childItem = static_cast<TreeItem*>(index.internalPointer());
    auto *parentItem = childItem->parent();

    if (parentItem == mRoot)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SectionTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    TreeItem *parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int SectionTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return mRoot->columnCount();
}

void SectionTreeModel::loadModelData(const QStringList &data)
{// TODO page/TreeItem to model generator
    for (int i=0; i<data.size(); ++i) {
        mRoot->appendChild(new TreeItem({data.at(i)}, i, mRoot));
    }
}

}
}
}
