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


SectionTreeItem::SectionTreeItem(const QString &name, int page,
                                 SectionTreeItem *parent)
    : mName(name)
    , mParent(parent)
    , mPage(page)
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

int SectionTreeItem::page() const {
    return mPage;
}

void SectionTreeItem::setPage(int page) {
    mPage = page;
}

int SectionTreeItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<SectionTreeItem*>(this));
    return 0;
}

ViewDataType SectionTreeItem::type() const {
    return mType;
}

void SectionTreeItem::setType(ViewDataType type) {
    mType = type;
}

void SectionTreeItem::setType(const QString &text)
{
    if (text == Mi::BPScaling)
        mType = ViewDataType::BP_Scaling;
    else if (text == Mi::BPOverview)
        mType = ViewDataType::BP_Overview;
    else if (text == Mi::BPCount)
        mType = ViewDataType::BP_Count;
    else if (text == Mi::BPAverage)
        mType = ViewDataType::BP_Average;
    else if (text == Mi::Postopt)
        mType = ViewDataType::Postopt;
    else if (text == Mi::SymbolView)
        mType = ViewDataType::Symbols;
    else if (text == Mi::Blockpic)
        mType = ViewDataType::Blockpic;
    else
        mType = ViewDataType::Unknown;
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
