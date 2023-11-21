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
#include "sectiontreeitem.h"
#include "common.h"

namespace gams {
namespace studio {
namespace mii {

SectionTreeItem::SectionTreeItem(const QString &name,
                                 SectionTreeItem *parent)
    : mName(name)
    , mParent(parent)
{

}

SectionTreeItem::SectionTreeItem(const QString &name,
                                 AbstractViewFrame* widget,
                                 SectionTreeItem *parent)
    : mName(name)
    , mParent(parent)
    , mWidget(widget)
{

}

SectionTreeItem::~SectionTreeItem()
{
    qDeleteAll(mChilds);
}

void SectionTreeItem::append(SectionTreeItem *child)
{
    mChilds.append(child);
}

QList<AbstractViewFrame*> SectionTreeItem::removeChilds()
{
    QList<AbstractViewFrame*> wgts;
    if (isGroup()) {
        while (mChilds.count()) {
            auto child = mChilds.takeFirst();
            wgts.append(child->removeChilds());
            delete child;
        }
        return wgts;
    }
    if (widget()) {
        wgts.append(widget());
    }
    return wgts;
}

void SectionTreeItem::remove(SectionTreeItem *child)
{
    if (!child)
        return;
    mChilds.removeOne(child);
    delete child;
}

void SectionTreeItem::remove(int index, int count)
{
    if (index+count > mChilds.size())
        return;
    QVector<SectionTreeItem*> items;
    for (int i=index; i<index+count; ++i) {
        items.push_back(mChilds[i]);
    }
    mChilds.remove(index, count);
    qDeleteAll(items);
}

SectionTreeItem *SectionTreeItem::child(int row)
{
    if (row < 0 || row >= mChilds.size())
        return nullptr;
    return mChilds.at(row);
}

int SectionTreeItem::childCount() const
{
    return mChilds.count();
}

int SectionTreeItem::columnCount() const {
    return 1;
}

QString SectionTreeItem::name() const {
    return mName;
}

void SectionTreeItem::setName(const QString &name) {
    mName = name;
}

AbstractViewFrame* SectionTreeItem::widget() const {
    return mWidget;
}

QList<AbstractViewFrame*> SectionTreeItem::widgets() const
{
    QList<AbstractViewFrame*> wgts;
    if (isGroup()) {
        for (auto child : mChilds) {
            wgts.append(child->widgets());
        }
        return wgts;
    }
    if (widget()) {
        wgts.append(widget());
    }
    return wgts;
}

void SectionTreeItem::setWidget(AbstractViewFrame* page) {
    mWidget = page;
}

int SectionTreeItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<SectionTreeItem*>(this));
    return 0;
}

ViewHelper::ViewDataType SectionTreeItem::type() const {
    return mType;
}

void SectionTreeItem::setType(ViewHelper::ViewDataType type) {
    mType = type;
}

void SectionTreeItem::setType(const QString &text)
{
    if (text == ViewHelper::BPScaling)
        mType = ViewHelper::ViewDataType::BP_Scaling;
    else if (text == ViewHelper::BPOverview)
        mType = ViewHelper::ViewDataType::BP_Overview;
    else if (text == ViewHelper::BPCount)
        mType = ViewHelper::ViewDataType::BP_Count;
    else if (text == ViewHelper::BPAverage)
        mType = ViewHelper::ViewDataType::BP_Average;
    else if (text == ViewHelper::Postopt)
        mType = ViewHelper::ViewDataType::Postopt;
    else if (text == ViewHelper::SymbolView)
        mType = ViewHelper::ViewDataType::Symbols;
    else if (text == ViewHelper::Blockpic)
        mType = ViewHelper::ViewDataType::Blockpic;
    else
        mType = ViewHelper::ViewDataType::Unknown;
}

bool SectionTreeItem::isCustom() const
{
    return mCustom;
}

void SectionTreeItem::setCustom(bool custom)
{
    mCustom = custom;
}

bool SectionTreeItem::isGroup() const
{
    return mGroup;
}

void SectionTreeItem::setGroup(bool group)
{
    mGroup = group;
}

SectionTreeItem *SectionTreeItem::parent() const
{
    return mParent;
}

void SectionTreeItem::setParent(SectionTreeItem *parent)
{
    mParent = parent;
}

}
}
}
