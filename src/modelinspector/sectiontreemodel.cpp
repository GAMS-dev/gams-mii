#include "sectiontreemodel.h"

namespace gams {
namespace studio {
namespace modelinspector {

ViewItem::ViewItem(const QString &name, int page, ViewItem *parent)
    : mName(name)
    , mParent(parent)
    , mPage(page)
{

}

ViewItem::~ViewItem()
{
    qDeleteAll(mChilds);
}

void ViewItem::append(ViewItem *child)
{
    mChilds.append(child);
}

ViewItem *ViewItem::child(int row)
{
    if (row < 0 || row >= mChilds.size())
        return nullptr;
    return mChilds.at(row);
}

int ViewItem::childCount() const
{
    return mChilds.count();
}

int ViewItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<ViewItem*>(this));
    return 0;
}

ViewItem *ViewItem::parent()
{
    return mParent;
}

SectionTreeModel::SectionTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , mRoot(new ViewItem({"Model Instance"}, -1))
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

    auto *item = static_cast<ViewItem*>(index.internalPointer());
    if (index.column() == 0)
        return item->name();
    return QVariant();
}

Qt::ItemFlags SectionTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

QVariant SectionTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
        return mRoot->name();
    return QVariant();
}

bool SectionTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                                     const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole &&
            section == 0) {
        mRoot->setName(value.toString());
        return true;
    }

    return false;
}

QModelIndex SectionTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ViewItem *parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = static_cast<ViewItem*>(parent.internalPointer());

    ViewItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex SectionTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto *childItem = static_cast<ViewItem*>(index.internalPointer());
    auto *parentItem = childItem->parent();

    if (parentItem == mRoot)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SectionTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    ViewItem *parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = static_cast<ViewItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int SectionTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<ViewItem*>(parent.internalPointer())->columnCount();
    return mRoot->columnCount();
}

void SectionTreeModel::loadModelData()
{
    QStringList mainItems { "Statistic", "Full View" };

    ViewItem *mainItem;
    for (int i=0; i<mainItems.size(); ++i) {
        if (mainItems.at(i) == "Full View") {
            mainItem = new ViewItem(mainItems.at(i), i, mRoot);
            mainItem->setType(ViewItem::Blockpic);
        } else {
            mainItem = new ViewItem(mainItems.at(i), i, mRoot);
            mainItem->setType(ViewItem::Statistic);
        }
        mRoot->append(mainItem);
    }
}

}
}
}
