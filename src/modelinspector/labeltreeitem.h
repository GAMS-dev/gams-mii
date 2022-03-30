#ifndef LABELTREEITEM_H
#define LABELTREEITEM_H

#include "common.h"

#include <QSet>

namespace gams {
namespace studio {
namespace modelinspector {

class LabelTreeItem
{
public:
    LabelTreeItem(LabelTreeItem *parent);

    LabelTreeItem(const QString &text = QString(), LabelTreeItem *parent = nullptr);

    ~LabelTreeItem()
    {
        qDeleteAll(mChilds);
    }

    void append(LabelTreeItem *child)
    {
        mChilds.append(child);
    }

    LabelTreeItem* child(int index);

    const QList<LabelTreeItem*>& childs() const
    {
        return mChilds;
    }

    QList<LabelTreeItem*> visibleChilds() const;

    void setChilds(const QList<LabelTreeItem*> childs)
    {
        mChilds = childs;
    }

    LabelTreeItem* clone(LabelTreeItem* newParent = nullptr) const;

    bool hasChildren() const
    {
        return mChilds.size();
    }

    bool isRoot() const
    {
        return mParent == nullptr;
    }

    void remove(LabelTreeItem *child);

    int firstSectionIndex() const;

    int sectionIndex() const
    {
        return mSectionIndex;
    }

    void setSectionIndex(int index)
    {
        mSectionIndex = index;
    }

    LabelTreeItem* parent() const
    {
        return mParent;
    }

    void setParent(LabelTreeItem *parent)
    {
        mParent = parent;
    }

    QList<LabelTreeItem*> siblings() const
    {
        return mParent ? mParent->childs() : QList<LabelTreeItem*>();
    }

    int size() const
    {
        return mChilds.size();
    }

    QSet<int> sections() const;

    UnitedSections unitedSections() const;

    void setSections(const QSet<int> &sections)
    {
        mSections = sections;
    }

    ///
    /// \brief Visible sections from the view of the source model,
    ///        i.e. childs which are invisible are not considered.
    /// \return List of visible sections, which might be unsorted.
    ///
    QList<int> visibleSections() const;

    QList<int> visibleSectionsSorted() const;

    SectionLabels sectionLabels(int startSection, int dimension) const;

    QString text() const
    {
        return mText;
    }

    void setText(const QString &text)
    {
        mText = text;
    }

    bool isVisible() const;

    void setVisible(bool visible);

    int sectionExtent() const;

    void unite(LabelTreeItem *other);

private:
    void unite(QList<LabelTreeItem*> childs);

private:
    LabelTreeItem *mParent;
    QString mText;
    int mSectionIndex = -1;
    QSet<int> mSections;
    QList<LabelTreeItem*> mChilds;
    bool mIsVisible = true;
};

}
}
}

#endif // LABELTREEITEM_H
