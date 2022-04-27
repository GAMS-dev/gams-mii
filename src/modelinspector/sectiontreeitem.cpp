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

int SectionTreeItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<SectionTreeItem*>(this));
    return 0;
}

void SectionTreeItem::setType(const QString &text)
{
    if (text == Statistic)
        mType = PredefinedViewEnum::Statistic;
    else if (text == EquationAttributes || text == VariableAttributes )
        mType = PredefinedViewEnum::Attributes;
    else if (text == Jaccobian)
        mType = PredefinedViewEnum::Jaccobian;
    else if (text == FullView)
        mType = PredefinedViewEnum::Full;
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
