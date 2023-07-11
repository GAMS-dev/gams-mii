/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023 GAMS Development Corp. <support@gams.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "sectiontreemodel.h"
#include "sectiontreeitem.h"

#include <QDebug>

namespace gams {
namespace studio {
namespace mii {

SectionTreeModel::SectionTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , mRoot(new SectionTreeItem({"Model Instance"}, -1))
{

}

SectionTreeModel::~SectionTreeModel()
{
    delete mRoot;
}

void SectionTreeModel::appendCustomView(const QString &text, ViewDataType type, int page)
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
    if (!item->childCount()) return false;
    beginRemoveRows(parent, row, row+count);
    item->remove(row, count);
    endRemoveRows();
    return true;
}

void SectionTreeModel::loadModelData()
{
    auto viewItem = new SectionTreeItem("Predefined Views",
                                        (int)ViewDataType::Unknown,
                                        mRoot);
    mRoot->append(viewItem);

    SectionTreeItem *mainItem = nullptr;
    for (int i=0; i<Mi::PredefinedViewTexts.size(); ++i) {
        if (Mi::PredefinedViewTexts.at(i) == Mi::BPScaling) {
            mainItem = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                           (int)ViewDataType::BP_Scaling,
                                           viewItem);
            mainItem->setType(Mi::PredefinedViewTexts.at(i));
        } else if (Mi::PredefinedViewTexts.at(i) == Mi::BPOverview) {
            mainItem = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                           (int)ViewDataType::BP_Overview,
                                           viewItem);
            mainItem->setType(Mi::PredefinedViewTexts.at(i));
        } else if (Mi::PredefinedViewTexts.at(i) == Mi::BPCount) {
            mainItem = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                           (int)ViewDataType::BP_Count,
                                           viewItem);
            mainItem->setType(Mi::PredefinedViewTexts.at(i));
        } else if (Mi::PredefinedViewTexts.at(i) == Mi::BPAverage) {
            mainItem = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                           (int)ViewDataType::BP_Average,
                                           viewItem);
            mainItem->setType(Mi::PredefinedViewTexts.at(i));
        } else if (Mi::PredefinedViewTexts.at(i) == Mi::Postopt) {
            mainItem = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                           (int)ViewDataType::Postopt,
                                           viewItem);
            mainItem->setType(Mi::PredefinedViewTexts.at(i));
        }
        if (mainItem) viewItem->append(mainItem);
    }

    mCustomRoot = new SectionTreeItem("Custom Views",
                                      (int)ViewDataType::Unknown,
                                      mRoot);
    mRoot->append(mCustomRoot);
}

}
}
}
