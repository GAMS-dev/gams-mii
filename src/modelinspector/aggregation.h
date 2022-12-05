#ifndef AGGREGATION_H
#define AGGREGATION_H

#include "common.h"
#include "symbol.h"

#include <QSet>
#include <QSharedPointer>

namespace gams {
namespace studio {
namespace modelinspector {

typedef QMap<int, QVariant> DataRow;
typedef QMap<int, DataRow> DataMatrix;

typedef QVector<QSet<int>> UnitedSections;

class Symbol;
class LabelTreeItem;

class AggregationItem
{
public:
    QString text() const;
    void setText(const QString &text);

    int symbolIndex() const;
    void setSymbolIndex(int index);

    int newSymbolIndex() const;

    const IndexCheckStates& checkStates() const;
    void setCheckState(int dimension, Qt::CheckState state);
    bool isChecked(int dimension) const;

    QSharedPointer<LabelTreeItem> aggregatedLabelTree() const;
    void setAggregatedLabelTree(QSharedPointer<LabelTreeItem> labelTree);

    QString label(int sectionIndex, int dimension) const;
    const SectionLabels& labels() const;
    void setLabels(const SectionLabels &labels);

    const QList<int>& mappedSections() const;
    void setMappedSections(const QList<int> &visibleSections);

    int visibleSectionCount() const;
    void setVisibleSectionCount(int sections);

    bool isSectionVisible(int sectionIndex) const;

    const QList<int>& visibleSections() const;
    void setVisibleSections(const QList<int> &visibleSections);

    const UnitedSections& unitedSections() const;
    void setUnitedSections(const UnitedSections& sections);

    QString domainLabel(int dimension) const;
    void setDomainLabels(const DomainLabels &labels);

    bool isScalar() const {
        return mScalar;
    }

    void setScalar(bool scalar) {
        mScalar = scalar;
    }

private:
    QString mText;
    int mSymbolIndex = -1;
    int mVisibleSectionCount = -1;
    bool mScalar = false;

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

    QList<int> mMappedSections;
    QList<int> mVisibleSections;

    DomainLabels mDomainLabels;
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
        MinMax,
        Sum,
        None
    };

    bool useAbsoluteValues() const;
    void setUseAbsoluteValues(bool absoluteValues);
    bool useAbsoluteValuesGlobal() const;
    void setUseAbsoluteValuesGlobal(bool absoluteValues);

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
    void setLabelState(Qt::Orientation orientation,
                       int symIndex, int stateIndex, Qt::CheckState state);

    const ValueFilter& valueFilter() const;
    void setValueFilter(const ValueFilter &filter);

    PredefinedViewEnum viewType() const;
    void setViewType(PredefinedViewEnum viewType);

    /**
     * @brief Start section mapping equation mapping for MinMax view.
     */
    QMap<int, int>& startSectionMapping() {
        return mStartSectionMapping;
    }

    /**
     * @brief Index to equation mapping for MinMax view.
     * @todo Probably extended to variables.
     */
    QMap<int, Symbol>& indexToSymbol(Qt::Orientation orientation) {
        return orientation == Qt::Vertical ? mIndexToEquations : mIndexToVariables;
    }

    const QMap<int, Symbol>& indexToEquation() const {
        return mIndexToEquations;
    }

    const QMap<int, Symbol>& indexToVariable() const {
        return mIndexToVariables;
    }

    int view() const;
    void setView(int view);

    bool isActive() const;

private:
    bool mUseAbsoluteValues = false;
    bool mUseAbsoluteValuesGlobal = false;
    Type mType = None;
    AggregationMap mAggregationMap;
    IdentifierFilter mIdentifierFilter;
    ValueFilter mValueFilter;
    PredefinedViewEnum mViewType = PredefinedViewEnum::Unknown;
    int mView = -1;

    ///
    /// \brief Map origial start section to new start section.
    ///
    QMap<int, int> mStartSectionMapping;
    QMap<int, Symbol> mIndexToEquations;
    QMap<int, Symbol> mIndexToVariables;

    static const QString CountText;
    static const QString MeanText;
    static const QString MedianText;
    static const QString MaximumText;
    static const QString MinimumText;
    static const QString MinMaxText;
    static const QString SumText;
    static const QString NoneText;
};

class Aggregator
{
public:
    Aggregator(const Symbol &symbol);

    void applyFilterStates(const IdentifierState &identifierStates,
                           const LabelCheckStates &globalLabelStates,
                           bool labelFilterAny);

    void aggregate(AggregationItem &item,
                   Aggregation::Type type,
                   const QString &typeText,
                   int &lastSymEndIndex);

private:
    void applyLabelFilter(const IdentifierState &symbolLabelStates,
                          const LabelCheckStates &globalLabelStates,
                          bool labelFilterAny);

    void aggregateLabels(const AggregationItem &item,
                         Aggregation::Type type,
                         const QString &typeText,
                         int &lastSymEndIndex);

    void aggregateLabels(const AggregationItem &item, const QString &typeText);

private:
    const Symbol &mSymbol;
    QSharedPointer<LabelTreeItem> mLabelTree;
};

}
}
}

#endif // AGGREGATION_H
