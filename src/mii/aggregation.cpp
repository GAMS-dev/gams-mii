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
#include "aggregation.h"
#include "symbol.h"
#include "labeltreeitem.h"

#include <algorithm>

#include <QDebug>

namespace gams {
namespace studio {
namespace mii {

QString AggregationItem::text() const
{
    return mText;
}

void AggregationItem::setText(const QString &text)
{
    mText = text;
}

int AggregationItem::symbolIndex() const
{
    return mSymbolIndex;
}

void AggregationItem::setSymbolIndex(int index)
{
    mSymbolIndex = index;
}

int AggregationItem::newSymbolIndex() const
{
    return mAggregatedLabelTree ? mAggregatedLabelTree->sectionIndex() : -1;
}

const IndexCheckStates& AggregationItem::checkStates() const
{
    return mCheckState;
}

void AggregationItem::setCheckState(int dimension, Qt::CheckState state)
{
    mCheckState[dimension] = state;
}

bool AggregationItem::isChecked(int dimension) const
{
    return mCheckState[dimension] == Qt::Checked;
}

const QSharedPointer<LabelTreeItem>& AggregationItem::aggregatedLabelTree() const
{
    return mAggregatedLabelTree;
}

void AggregationItem::setAggregatedLabelTree(const QSharedPointer<LabelTreeItem> &labelTree)
{
    mAggregatedLabelTree = labelTree;
}

QString AggregationItem::label(int sectionIndex, int dimension) const
{
    if (!mLabels.contains(sectionIndex))
        return QString();
    auto data = mLabels[sectionIndex];
    if (dimension >= 0 && dimension < data.size())
        return data[dimension];
    return QString();
}

const SectionLabels &AggregationItem::labels() const
{
    return mLabels;
}

void AggregationItem::setLabels(const SectionLabels &labels)
{
    mLabels = labels;
}

const QList<int>& AggregationItem::mappedSections() const
{
    return mMappedSections;
}

void AggregationItem::setMappedSections(const QList<int> &visibleSections)
{
    mMappedSections = visibleSections;
}

int AggregationItem::visibleSectionCount() const
{
    return mVisibleSectionCount;
}

void AggregationItem::setVisibleSectionCount(int sections)
{
    mVisibleSectionCount = sections;
}

bool AggregationItem::isSectionVisible(int sectionIndex) const
{
    int index = mVisibleSections.indexOf(sectionIndex);
    if (index >= 0 && index < mVisibleSectionCount)
        return true;
    return false;
}

const QList<int>& AggregationItem::visibleSections() const
{
    return mVisibleSections;
}

void AggregationItem::setVisibleSections(const QList<int> &visibleSections)
{
    mVisibleSections = visibleSections;
}

const UnitedSections& AggregationItem::unitedSections() const
{
    return mUnitedSections;
}

void AggregationItem::setUnitedSections(const UnitedSections& unitedSections)
{
    mUnitedSections = unitedSections;
}

QString AggregationItem::domainLabel(int dimension) const
{
    return dimension >= 0 && dimension < mDomainLabels.size() ?
                mDomainLabels[dimension] : QString();
}

void AggregationItem::setDomainLabels(const DomainLabels &labels)
{
    mDomainLabels = labels;
}

const QString Aggregation::CountText = "Count";
const QString Aggregation::MeanText = "Mean";
const QString Aggregation::MedianText = "Median";
const QString Aggregation::MaximumText = "Maximum";
const QString Aggregation::MinimumText = "Minimum";
const QString Aggregation::MinMaxText = "MinMax";
const QString Aggregation::SumText = "Sum";
const QString Aggregation::NoneText = "None";

bool Aggregation::useAbsoluteValues() const
{
    return mUseAbsoluteValues;
}

void Aggregation::setUseAbsoluteValues(bool absoluteValues)
{
    mUseAbsoluteValues = absoluteValues;
}

Aggregation::Type Aggregation::type() const
{
    return mType;
}

void Aggregation::setType(Type type)
{
    mType = type;
}

void Aggregation::setType(const QString &text)
{
    if (!text.compare(CountText, Qt::CaseInsensitive)) {
        mType = Count;
    } else if (!text.compare(MeanText, Qt::CaseInsensitive)) {
        mType = Mean;
    } else if (!text.compare(MedianText, Qt::CaseInsensitive)) {
        mType = Median;
    } else if (!text.compare(MaximumText, Qt::CaseInsensitive)) {
        mType = Maximum;
    } else if (!text.compare(MinimumText, Qt::CaseInsensitive)) {
        mType = Minimum;
    } else if (!text.compare(SumText, Qt::CaseInsensitive)) {
        mType = Sum;
    } else if (!text.compare(NoneText, Qt::CaseInsensitive)) {
        mType = None;
    }
}

const QString& Aggregation::typeText() const
{
    switch (mType) {
    case Count:
        return CountText;
    case Mean:
        return MeanText;
    case Median:
        return MedianText;
    case Maximum:
        return MaximumText;
    case Minimum:
        return MinimumText;
    case Sum:
        return SumText;
    default:
        return NoneText;
    }
}

const AggregationMap& Aggregation::aggregationMap() const
{
    return mAggregationMap;
}

void Aggregation::setAggregationMap(const AggregationMap &aggregationMap)
{
    mAggregationMap = aggregationMap;
}

AggregationSymbols Aggregation::aggregationSymbols(Qt::Orientation orientation) const
{
    return mAggregationMap[orientation];
}

void Aggregation::setAggregationSymbols(Qt::Orientation orientation,
                                        const AggregationSymbols &aggrSymbols)
{
    mAggregationMap[orientation] = aggrSymbols;
}

ViewHelper::ViewDataType Aggregation::viewType() const
{
    return mViewType;
}

void Aggregation::setViewType(ViewHelper::ViewDataType viewType)
{
    mViewType = viewType;
}

bool Aggregation::isActive() const
{
    return !mAggregationMap[Qt::Horizontal].isEmpty() ||
            !mAggregationMap[Qt::Vertical].isEmpty();
}

Aggregator::Aggregator(const Symbol *symbol)
    : mSymbol(symbol)
{
    mLabelTree = QSharedPointer<LabelTreeItem>(mSymbol->labelTree()->clone());
}

void Aggregator::applyFilterStates(const IdentifierState &identifierStates,
                                   const LabelCheckStates &globalLabelStates,
                                   bool labelFilterAny)
{
    mLabelTree->setVisible(identifierStates.Checked == Qt::Checked);
    applyLabelFilter(identifierStates, globalLabelStates, labelFilterAny);
}

void Aggregator::aggregate(AggregationItem &item, const QString &typeText)
{
    item.setVisibleSections(mLabelTree->visibleSectionsSorted());
    aggregateLabels(item, typeText);
    item.setAggregatedLabelTree(mLabelTree);
    item.setVisibleSectionCount(mLabelTree->sectionExtent());
    auto labels = mLabelTree->sectionLabels(mSymbol->firstSection(), mSymbol->dimension());
    item.setLabels(labels);
    item.setUnitedSections(mLabelTree->unitedSections());
}

void Aggregator::applyLabelFilter(const IdentifierState &symbolLabelStates,
                                  const LabelCheckStates &globalLabelStates,
                                  bool labelFilterAny)
{
    QList<LabelTreeItem*> items { mLabelTree->childs() };
    IndexCheckStates symLabelStates = symbolLabelStates.isValid() ? symbolLabelStates.CheckStates
                                                                  : IndexCheckStates();
    if (labelFilterAny) {
        QList<LabelTreeItem*> reverse { mLabelTree->childs() };
        while (!items.isEmpty()) {
            auto item = items.takeFirst();
            items.append(item->childs());
            reverse.append(item->childs());
        }
        for (auto iter=reverse.rbegin(); iter!=reverse.rend(); ++iter) {
            if (globalLabelStates[(*iter)->text()] == Qt::Checked)
                (*iter)->setVisible(true);
            else if (!(*iter)->hasChildren())
                (*iter)->setVisible(false);
            if (!(*iter)->hasChildren() && symLabelStates.contains((*iter)->sectionIndex()))
                (*iter)->setVisible(symLabelStates[(*iter)->sectionIndex()] == Qt::Checked);
        }
    } else {
        while (!items.isEmpty()) {
            auto item = items.takeFirst();
            if (globalLabelStates[item->text()] == Qt::Checked) {
                item->setVisible(true);
                items.append(item->childs());
            } else {
                item->setVisible(false);
            }
            if (!item->hasChildren() && symLabelStates.contains(item->sectionIndex()))
                item->setVisible(symLabelStates[item->sectionIndex()] == Qt::Checked);
        }
    }
}

void Aggregator::aggregateLabels(const AggregationItem &item, const QString &typeText)
{
    if (!mLabelTree->isVisible()) return;
    mLabelTree->setSectionIndex(mSymbol->firstSection());
    QList<LabelTreeItem*> currentLevel { mLabelTree->childs() };
    for (int d=1; d<=mSymbol->dimension(); ++d) {
        if (currentLevel.isEmpty())
            continue;
        if (item.isChecked(d)) {
            if (currentLevel.first()->parent()->isRoot()) {
                LabelTreeItem* newItem = LabelTreeItem::visibleBranch(currentLevel, typeText, d);
                if (!newItem) return;
                while (!currentLevel.isEmpty()) {
                    auto oldItem = currentLevel.takeFirst();
                    newItem->unite(oldItem);
                    oldItem->parent()->remove(oldItem);
                    delete oldItem;
                }
                currentLevel = newItem->childs();
            } else {
                QList<LabelTreeItem*> nextLevel;
                while (!currentLevel.isEmpty()) {
                    LabelTreeItem* newItem = LabelTreeItem::visibleBranch(currentLevel, typeText, d);
                    if (!newItem) return;
                    Q_FOREACH(auto sibling, newItem->siblings()) {
                        if (newItem == sibling)
                            continue;
                        newItem->unite(sibling);
                        currentLevel.removeAll(sibling);
                        sibling->parent()->remove(sibling);
                        delete sibling;
                    }
                    nextLevel.append(newItem->childs());
                }
                currentLevel = nextLevel;
            }
        } else {
            QList<LabelTreeItem*> nextLevel;
            while (!currentLevel.isEmpty()) {
                auto item = currentLevel.takeFirst();
                if (item->isVisible()) {
                    nextLevel.append(item->childs());
                } else {
                    item->parent()->remove(item);
                    delete item;
                }
            }
            currentLevel = nextLevel;
        }
    }
}

}
}
}
