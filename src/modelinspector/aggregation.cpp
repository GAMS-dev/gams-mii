#include "aggregation.h"
#include "symbolinfo.h"
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

const IdentifierFilter& Aggregation::identifierFilter() const
{
    return mIdentifierFilter;
}

void Aggregation::setIdentifierFilter(const IdentifierFilter& filter)
{
    mIdentifierFilter = filter;
}

void Aggregation::setIdentifierLabelFilter(const IdentifierFilter& filter)
{
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

Aggregator::Aggregator(const SymbolInfo &symbol)
    : mSymbol(symbol)
{
    mLabelTree = QSharedPointer<LabelTreeItem>(clone(mSymbol.labelTree()));
}

void Aggregator::applyFilterStates(const IdentifierState &identifierState,
                                   const IdentifierState &symbolLabelStates,
                                   const LabelCheckStates &globalLabelStates,
                                   bool labelFilterAny)
{
    mLabelTree->setVisible(identifierState.Checked == Qt::Checked);
    applyLabelFilter(symbolLabelStates, globalLabelStates, labelFilterAny);
}

void Aggregator::aggregate(AggregationItem &item, const QString &type)
{
    item.setVisibleSections(mLabelTree->visibleSectionsSorted());
    aggregateLabels(item, type);
    item.setAggregatedLabelTree(mLabelTree);
    item.setVisibleSectionCount(mLabelTree->sectionExtent());
    item.setLabels(mLabelTree->sectionLabels(mSymbol.firstSection(), mSymbol.dimension()));
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

LabelTreeItem* Aggregator::clone(QSharedPointer<LabelTreeItem> labelTree)
{
    if (!labelTree) return nullptr;
    LabelTreeItem *root = new LabelTreeItem;
    Q_FOREACH(auto child, labelTree->childs()) {
        auto newChild = child->clone(root);
        root->append(newChild);
    }
    return root;
}

void Aggregator::aggregateLabels(const AggregationItem &item, const QString &type)
{
    if (!mLabelTree->isVisible()) return;
    QList<LabelTreeItem*> currentLevel { mLabelTree->childs() };
    for (int d=1; d<=mSymbol.dimension(); ++d) {
        if (currentLevel.isEmpty())
            continue;
        if (item.isChecked(d)) {
            if (currentLevel.first()->parent()->isRoot()) {
                LabelTreeItem* newItem = visibleBranch(currentLevel, type, d);
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
                    LabelTreeItem* newItem = visibleBranch(currentLevel, type, d);
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

LabelTreeItem* Aggregator::visibleBranch(QList<LabelTreeItem*> &currentLevel,
                                         const QString &type, int dimension)
{
    LabelTreeItem* newItem = nullptr;
    do {
        newItem = currentLevel.takeFirst();
        if (newItem->isVisible()) {
            newItem->setText(type + " - " + QString::number(dimension));
            break;
        } else {
            newItem->parent()->remove(newItem);
            delete newItem;
            newItem = nullptr;
        }
    } while (!currentLevel.isEmpty());
    return newItem;
}

}
}
}
