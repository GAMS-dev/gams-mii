#include "sectiontreemodel.h"
#include "sectiontreeitem.h"
#include "common.h"

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

SectionTreeModel::SectionTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , mRoot(new SectionTreeItem({"Model Instance"}, -1))
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

    auto *item = static_cast<SectionTreeItem*>(index.internalPointer());
    if (index.column() == 0)
        return item->name();
    return QVariant();
}

Qt::ItemFlags SectionTreeModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractItemModel::flags(index);
    return Qt::NoItemFlags;
}

QVariant SectionTreeModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
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

QModelIndex SectionTreeModel::index(int row, int column,
                                    const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    SectionTreeItem *parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = static_cast<SectionTreeItem*>(parent.internalPointer());

    SectionTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex SectionTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto *childItem = static_cast<SectionTreeItem*>(index.internalPointer());
    auto *parentItem = childItem->parent();

    if (parentItem == mRoot)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SectionTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    SectionTreeItem *parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = static_cast<SectionTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int SectionTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<SectionTreeItem*>(parent.internalPointer())->columnCount();
    return mRoot->columnCount();
}

void SectionTreeModel::loadModelData()
{
    auto viewItem = new SectionTreeItem("Predefined Views", mRoot);
    mRoot->append(viewItem);

    SectionTreeItem *mainItem;
    for (int i=0; i<PredefinedViews.size(); ++i) {
        if (PredefinedViews.at(i) == EquationAttributes) {
            mainItem = new SectionTreeItem(PredefinedViews.at(i), 2, viewItem);
            mainItem->setType(PredefinedViews.at(i));
        } else if (PredefinedViews.at(i) == VariableAttributes) {
            mainItem = new SectionTreeItem(PredefinedViews.at(i), 3, viewItem);
            mainItem->setType(PredefinedViews.at(i));
        } else if (PredefinedViews.at(i) == Statistic) {
            mainItem = new SectionTreeItem(PredefinedViews.at(i), 0, viewItem);
            mainItem->setType(PredefinedViewEnum::Statistic);
        } else {
            mainItem = new SectionTreeItem(PredefinedViews.at(i), 1, viewItem);
            mainItem->setType(PredefinedViews.at(i));
        }
        viewItem->append(mainItem);
    }

    viewItem = new SectionTreeItem("Custom Views", mRoot);
    mRoot->append(viewItem);
}

}
}
}
