/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023 GAMS Development Corp. <support@gams.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef AGGREGATION_H
#define AGGREGATION_H

#include "common.h"
#include "symbol.h"

#include <QDebug>
#include <QSet>
#include <QSharedPointer>

namespace gams {
namespace studio {
namespace mii {

typedef QVector<QSet<int>> UnitedSections;

class Symbol;
class LabelTreeItem;
class AbstractModelInstance;

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

    ViewHelper::ViewDataType viewType() const;
    void setViewType(ViewHelper::ViewDataType viewType);

    /**
     * @brief Start section mapping equation mapping for MinMax view.
     */
    QHash<int, int>& startSectionMapping() {
        return mStartSectionMapping;
    }

    /**
     * @brief Index to equation mapping for MinMax view.
     * @todo Probably extended to variables.
     */
    QHash<int, Symbol*>& indexToSymbol(Qt::Orientation orientation) {
        return orientation == Qt::Vertical ? mIndexToEquations : mIndexToVariables;
    }

    const QHash<int, Symbol*>& indexToEquation() const {
        return mIndexToEquations;
    }

    const QHash<int, Symbol*>& indexToVariable() const {
        return mIndexToVariables;
    }

    bool isActive() const;

private:
    bool mUseAbsoluteValues = false;
    Type mType = None;
    AggregationMap mAggregationMap;
    ViewHelper::ViewDataType mViewType = ViewHelper::ViewDataType::Unknown;

    ///
    /// \brief Map origial start section to new start section.
    ///
    QHash<int, int> mStartSectionMapping;
    QHash<int, Symbol*> mIndexToEquations;
    QHash<int, Symbol*> mIndexToVariables;

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
    Aggregator(const Symbol *symbol);

    void applyFilterStates(const IdentifierState &identifierStates,
                           const LabelCheckStates &globalLabelStates,
                           bool labelFilterAny);

    void aggregate(AggregationItem &item, const QString &typeText);

private:
    void applyLabelFilter(const IdentifierState &symbolLabelStates,
                          const LabelCheckStates &globalLabelStates,
                          bool labelFilterAny);

    void aggregateLabels(const AggregationItem &item, const QString &typeText);

private:
    const Symbol *mSymbol;
    QSharedPointer<LabelTreeItem> mLabelTree;
};

}
}
}

#endif // AGGREGATION_H
