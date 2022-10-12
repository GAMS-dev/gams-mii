#include "filtertreeitem.h"

namespace gams {
namespace studio {
namespace modelinspector {

const QString FilterTreeItem::AttributesText = "Attributes";
const QString FilterTreeItem::EquationText = "Equations";
const QString FilterTreeItem::VariableText = "Variables";

FilterTreeItem::FilterTreeItem(const QString &text,
                               Qt::CheckState checkState,
                               FilterTreeItem *parent)
    : mParent(parent)
    , mText(text)
    , mChecked(checkState)
{

}

FilterTreeItem::~FilterTreeItem()
{
    qDeleteAll(mChilds);
}

FilterTreeItem* FilterTreeItem::findChild(const QString &text)
{
    Q_FOREACH(auto item, mChilds) {
        if (!item->text().compare(text, Qt::CaseInsensitive))
            return item;
    }
    return nullptr;
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
{
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
    if (mChilds.isEmpty())
        return mEnabled;
    Q_FOREACH(auto child, mChilds) {
        if (child->isEnabled())
            return true;
    }
    return false;
}

void FilterTreeItem::setEnabled(bool enabled)
{
    mEnabled = enabled;
    Q_FOREACH(auto child, mChilds) {
        child->setEnabled(enabled);
    }
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

int FilterTreeItem::sectionIndex() const
{
    return mSectionIndex;
}

void FilterTreeItem::setSectionIndex(int index)
{
    mSectionIndex = index;
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
