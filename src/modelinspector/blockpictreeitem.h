#ifndef BLOCKPICKBlockpicTreeItem_H
#define BLOCKPICKBlockpicTreeItem_H

#include <QVariant>
#include <QVector>

namespace gams {
namespace studio{
namespace modelinspector {

class BlockpicTreeItem
{
protected:
    explicit BlockpicTreeItem(BlockpicTreeItem *parent = nullptr);

public:
    ~BlockpicTreeItem();

    void append(BlockpicTreeItem *child);

    BlockpicTreeItem *child(int row);

    const QVector<BlockpicTreeItem*>& childs() const;

    int childCount() const;

    QVariant data(int column) const;
    void setData(const QVector<QVariant> &data);

    BlockpicTreeItem *parent();

    int columns() const;

    int rows() const;

    // int type(); // root, eqn, sym, ...

protected:
    BlockpicTreeItem *mParent;
    QVector<BlockpicTreeItem*> mChilds;
    QVector<QVariant> mData;
};

class BlockpicRootItem : public BlockpicTreeItem
{
public:
    explicit BlockpicRootItem(BlockpicRootItem *parent = nullptr);
};

class BlockpicEqnItem : public BlockpicTreeItem
{
public:
    explicit BlockpicEqnItem(BlockpicTreeItem *parent = nullptr);
};

// TODO rename
class BlockpicUelItem : public BlockpicTreeItem
{
public:
    explicit BlockpicUelItem(BlockpicTreeItem *parent = nullptr);
};

}
}
}

#endif // BLOCKPICKBlockpicTreeItem_H
