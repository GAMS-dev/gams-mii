#include "sectiontreeitem.h"
#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

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
    if (text == Mi::Jaccobian)
        mType = ViewDataType::Jaccobian;
    else if (text == Mi::BPScaling)
        mType = ViewDataType::BP_Scaling;
    else if (text == Mi::BPOverview)
        mType = ViewDataType::BP_Overview;
    else if (text == Mi::BPCount)
        mType = ViewDataType::BP_Count;
    else if (text == Mi::BPAverage)
        mType = ViewDataType::BP_Average;
    else
        mType = ViewDataType::Unknown;
}

SectionTreeItem *SectionTreeItem::parent()
{
    return mParent;
}

}
}
}
