#include "filtertreeitem.h"

namespace gams {
namespace studio {
namespace modelinspector {

const QString FilterTreeItem::EquationText = "Equations";
const QString FilterTreeItem::VariableText = "Variables";

FilterTreeItem::FilterTreeItem(const QString &text,
                               bool checkable,
                               FilterTreeItem *parent)
    : FilterTreeItem(text, Qt::Unchecked, -1, parent)
{
    setCheckable(checkable);
}

FilterTreeItem::FilterTreeItem(const QString &text,
                                 Qt::CheckState checkState,
                                 int index,
                                 FilterTreeItem *parent)
    : mParent(parent)
    , mText(text)
    , mChecked(checkState)
    , mIndex(index)
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

FilterTreeItem* FilterTreeItem::child(int index)
{
    if (index < 0 || index >= mChilds.size())
        return nullptr;
    return mChilds.at(index);
}

QList<FilterTreeItem*> FilterTreeItem::childs() const
{
    return mChilds;
}

bool FilterTreeItem::hasChildren() const
{
    return mChilds.size();
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
{// TODO rename index() ???
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<FilterTreeItem*>(this));
    return 0;
}

FilterTreeItem* FilterTreeItem::parent() const
{
    return mParent;
}

void FilterTreeItem::setParent(FilterTreeItem *parent)
{
    mParent = parent;
}

QString FilterTreeItem::text() const
{
    return mText;
}

void FilterTreeItem::setText(const QString &text)
{
    mText = text;
}

bool FilterTreeItem::isCheckable() const
{
    return mCheckable;
}

void FilterTreeItem::setCheckable(bool checkable)
{
    mCheckable = checkable;
}

bool FilterTreeItem::isEnabled() const
{
    return mEnabled;
}

void FilterTreeItem::setEnabled(bool enabled)
{
    mEnabled = enabled;
}

Qt::CheckState FilterTreeItem::checked()
{
    if (mChilds.isEmpty())
        return mChecked;
    int unchecked = 0, checked = 0, disabled = 0;
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
        if (!mChilds[i]->isEnabled())
            ++disabled;
    }
    if (unchecked == mChilds.size())
        return Qt::Unchecked;
    if (checked == mChilds.size() || (disabled && !(unchecked - disabled)))
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
        if (!item->isEnabled() || !item->isCheckable())
            continue;
        item->setChecked(checked);
        subTree.append(item->childs());
    }
}

int FilterTreeItem::index() const
{// TODO this is mostly a sectionIndex, use symbolIndex if it is one and rename to sectionIndex()
    return mIndex;
}

void FilterTreeItem::setIndex(int index)
{
    mIndex = index;
}

int FilterTreeItem::symbolIndex() const
{
    return mSymbolIndex;
}

void FilterTreeItem::setSymbolIndex(int index)
{
    mSymbolIndex = index;
}

}
}
}
