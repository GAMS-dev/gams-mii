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
#include "abstractviewframe.h"

#include <QStackedWidget>

namespace gams {
namespace studio {
namespace mii {

SectionTreeModel::SectionTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , mRoot(new SectionGroupTreeItem({ViewHelper::ModelInstance}))
    , mCustomRoot(nullptr)
    , mPredefinedRoot(nullptr)
    , mCustomBlockpic(nullptr)
    , mCustomPostopt(nullptr)
    , mCustomSymbolView(nullptr)
{

}

SectionTreeModel::~SectionTreeModel()
{
    delete mRoot;
}

void SectionTreeModel::appendCustomView(const QString &text,
                                        ViewHelper::ViewDataType type,
                                        AbstractViewFrame* widget)
{
    if (type == ViewHelper::ViewDataType::Unknown) {
        return;
    }
    beginResetModel();
    if (type == ViewHelper::ViewDataType::Postopt) {
        if (!mCustomPostopt) {
            mCustomPostopt = new SectionGroupTreeItem(ViewHelper::Postopt, mCustomRoot);
            mCustomPostopt->setType(ViewHelper::ViewDataType::Postopt);
            mCustomPostopt->setCustom(true);
            mCustomRoot->append(mCustomPostopt);
        }
        auto item = new SectionTreeItem(text, widget, mCustomPostopt);
        item->setCustom(true);
        item->setType(type);
        mCustomPostopt->append(item);
    } else if (type == ViewHelper::ViewDataType::Symbols) {
        if (!mCustomSymbolView) {
            mCustomSymbolView = new SectionGroupTreeItem(ViewHelper::SymbolView, mCustomRoot);
            mCustomSymbolView->setType(ViewHelper::ViewDataType::Symbols);
            mCustomSymbolView->setCustom(true);
            mCustomRoot->append(mCustomSymbolView);
        }
        auto item = new SectionTreeItem(text, widget, mCustomSymbolView);
        item->setCustom(true);
        item->setType(type);
        mCustomSymbolView->append(item);
    } else {
        if (!mCustomBlockpic) {
            mCustomBlockpic = new SectionGroupTreeItem(ViewHelper::Blockpic, mCustomRoot);
            mCustomBlockpic->setType(ViewHelper::ViewDataType::Blockpic);
            mCustomBlockpic->setCustom(true);
            mCustomRoot->append(mCustomBlockpic);
        }
        auto item = new SectionTreeItem(text, widget, mCustomBlockpic);
        item->setCustom(true);
        item->setType(type);
        mCustomBlockpic->append(item);
    }
    endResetModel();
}

AbstractSectionTreeItem *SectionTreeModel::data() const
{
    return mRoot;
}

AbstractSectionTreeItem *SectionTreeModel::customItems() const
{
    return mCustomRoot;
}

AbstractSectionTreeItem *SectionTreeModel::predefinedItems() const
{
    return mPredefinedRoot;
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
            return item->text();
    }
    return QVariant();
}

bool SectionTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto text = value.toString().trimmed();
    if (index.isValid() && role == Qt::EditRole && !text.isEmpty()) {
        auto item = static_cast<SectionTreeItem*>(index.internalPointer());
        item->setText(text);
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
        return mRoot->text();
    return QVariant();
}

bool SectionTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                                     const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole &&
            section == 0) {
        mRoot->setText(value.toString());
        return true;
    }
    return false;
}

QModelIndex SectionTreeModel::index(int row, int column,
                                    const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    auto parentItem = parent.isValid() ? static_cast<AbstractSectionTreeItem*>(parent.internalPointer()) : mRoot;

    AbstractSectionTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex SectionTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    auto *childItem = static_cast<AbstractSectionTreeItem*>(index.internalPointer());
    auto *parentItem = childItem->parent();
    if (parentItem == mRoot)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}

int SectionTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;
    auto parentItem = parent.isValid() ? static_cast<AbstractSectionTreeItem*>(parent.internalPointer()) : mRoot;
    return parentItem->childCount();
}

int SectionTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<AbstractSectionTreeItem*>(parent.internalPointer())->columnCount();
    return mRoot->columnCount();
}

QHash<int, QByteArray> SectionTreeModel::roleNames() const
{
    static QHash<int, QByteArray> mapping { { ItemDataTypeRole, "itemdata" } };
    return mapping;
}

bool SectionTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;
    auto parentItem = static_cast<AbstractSectionTreeItem*>(parent.internalPointer());
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
    beginResetModel();
    parentItem->remove(row, count);
    endResetModel();
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

QList<AbstractViewFrame*> SectionTreeModel::removeItem(AbstractSectionTreeItem *item)
{
    if (!item)
        return QList<AbstractViewFrame*>();
    beginResetModel();
    auto wgts = item->removeChilds();
    if (item == mCustomRoot) {
        mCustomBlockpic = nullptr;
        mCustomPostopt = nullptr;
        mCustomSymbolView = nullptr;
    } else {
        if (item == mCustomBlockpic) {
            mCustomBlockpic = nullptr;
        } else if (item == mCustomPostopt) {
            mCustomPostopt = nullptr;
        } else if (item == mCustomSymbolView) {
            mCustomSymbolView = nullptr;
        }
        auto parent = item->parent();
        parent->remove(item);
    }
    endResetModel();
    return wgts;
}

QList<AbstractViewFrame*> SectionTreeModel::removeCustomRows()
{
    beginResetModel();
    auto wgts = mCustomRoot->removeChilds();
    mCustomBlockpic = nullptr;
    mCustomPostopt = nullptr;
    mCustomSymbolView = nullptr;
    endResetModel();
    return wgts;
}

void SectionTreeModel::clearModelData()
{
    beginResetModel();
    mCustomRoot = nullptr;
    mPredefinedRoot = nullptr;
    mCustomBlockpic = nullptr;
    mCustomPostopt = nullptr;
    mCustomSymbolView = nullptr;
    mRoot->removeAllChilds();
    endResetModel();
}

void SectionTreeModel::loadModelData(QStackedWidget *stackedWidget)
{
    mPredefinedRoot = new SectionGroupTreeItem(ViewHelper::PredefinedViews, mRoot);
    mRoot->append(mPredefinedRoot);
    auto blockpicItem = new SectionGroupTreeItem(ViewHelper::Blockpic, mPredefinedRoot);
    mPredefinedRoot->append(blockpicItem);
    for (int i=0; i<ViewHelper::PredefinedViewTexts.size(); ++i) {
        if (ViewHelper::PredefinedViewTexts.at(i) == ViewHelper::BPScaling) {
            auto widget = stackedWidget->widget((int)ViewHelper::ViewDataType::BP_Scaling);
            auto item = new SectionTreeItem(ViewHelper::PredefinedViewTexts.at(i),
                                            static_cast<AbstractViewFrame*>(widget->children().last()),
                                            blockpicItem);
            item->setType(ViewHelper::PredefinedViewTexts.at(i));
            blockpicItem->append(item);
        } else if (ViewHelper::PredefinedViewTexts.at(i) == ViewHelper::BPOverview) {
            auto widget = stackedWidget->widget((int)ViewHelper::ViewDataType::BP_Overview);
            auto item = new SectionTreeItem(ViewHelper::PredefinedViewTexts.at(i),
                                            static_cast<AbstractViewFrame*>(widget->children().last()),
                                            blockpicItem);
            item->setType(ViewHelper::PredefinedViewTexts.at(i));
            blockpicItem->append(item);
        } else if (ViewHelper::PredefinedViewTexts.at(i) == ViewHelper::BPCount) {
            auto widget = stackedWidget->widget((int)ViewHelper::ViewDataType::BP_Count);
            auto item = new SectionTreeItem(ViewHelper::PredefinedViewTexts.at(i),
                                            static_cast<AbstractViewFrame*>(widget->children().last()),
                                            blockpicItem);
            item->setType(ViewHelper::PredefinedViewTexts.at(i));
            blockpicItem->append(item);
        } else if (ViewHelper::PredefinedViewTexts.at(i) == ViewHelper::BPAverage) {
            auto widget = stackedWidget->widget((int)ViewHelper::ViewDataType::BP_Average);
            auto item = new SectionTreeItem(ViewHelper::PredefinedViewTexts.at(i),
                                            static_cast<AbstractViewFrame*>(widget->children().last()),
                                            blockpicItem);
            item->setType(ViewHelper::PredefinedViewTexts.at(i));
            blockpicItem->append(item);
        } else if (ViewHelper::PredefinedViewTexts.at(i) == ViewHelper::Postopt) {
            auto widget = stackedWidget->widget((int)ViewHelper::ViewDataType::Postopt);
            auto item = new SectionTreeItem(ViewHelper::PredefinedViewTexts.at(i),
                                            static_cast<AbstractViewFrame*>(widget->children().last()),
                                            mPredefinedRoot);
            item->setType(ViewHelper::PredefinedViewTexts.at(i));
            mPredefinedRoot->append(item);
        }
    }
    mCustomRoot = new SectionGroupTreeItem(ViewHelper::CustomViews, mRoot);
    mCustomRoot->setCustom(true);
    mRoot->append(mCustomRoot);
}

}
}
}
