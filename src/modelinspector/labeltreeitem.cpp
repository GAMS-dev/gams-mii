#include "labeltreeitem.h"

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

LabelTreeItem::LabelTreeItem(LabelTreeItem *parent)
    : mParent(parent)
{

}

LabelTreeItem::LabelTreeItem(const QString &text, LabelTreeItem *parent)
    : mParent(parent)
    , mText(text)
{

}

LabelTreeItem* LabelTreeItem::child(int index)
{
    if (index < 0 || index >= mChilds.size())
        return nullptr;
    return mChilds.at(index);
}

QList<LabelTreeItem*> LabelTreeItem::visibleChilds() const
{
    QList<LabelTreeItem*> visible;
    Q_FOREACH(auto c, mChilds) {
        if (c->isVisible())
            visible.append(c);
    }
    return visible;
}

LabelTreeItem* LabelTreeItem::clone(LabelTreeItem* newParent) const
{
    auto root = new LabelTreeItem(newParent);
    root->setText(mText);
    root->setSectionIndex(mSectionIndex);
    root->setSections(mSections);
    Q_FOREACH(auto child, mChilds) {
        auto newChild = child->clone(root);
        root->append(newChild);
    }
    return root;
}

void LabelTreeItem::remove(LabelTreeItem *child)
{
    mChilds.removeAll(child);
    child->setParent(nullptr);
}

int LabelTreeItem::firstSectionIndex() const
{
    if (mSections.isEmpty())
        return -1;
    auto sections = mSections.values();
    std::sort(sections.begin(), sections.end());
    return sections.first();
}

QSet<int> LabelTreeItem::sections() const
{
    if (!hasChildren())
        return mSections;
    QSet<int> allSections;
    Q_FOREACH(auto child, mChilds) {
        allSections.unite(child->sections());
    }
    return allSections;
}

UnitedSections LabelTreeItem::unitedSections() const
{
    UnitedSections united;
    QList<LabelTreeItem*> items { mChilds };
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        if (item->hasChildren()) {
            items.append(item->childs());
        } else if (item->isVisible()) {
            united.append(item->sections());
        }
    }
    return united;
}

QList<int> LabelTreeItem::visibleSections() const
{
    if (!hasChildren() && isVisible())
        return mSections.values();
    QList<int> allSections;
    Q_FOREACH(auto child, mChilds) {
        if (child->isVisible())
            allSections.append(child->visibleSections());
    }
    return allSections;
}

QList<int> LabelTreeItem::visibleSectionsSorted() const
{
    auto list = visibleSections();
    std::sort(list.begin(), list.end());
    return list;
}

SectionLabels LabelTreeItem::sectionLabels(int startSection, int dimension) const
{// TODO header drawing rules... when to generate empty label cells
    SectionLabels sectionLabels;
    int extent = sectionExtent();
    QList<LabelTreeItem*> levelItems { childs() };
    if (levelItems.isEmpty()) return sectionLabels;
    for (int d=1, s=startSection; d<=dimension; ++d, s=startSection) {
        QList<LabelTreeItem*> newItems;
        int duplicate = extent / levelItems.size();
        while (!levelItems.isEmpty()) {
            auto item = levelItems.takeFirst();
            if (!item->isVisible())
                continue;
            newItems.append(item->visibleChilds());
            for (int repeat=0; repeat<duplicate; ++repeat) {
                auto data = sectionLabels[s];
                data << item->text();
                sectionLabels[s] = data;
                ++s;
            }
        }
        levelItems = newItems;
    }
    return sectionLabels;
}

bool LabelTreeItem::isVisible() const
{
    if (!hasChildren())
        return mIsVisible;
    Q_FOREACH(auto child, mChilds) {
        if (child->isVisible()) {
            return true;
        }
    }
    return false;
}

void LabelTreeItem::setVisible(bool visible)
{
    mIsVisible = visible;
    Q_FOREACH(auto child, mChilds) {
        child->setVisible(visible);
    }
}

int LabelTreeItem::sectionExtent() const
{
    QList<LabelTreeItem*> leafs;
    QList<LabelTreeItem*> items { mChilds };
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        if (!item->isVisible())
            continue;
        items.append(item->childs());
        if (!item->hasChildren())
            leafs.append(item);
    }
    return leafs.size();
}

void LabelTreeItem::unite(LabelTreeItem *other)
{
    if (!other)
        return;
    if (hasChildren()) {
        unite(other->childs());
    } else {
        auto visible = other->visibleSections();
        mSections.unite(QSet<int>(visible.begin(), visible.end()));
    }
}

void LabelTreeItem::unite(QList<LabelTreeItem*> childs)
{
    QList<LabelTreeItem*> invisible;
    QHash<QString, LabelTreeItem*> unified;
    Q_FOREACH(auto child, mChilds) {
        if (child->isVisible()) {
            unified[child->text()] = child;
        } else {
            child->setParent(nullptr);
            invisible.append(child);
        }
    }
    qDeleteAll(invisible);
    Q_FOREACH(auto child, childs) {
        if (!child->isVisible())
            continue;
        if (unified.contains(child->text())) {
            unified[child->text()]->unite(child);
        } else {
            auto newChild = child->clone(this);
            unified[newChild->text()] = newChild;
        }
    }
    QMap<int, LabelTreeItem*> newChilds;
    Q_FOREACH(auto child, unified) {
        auto firstSection = child->firstSectionIndex();
        if (firstSection < 0) continue;
        newChilds[firstSection] = child;
    }
    mChilds = newChilds.values();
}

}
}
}
