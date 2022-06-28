#include "sectiontreemodel.h"
#include "sectiontreeitem.h"

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

void SectionTreeModel::appendCustomView(const QString &text, PredefinedViewEnum type, int page)
{
    beginResetModel();
    auto item = new SectionTreeItem(text, page, mCustomRoot);
    item->setType(type);
    mCustomRoot->append(item);
    endResetModel();
}

QVariant SectionTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto *item = static_cast<SectionTreeItem*>(index.internalPointer());
        if (index.column() == 0)
            return item->name();
    }
    return QVariant();
}

bool SectionTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        auto item = static_cast<SectionTreeItem*>(index.internalPointer());
        item->setName(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags SectionTreeModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEditable;
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

bool SectionTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid()) return false;
    auto item = static_cast<SectionTreeItem*>(parent.internalPointer());
    beginRemoveRows(parent, row, row+count);
    item->remove(row, count);
    endRemoveRows();
    return true;
}

void SectionTreeModel::loadModelData()
{
    auto viewItem = new SectionTreeItem("Predefined Views",
                                        (int)PredefinedViewEnum::Unknown,
                                        mRoot);
    mRoot->append(viewItem);

    SectionTreeItem *mainItem;
    for (int i=0; i<PredefinedViewTexts.size(); ++i) {
        if (PredefinedViewTexts.at(i) == Statistic) {
            mainItem = new SectionTreeItem(PredefinedViewTexts.at(i),
                                           (int)PredefinedViewEnum::Statistic,
                                           viewItem);
            mainItem->setType(PredefinedViewEnum::Statistic);
        } else if (PredefinedViewTexts.at(i) == EquationAttributes) {
            mainItem = new SectionTreeItem(PredefinedViewTexts.at(i),
                                           (int)PredefinedViewEnum::EqnAttributes,
                                           viewItem);
            mainItem->setType(PredefinedViewTexts.at(i));
        } else if (PredefinedViewTexts.at(i) == VariableAttributes) {
            mainItem = new SectionTreeItem(PredefinedViewTexts.at(i),
                                           (int)PredefinedViewEnum::VarAttributes,
                                           viewItem);
            mainItem->setType(PredefinedViewTexts.at(i));
        } else if (PredefinedViewTexts.at(i) == Jaccobian)  {
            mainItem = new SectionTreeItem(PredefinedViewTexts.at(i),
                                           (int)PredefinedViewEnum::Jaccobian,
                                           viewItem);
            mainItem->setType(PredefinedViewTexts.at(i));
        } else if (PredefinedViewTexts.at(i) == MinMax) {
            mainItem = new SectionTreeItem(PredefinedViewTexts.at(i),
                                           (int)PredefinedViewEnum::MinMax,
                                           viewItem);
            mainItem->setType(PredefinedViewTexts.at(i));
        } else {
            mainItem = new SectionTreeItem(PredefinedViewTexts.at(i),
                                           (int)PredefinedViewEnum::Full,
                                           viewItem);
            mainItem->setType(PredefinedViewTexts.at(i));
        }
        viewItem->append(mainItem);
    }

    mCustomRoot = new SectionTreeItem("Custom Views",
                                      (int)PredefinedViewEnum::Unknown,
                                      mRoot);
    mRoot->append(mCustomRoot);
}

}
}
}
