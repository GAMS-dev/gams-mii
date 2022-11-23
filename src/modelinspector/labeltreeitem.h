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

    ~LabelTreeItem();

    void append(LabelTreeItem *child);

    LabelTreeItem* child(int index);

    const QList<LabelTreeItem*>& childs() const;

    QList<LabelTreeItem*> visibleChilds() const;

    void setChilds(const QList<LabelTreeItem*> childs);

    LabelTreeItem* clone(LabelTreeItem* newParent = nullptr) const;

    bool hasChildren() const;

    bool isRoot() const;

    void remove(LabelTreeItem *child);

    int firstSectionIndex() const;

    int sectionIndex() const;

    void setSectionIndex(int index);

    LabelTreeItem* parent() const;

    void setParent(LabelTreeItem *parent);

    ///
    /// \brief All siblings of one level including the
    ///        one this function was called on.
    /// \return All siblings of one level
    ///
    QList<LabelTreeItem*> siblings() const;

    int size() const;

    QSet<int> sections() const;

    UnitedSections unitedSections() const;

    void setSections(const QSet<int> &sections);

    ///
    /// \brief Visible sections from the view of the source model,
    ///        i.e. childs which are invisible are not considered.
    /// \return List of visible sections, which might be unsorted.
    ///
    QList<int> visibleSections() const;

    QList<int> visibleSectionsSorted() const;

    SectionLabels sectionLabels(int startSection, int dimension) const;
    SectionLabels minMaxSectionLabels(const QString &symName, int startSection);

    QString text() const;

    void setText(const QString &text);

    bool isVisible() const;

    void setVisible(bool visible);

    int sectionExtent() const;

    void unite(LabelTreeItem *other);

    static LabelTreeItem* visibleBranch(QList<LabelTreeItem*> &currentLevel,
                                        const QString &typeText, int dimension);

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
