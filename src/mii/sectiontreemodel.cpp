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
    , mRoot(new SectionTreeItem({Mi::ModelInstance}, -1))
{

}

SectionTreeModel::~SectionTreeModel()
{
    delete mRoot;
}

void SectionTreeModel::appendCustomView(const QString &text, ViewDataType type, int page)
{
    if (type == ViewDataType::Unknown) {
        return;
    }
    beginResetModel();
    if (type == ViewDataType::Postopt) {
        if (!mCustomPostopt) {
            mCustomPostopt = new SectionTreeItem(Mi::Postopt, mCustomRoot);
            mCustomPostopt->setType(ViewDataType::Postopt);
            mCustomPostopt->setCustom(true);
            mCustomPostopt->setGroup(true);
            mCustomRoot->append(mCustomPostopt);
        }
        auto item = new SectionTreeItem(text, page, mCustomPostopt);
        item->setCustom(true);
        item->setType(type);
        mCustomPostopt->append(item);
    } else if (type == ViewDataType::Symbols) {
        if (!mCustomSymbolView) {
            mCustomSymbolView = new SectionTreeItem(Mi::SymbolView, mCustomRoot);
            mCustomSymbolView->setType(ViewDataType::Symbols);
            mCustomSymbolView->setCustom(true);
            mCustomSymbolView->setGroup(true);
            mCustomRoot->append(mCustomSymbolView);
        }
        auto item = new SectionTreeItem(text, page, mCustomSymbolView);
        item->setCustom(true);
        item->setType(type);
        mCustomSymbolView->append(item);
    } else {
        if (!mCustomBlockpic) {
            mCustomBlockpic = new SectionTreeItem(Mi::Blockpic, mCustomRoot);
            mCustomBlockpic->setType(ViewDataType::Blockpic);
            mCustomBlockpic->setCustom(true);
            mCustomBlockpic->setGroup(true);
            mCustomRoot->append(mCustomBlockpic);
        }
        auto item = new SectionTreeItem(text, page, mCustomBlockpic);
        item->setCustom(true);
        item->setType(type);
        mCustomBlockpic->append(item);
    }
    endResetModel();
}

QVariant SectionTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == ItemDataTypeRole) {
        auto *item = static_cast<SectionTreeItem*>(index.internalPointer());
        return (int)item->type();
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto *item = static_cast<SectionTreeItem*>(index.internalPointer());
        if (index.column() == 0)
            return item->name();
    }
    return QVariant();
}

bool SectionTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto text = value.toString().trimmed();
    if (index.isValid() && role == Qt::EditRole && !text.isEmpty()) {
        auto item = static_cast<SectionTreeItem*>(index.internalPointer());
        item->setName(text);
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

QHash<int, QByteArray> SectionTreeModel::roleNames() const
{
    static QHash<int, QByteArray> mapping {
                                          { ItemDataTypeRole, "itemdata" }
    };
    return mapping;
}

bool SectionTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;
    auto parentItem = static_cast<SectionTreeItem*>(parent.internalPointer());
    if (!parentItem->childCount())
        return false;
    if (parentItem == mCustomRoot) {
        for (int r=row; r<row+count && r<parentItem->childCount(); ++r) {
            if (mCustomBlockpic == parentItem->child(r)) {
                mCustomBlockpic = nullptr;
            } else if (mCustomPostopt == parentItem->child(r)) {
                mCustomPostopt = nullptr;
            } else if (mCustomSymbolView == parentItem->child(r)) {
                mCustomSymbolView = nullptr;
            }
        }
    }
    beginRemoveRows(parent, row, row+count);
    parentItem->remove(row, count);
    endRemoveRows();
    if (parentItem != mCustomRoot && !parentItem->childCount()) {
        if (mCustomBlockpic == parentItem) {
            mCustomBlockpic = nullptr;
        } else if (mCustomPostopt == parentItem) {
            mCustomPostopt = nullptr;
        } else if (mCustomSymbolView == parentItem) {
            mCustomSymbolView = nullptr;
        }
    }
    return true;
}

void SectionTreeModel::loadModelData()
{
    auto predefinedItem = new SectionTreeItem(Mi::PredefinedViews,
                                              (int)ViewDataType::Unknown,
                                              mRoot);
    mRoot->append(predefinedItem);
    auto blockpicItem = new SectionTreeItem(Mi::Blockpic,
                                            (int)ViewDataType::Unknown,
                                            predefinedItem);
    blockpicItem->setGroup(true);
    predefinedItem->append(blockpicItem);
    for (int i=0; i<Mi::PredefinedViewTexts.size(); ++i) {
        if (Mi::PredefinedViewTexts.at(i) == Mi::BPScaling) {
            auto item = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                            (int)ViewDataType::BP_Scaling,
                                            blockpicItem);
            item->setType(Mi::PredefinedViewTexts.at(i));
            blockpicItem->append(item);
        } else if (Mi::PredefinedViewTexts.at(i) == Mi::BPOverview) {
            auto item = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                            (int)ViewDataType::BP_Overview,
                                            blockpicItem);
            item->setType(Mi::PredefinedViewTexts.at(i));
            blockpicItem->append(item);
        } else if (Mi::PredefinedViewTexts.at(i) == Mi::BPCount) {
            auto item = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                            (int)ViewDataType::BP_Count,
                                            blockpicItem);
            item->setType(Mi::PredefinedViewTexts.at(i));
            blockpicItem->append(item);
        } else if (Mi::PredefinedViewTexts.at(i) == Mi::BPAverage) {
            auto item = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                            (int)ViewDataType::BP_Average,
                                            blockpicItem);
            item->setType(Mi::PredefinedViewTexts.at(i));
            blockpicItem->append(item);
        } else if (Mi::PredefinedViewTexts.at(i) == Mi::Postopt) {
            auto item = new SectionTreeItem(Mi::PredefinedViewTexts.at(i),
                                            (int)ViewDataType::Postopt,
                                            predefinedItem);
            item->setType(Mi::PredefinedViewTexts.at(i));
            predefinedItem->append(item);
        }
    }

    mCustomRoot = new SectionTreeItem(Mi::CustomViews,
                                      (int)ViewDataType::Unknown,
                                      mRoot);
    mCustomRoot->setGroup(true);
    mCustomRoot->setCustom(true);
    mRoot->append(mCustomRoot);
}

}
}
}
