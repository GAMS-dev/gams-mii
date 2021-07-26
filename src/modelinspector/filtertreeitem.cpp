#include "filtertreeitem.h"

namespace gams {
namespace studio {
namespace modelinspector {

const QString FilterTreeItem::HeaderText = "Uels";

FilterTreeItem::FilterTreeItem(const QString &text,
                                 Qt::CheckState checkState,
                                 int logicalIndex,
                                 FilterTreeItem *parent)
    : mParent(parent)
    , mText(text)
    , mChecked(checkState)
    , mLogicalIndex(logicalIndex)
{

}

FilterTreeItem::~FilterTreeItem()
{
    qDeleteAll(mChilds);
}

void FilterTreeItem::append(FilterTreeItem *child)
{
    mChilds.append(child);
}

FilterTreeItem* FilterTreeItem::child(int row)
{
    if (row < 0 || row >= mChilds.size())
        return nullptr;
    return mChilds.at(row);
}

QList<FilterTreeItem*> FilterTreeItem::childs() const
{
    return mChilds;
}

int FilterTreeItem::columnCount() const
{
    return 1;
}

int FilterTreeItem::rowCount() const
{
    return mChilds.size();
}

int FilterTreeItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<FilterTreeItem*>(this));
    return 0;
}

FilterTreeItem* FilterTreeItem::parent()
{
    return mParent;
}

QString FilterTreeItem::text() const
{
    return mText;
}

bool FilterTreeItem::isCheckable() const
{
    return mCheckable;
}

void FilterTreeItem::setCheckable(bool checkable)
{
    mCheckable = checkable;
}

Qt::CheckState FilterTreeItem::checked()
{
    if (mChilds.isEmpty())
        return mChecked;
    int unchecked = 0, checked = 0;
    for (int i=0; i<mChilds.size(); ++i) {
        switch (mChilds[i]->checked()) {
            case Qt::Unchecked:
                ++unchecked;
                break;
            case Qt::Checked:
                ++checked;
                break;
            default:
                return Qt::PartiallyChecked;
        }
    }
    if (unchecked == mChilds.size())
        return Qt::Unchecked;
    if (checked == mChilds.size())
        return Qt::Checked;
    return Qt::PartiallyChecked;
}

void FilterTreeItem::setChecked(Qt::CheckState state)
{
    mChecked = state;
}

void FilterTreeItem::setSubTreeState(Qt::CheckState checked)
{
    QList<FilterTreeItem*> subTree(mChilds);
    while (!subTree.isEmpty()) {
        auto item = subTree.takeFirst();
        item->setChecked(checked);
        subTree.append(item->childs());
    }
}

int FilterTreeItem::logicalIndex() const
{
    return mLogicalIndex;
}

void FilterTreeItem::setLogicalIndex(int index)
{
    mLogicalIndex = index;
}

}
}
}
