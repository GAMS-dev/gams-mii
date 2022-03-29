#ifndef AGGREGATION_H
#define AGGREGATION_H

#include "common.h"

#include <QSet>
#include <QSharedPointer>

namespace gams {
namespace studio {
namespace modelinspector {

typedef QVector<QSet<int>> UnitedSections;

class SymbolInfo;

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

    QList<LabelTreeItem*> visibleChilds() const
    {
        QList<LabelTreeItem*> visible;
        Q_FOREACH(auto c, mChilds) {
            if (c->isVisible())
                visible.append(c);
        }
        return visible;
    }

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
    /// \return List of visible sections.
    ///
    QList<int> visibleSections() const;

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

class AggregationItem
{
public:
    QString text() const;
    void setText(const QString &text);

    int symbolIndex() const;
    void setSymbolIndex(int index);

    const IndexCheckState& checkState() const;
    void setCheckState(int dimension, Qt::CheckState state);
    bool isChecked(int dimension) const;

    QSharedPointer<LabelTreeItem> aggregatedLabelTree() const;
    void setAggregatedLabelTree(QSharedPointer<LabelTreeItem> labelTree);

    QString label(int sectionIndex, int dimension) const;
    void setLabels(const SectionLabels &labels);

    int visibleSectionCount() const;
    void setVisibleSectionCount(int sections);

    bool isSectionVisible(int sectionIndex) const;

    const QList<int>& visibleSections() const;
    void setVisibleSections(const QList<int> &visibleSections);

    const UnitedSections& unitedSections() const;
    void setUnitedSections(const UnitedSections& sections);

private:
    QString mText;
    int mSymbolIndex;
    int mVisibleSectionCount = -1;

    /**
     * @brief CheckState Dimension to Qt::CheckState mapping.
     */
    IndexCheckState mCheckState;

    QSharedPointer<LabelTreeItem> mAggregatedLabelTree = nullptr;

    ///
    /// \brief Labels by section index.
    ///
    /// \remark List index is symbol dimension.
    ///
    SectionLabels mLabels;

    UnitedSections mUnitedSections;

    QList<int> mVisibleSections;
};

typedef QMap<int, AggregationItem> AggregationSymbols;
typedef QMap<Qt::Orientation, AggregationSymbols> AggregationMap;

class Aggregation
{
public:
    enum Type
    {
        Count,
        Mean,
        Median,
        Maximum,
        Minimum,
        Sum,
        None
    };

    bool useAbsoluteValues() const;
    void setUseAbsoluteValues(bool absoluteValues);

    Type type() const;
    void setType(Type type);
    void setType(const QString &text);
    const QString& typeText() const;

    const AggregationMap& aggregationMap() const;
    void setAggregationMap(const AggregationMap &aggregationMap);

    AggregationSymbols aggregationSymbols(Qt::Orientation orientation) const;
    void setAggregationSymbols(Qt::Orientation orientation,
                               const AggregationSymbols &aggrSymbols);

    const IdentifierFilter& identifierFilter() const;
    void setIdentifierFilter(const IdentifierFilter& filter);
    void setIdentifierLabelFilter(const IdentifierFilter& filter);
    void setLabelState(Qt::Orientation orientation,
                       int symIndex, int stateIndex, Qt::CheckState state);

    const ValueFilter& valueFilter() const;
    void setValueFilter(const ValueFilter &filter);

private:
    bool mUseAbsoluteValues = false;
    Type mType = None;
    AggregationMap mAggregationMap;
    IdentifierFilter mIdentifierFilter;
    ValueFilter mValueFilter;

    static const QString CountText;
    static const QString MeanText;
    static const QString MedianText;
    static const QString MaximumText;
    static const QString MinimumText;
    static const QString SumText;
    static const QString NoneText;
};

class Aggregator
{
public:
    Aggregator(const SymbolInfo &symbol);

    void applyFilterStates(const IdentifierState &identifierState,
                           const IdentifierState &symbolLabelStates,
                           const LabelStates &globalLabelStates);

    void aggregate(AggregationItem &item, const QString &type);

private:
    void applyLabelFilter(const IdentifierState &symbolLabelStates,
                          const LabelStates &globalLabelStates);

    LabelTreeItem* clone(QSharedPointer<LabelTreeItem> labelTree);

    void aggregateLabels(const AggregationItem &item, const QString &type);

    SectionLabels sectionLabels(const AggregationItem &item);

    LabelTreeItem* visibleBranch(QList<LabelTreeItem*> &currentLevel,
                                 const QString &type, int dimension);

private:
    const SymbolInfo &mSymbol;
    QSharedPointer<LabelTreeItem> mLabelTree;
};

}
}
}

#endif // AGGREGATION_H
