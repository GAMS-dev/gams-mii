#ifndef AGGREGATION_H
#define AGGREGATION_H

#include "common.h"

#include <QSet>
#include <QSharedPointer>

namespace gams {
namespace studio {
namespace modelinspector {

typedef QMap<int, QVariant> DataRow;
typedef QMap<int, DataRow> DataMatrix;

typedef QVector<QSet<int>> UnitedSections;

class SymbolInfo;
class LabelTreeItem;

class AggregationItem
{
public:
    QString text() const;
    void setText(const QString &text);

    int symbolIndex() const;
    void setSymbolIndex(int index);

    const IndexCheckStates& checkStates() const;
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
    int mSymbolIndex = -1;
    int mVisibleSectionCount = -1;

    /**
     * @brief CheckState Dimension to Qt::CheckState mapping.
     */
    IndexCheckStates mCheckState;

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

    PredefinedViewEnum viewType() const;
    void setViewType(PredefinedViewEnum viewType);

    int view() const;
    void setView(int view);

    bool isActive() const;

private:
    bool mUseAbsoluteValues = false;
    Type mType = None;
    AggregationMap mAggregationMap;
    IdentifierFilter mIdentifierFilter;
    ValueFilter mValueFilter;
    PredefinedViewEnum mViewType = PredefinedViewEnum::Unknown;
    int mView = -1;

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
                           const LabelCheckStates &globalLabelStates,
                           bool labelFilterAny);

    void aggregate(AggregationItem &item, const QString &type);

private:
    void applyLabelFilter(const IdentifierState &symbolLabelStates,
                          const LabelCheckStates &globalLabelStates,
                          bool labelFilterAny);

    LabelTreeItem* clone(QSharedPointer<LabelTreeItem> labelTree);

    void aggregateLabels(const AggregationItem &item, const QString &type);

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
