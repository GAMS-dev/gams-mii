#include "aggregation.h"
#include "symbolinfo.h"

#include <algorithm>

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

const IndexCheckState& AggregationItem::checkState() const
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
    auto data = mLabels[sectionIndex];
    if (dimension < data.size())
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
    mIdentifierFilter[orientation][symIndex].LabelCheckStates[stateIndex] = state;
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
                                   const LabelStates &globalLabelStates)
{
    mLabelTree->setVisible(identifierState.Checked == Qt::Checked);
    QList<LabelTreeItem*> items { mLabelTree->childs() };
    IndexCheckState symLabelStates = symbolLabelStates.isValid() ? symbolLabelStates.LabelCheckStates
                                                                 : IndexCheckState();
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

void Aggregator::aggregate(AggregationItem &item, const QString &type)
{
    auto visibleSections = mLabelTree->visibleSections();
    std::sort(visibleSections.begin(), visibleSections.end());
    item.setVisibleSections(visibleSections);
    aggregateLabels(item, type);
    item.setAggregatedLabelTree(mLabelTree);
    item.setVisibleSectionCount(mLabelTree->sectionExtent());
    item.setLabels(sectionLabels(item));
    item.setUnitedSections(mLabelTree->unitedSections());
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

SectionLabels Aggregator::sectionLabels(const AggregationItem &item)
{// TODO header drawing rules... when to generate empty label cells
    SectionLabels sectionLabels;
    int sections = item.visibleSectionCount();
    QList<LabelTreeItem*> levelItems { item.aggregatedLabelTree()->childs() };
    auto visibleSections = item.visibleSections();
    for (int d=1, s=0; d<=mSymbol.dimension(); ++d, s=0) {
        QList<LabelTreeItem*> newItems;
        int duplicate = sections / levelItems.size();
        while (!levelItems.isEmpty()) {
            auto item = levelItems.takeFirst();
            if (!item->isVisible())
                continue;
            newItems.append(item->visibleChilds());
            for (int repeat=0; repeat<duplicate; ++repeat) {
                auto data = sectionLabels[visibleSections[s]];
                data << item->text();
                sectionLabels[visibleSections[s]] = data;
                ++s;
            }
        }
        levelItems = newItems;
    }
    return sectionLabels;
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
