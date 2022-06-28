#include "aggregation.h"
#include "symbol.h"
#include "labeltreeitem.h"

#include <algorithm>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

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

int AggregationItem::newSymbolIndex() const {
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

QSharedPointer<LabelTreeItem> AggregationItem::aggregatedLabelTree() const
{
    return mAggregatedLabelTree;
}

void AggregationItem::setAggregatedLabelTree(QSharedPointer<LabelTreeItem> labelTree)
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

bool Aggregation::useAbsoluteValuesGlobal() const
{
    return mUseAbsoluteValues;
}

void Aggregation::setUseAbsoluteValuesGlobal(bool absoluteValues)
{
    mUseAbsoluteValuesGlobal = absoluteValues;
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
    } else if (text.startsWith(MinMaxText, Qt::CaseInsensitive)) {
        mType = MinMax;
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
    case MinMax:
        return MinMaxText;
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

const IdentifierFilter& Aggregation::identifierFilter() const
{
    return mIdentifierFilter;
}

void Aggregation::setIdentifierFilter(const IdentifierFilter& filter)
{
    mIdentifierFilter = filter;
    for (auto oIter=filter.keyValueBegin(); oIter!=filter.keyValueEnd(); ++oIter) {
        for (auto sIter=oIter->second.keyValueBegin(); sIter!=oIter->second.keyValueEnd(); ++sIter) {
            if (mIdentifierFilter[oIter->first].contains(sIter->first)) {
                mIdentifierFilter[oIter->first][sIter->first].unite(sIter->second);
            } else {
                mIdentifierFilter[oIter->first][sIter->first] = sIter->second;
            }
        }
    }
}

void Aggregation::setLabelState(Qt::Orientation orientation,
                                int symIndex, int stateIndex,
                                Qt::CheckState state)
{
    mIdentifierFilter[orientation][symIndex].CheckStates[stateIndex] = state;
}

const ValueFilter& Aggregation::valueFilter() const
{
    return mValueFilter;
}

void Aggregation::setValueFilter(const ValueFilter &filter)
{
    mValueFilter = filter;
}

PredefinedViewEnum Aggregation::viewType() const
{
    return mViewType;
}

void Aggregation::setViewType(PredefinedViewEnum viewType)
{
    mViewType = viewType;
}

int Aggregation::view() const
{
    return mView;
}

void Aggregation::setView(int view)
{
    mView = view;
}

bool Aggregation::isActive() const
{
    return !mAggregationMap[Qt::Horizontal].isEmpty() ||
            !mAggregationMap[Qt::Vertical].isEmpty();
}

Aggregator::Aggregator(const Symbol &symbol)
    : mSymbol(symbol)
{
    mLabelTree = QSharedPointer<LabelTreeItem>(mSymbol.labelTree()->clone());
}

void Aggregator::applyFilterStates(const IdentifierState &identifierStates,
                                   const LabelCheckStates &globalLabelStates,
                                   bool labelFilterAny)
{
    mLabelTree->setVisible(identifierStates.Checked == Qt::Checked);
    applyLabelFilter(identifierStates, globalLabelStates, labelFilterAny);
}

void Aggregator::aggregate(AggregationItem &item, Aggregation::Type type,
                           const QString &typeText, int &lastSymEndIndex)
{
    if (type == Aggregation::MinMax) {
        if (mSymbol.isScalar())
            item.setMappedSections({mSymbol.firstSection()});
        else
            item.setMappedSections(mLabelTree->visibleSectionsSorted());
        aggregateLabels(item, type, typeText, lastSymEndIndex);
        item.setVisibleSections(mLabelTree->visibleSectionsSorted());
        auto labels = mLabelTree->sectionLabels(mLabelTree->sectionIndex(),
                                                mSymbol.isScalar() ? 1 : mSymbol.dimension());
        item.setLabels(labels);
        item.setAggregatedLabelTree(mLabelTree);
        item.setVisibleSectionCount(mLabelTree->sectionExtent());
        item.setUnitedSections(mLabelTree->unitedSections());
    } else {
        item.setVisibleSections(mLabelTree->visibleSectionsSorted());
        aggregateLabels(item, typeText);
        item.setAggregatedLabelTree(mLabelTree);
        item.setVisibleSectionCount(mLabelTree->sectionExtent());
        auto labels = mLabelTree->sectionLabels(mSymbol.firstSection(), mSymbol.dimension());
        item.setLabels(labels);
        item.setUnitedSections(mLabelTree->unitedSections());
    }
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

void Aggregator::aggregateLabels(const AggregationItem &item,
                                 Aggregation::Type type,
                                 const QString &typeText,
                                 int &lastSymEndIndex)
{
    if (!mLabelTree->isVisible()) return;
    if (type == Aggregation::MinMax) {
        if (mSymbol.isEquation()) {
            QList<LabelTreeItem*> currentLevel { mLabelTree->childs() };
            while (!currentLevel.isEmpty()) {
                auto oldItem = currentLevel.takeFirst();
                oldItem->parent()->remove(oldItem);
                delete oldItem;
            }
            auto maxItem = new LabelTreeItem("Max", mLabelTree.get());
            int index = lastSymEndIndex < 0 ? mSymbol.firstSection() : lastSymEndIndex + 1;
            mLabelTree->setSectionIndex(index);
            maxItem->setSectionIndex(index);
            maxItem->setSections({maxItem->sectionIndex()});
            mLabelTree->append(maxItem);
            auto minItem = new LabelTreeItem("Min", mLabelTree.get());
            minItem->setSectionIndex(index + 1);
            minItem->setSections({minItem->sectionIndex()});
            mLabelTree->append(minItem);
            lastSymEndIndex = minItem->sectionIndex();
            return;
        }
    }
    aggregateLabels(item, typeText);
}

void Aggregator::aggregateLabels(const AggregationItem &item, const QString &typeText)
{
    mLabelTree->setSectionIndex(mSymbol.firstSection());
    QList<LabelTreeItem*> currentLevel { mLabelTree->childs() };
    for (int d=1; d<=mSymbol.dimension(); ++d) {
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
