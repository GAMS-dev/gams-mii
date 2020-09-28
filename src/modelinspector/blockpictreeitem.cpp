#include "blockpictreeitem.h"

namespace gams {
namespace studio{
namespace modelinspector {

BlockpicTreeItem::BlockpicTreeItem(BlockpicTreeItem *parent)
    : mParent(parent)
{

}

BlockpicTreeItem::~BlockpicTreeItem()
{
    qDeleteAll(mChilds);
}

void BlockpicTreeItem::append(BlockpicTreeItem *child)
{
    mChilds.append(child);
}

BlockpicTreeItem* BlockpicTreeItem::child(int row)
{
    if (row < 0 || mChilds.size() <= row)
        return nullptr;
    return mChilds.at(row);
}

const QVector<BlockpicTreeItem*>& BlockpicTreeItem::childs() const
{
    return mChilds;
}

int BlockpicTreeItem::childCount() const
{
    return mChilds.count();
}

QVariant BlockpicTreeItem::data(int column) const
{
    if (column < 0 || mData.size() <= column)
        return QVariant();
    return mData.at(column);
}

void BlockpicTreeItem::setData(const QVector<QVariant> &data)
{
    mData = data;
}

BlockpicTreeItem* BlockpicTreeItem::parent()
{
    return mParent;
}

int BlockpicTreeItem::columns() const
{
    return mData.count();
}

int BlockpicTreeItem::rows() const
{
    if (mParent)
        return mParent->childs().indexOf(const_cast<BlockpicTreeItem*>(this));
    return 0;
}




BlockpicRootItem::BlockpicRootItem(BlockpicRootItem *parent)
    : BlockpicTreeItem(parent)
{

}




BlockpicEqnItem::BlockpicEqnItem(BlockpicTreeItem *parent)
    : BlockpicTreeItem(parent)
{

}




BlockpicUelItem::BlockpicUelItem(BlockpicTreeItem *parent)
    : BlockpicTreeItem(parent)
{

}

}
}
}
