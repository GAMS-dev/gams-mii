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

PredefinedViewEnum SectionTreeItem::type() const {
    return mType;
}

void SectionTreeItem::setType(PredefinedViewEnum type) {
    mType = type;
}

void SectionTreeItem::setType(const QString &text)
{
    if (text == constant->Statistic)
        mType = PredefinedViewEnum::Statistic;
    else if (text == constant->EquationAttributes)
        mType = PredefinedViewEnum::EqnAttributes;
    else if (text == constant->VariableAttributes)
        mType = PredefinedViewEnum::VarAttributes;
    else if (text == constant->Jaccobian)
        mType = PredefinedViewEnum::Jaccobian;
    else if (text == constant->FullView)
        mType = PredefinedViewEnum::Full;
    else if (text == constant->MinMax)
        mType = PredefinedViewEnum::MinMax;
    else
        mType = PredefinedViewEnum::Unknown;
}

SectionTreeItem *SectionTreeItem::parent()
{
    return mParent;
}

}
}
}
