#include "viewconfigurationprovider.h"
#include "abstractmodelinstance.h"
#include "labeltreeitem.h"

#include <QAbstractItemModel>
#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

class DefaultViewConfiguration final : public AbstractViewConfiguration
{
public:
    DefaultViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {

    }

    AbstractViewConfiguration* clone() override
    {
        return new DefaultViewConfiguration(*this);
    }

    void initialize(QAbstractItemModel *model) override
    {
        Q_UNUSED(model);
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        Q_UNUSED(orientation);
        return IdentifierStates();
    }

    //void createDefaultAggregation()
    //{
    //    //auto initAggregation = [this](Symbol::Type type) {
    //    //    AggregationSymbols items;
    //    //    Q_FOREACH(const auto& sym, mModelInstance->symbols(type)) {
    //    //        AggregationItem aggrItem;
    //    //        aggrItem.setText(sym->name());
    //    //        aggrItem.setSymbolIndex(sym->firstSection());
    //    //        aggrItem.setDomainLabels(sym->domainLabels());
    //    //        for (int d=1; d<=sym->dimension(); ++d) {
    //    //            aggrItem.setCheckState(d, Qt::Unchecked);
    //    //        }
    //    //        items[aggrItem.symbolIndex()] = aggrItem;
    //    //    }
    //    //    return items;
    //    //};
    //    //Aggregation aggregation;
    //    //aggregation.setViewType(mViewType);
    //    //aggregation.setAggregationSymbols(Qt::Horizontal, initAggregation(Symbol::Variable));
    //    //aggregation.setAggregationSymbols(Qt::Vertical, initAggregation(Symbol::Equation));
    //    //return aggregation;
    //}

};

class EqnViewConfiguration final : public AbstractViewConfiguration
{
public:
    EqnViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {

    }

    AbstractViewConfiguration* clone() override
    {
        return new EqnViewConfiguration(*this);
    }

    void initialize(QAbstractItemModel *model) override
    {
        Q_UNUSED(model);
        createLabelFilter();
        mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(mViewType);
        mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(mViewType);
        mCurrentValueFilter = mDefaultValueFilter;
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        //createDefaultAggregation(); TODO !!! activate aggregation
        //createCurrentAggregation();
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        IdentifierStates states;
        if (orientation == Qt::Vertical) {
            int symIndex = 0;
            for (const auto& sym : mModelInstance->equations()) {
                IdentifierState identifierState;
                identifierState.Enabled = true;
                identifierState.SectionIndex = symIndex;
                identifierState.SymbolIndex = sym->firstSection();
                identifierState.Text = sym->name();
                identifierState.Checked = Qt::Checked;
                states[sym->firstSection()] = identifierState;
                symIndex++;
            }
        } else {
            int section = 0;
            for (const auto& attr : constant->PredefinedHeader) {
                IdentifierState identifierState;
                identifierState.Enabled = true;
                identifierState.SymbolIndex = section;
                identifierState.Text = attr;
                identifierState.Checked = Qt::Checked;
                states[section++] = identifierState;
            }
        }
        return states;
    }
};

class VarViewConfiguration final : public AbstractViewConfiguration
{
public:
    VarViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {

    }

    AbstractViewConfiguration* clone() override
    {
        return new VarViewConfiguration(*this);
    }

    void initialize(QAbstractItemModel *model) override
    {
        Q_UNUSED(model);
        createLabelFilter();
        mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(mViewType);
        mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(mViewType);
        mCurrentValueFilter = mDefaultValueFilter;
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        //createDefaultAggregation(); TODO !!! activate aggregation
        //createCurrentAggregation();
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        IdentifierStates states;
        if (orientation == Qt::Horizontal) {
            int symIndex = 0;
            for (const auto& sym : mModelInstance->variables()) {
                IdentifierState identifierState;
                identifierState.Enabled = true;
                identifierState.SectionIndex = symIndex;
                identifierState.SymbolIndex = sym->firstSection();
                identifierState.Text = sym->name();
                identifierState.Checked = Qt::Checked;
                states[sym->firstSection()] = identifierState;
                symIndex++;
            }
        } else {
            int section = 0;
            for (const auto& attr : constant->PredefinedHeader) {
                IdentifierState identifierState;
                identifierState.Enabled = true;
                identifierState.SymbolIndex = section;
                identifierState.Text = attr;
                identifierState.Checked = Qt::Checked;
                states[section++] = identifierState;
            }
        }
        return states;
    }
};

class JaccobianViewConfiguration final : public AbstractViewConfiguration
{
public:
    JaccobianViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {

    }

    AbstractViewConfiguration* clone() override
    {
        return new JaccobianViewConfiguration(*this);
    }

    void initialize(QAbstractItemModel *model) override
    {
        Q_UNUSED(model);
        createLabelFilter();
        mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(mViewType);
        mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(mViewType);
        mCurrentValueFilter = mDefaultValueFilter;
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        //createDefaultAggregation(); TODO !!! activate aggregation
        //createCurrentAggregation();
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        const auto& symbols = orientation == Qt::Horizontal ? mModelInstance->variables()
                                                            : mModelInstance->equations();
        int symIndex = 0;
        IdentifierStates states;
        for (const auto& sym : symbols) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SectionIndex = symIndex;
            identifierState.SymbolIndex = sym->firstSection();
            identifierState.Text = sym->name();
            identifierState.Checked = Qt::Checked;
            states[sym->firstSection()] = identifierState;
            symIndex++;
        }
        return states;
    }
};

class MinMaxViewConfiguration final : public AbstractViewConfiguration
{
public:
    MinMaxViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {

    }

    AbstractViewConfiguration* clone() override
    {
        return new MinMaxViewConfiguration(*this);
    }

    void initialize(QAbstractItemModel *model) override
    {
        Q_UNUSED(model);
        createLabelFilter();
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        createDefaultAggregation();
        createCurrentAggregation();
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        const auto& symbols = orientation == Qt::Horizontal ? mModelInstance->variables()
                                                            : mModelInstance->equations();
        int symIndex = 0;
        IdentifierStates states;
        for (const auto& sym : symbols) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SectionIndex = symIndex;
            identifierState.SymbolIndex = sym->firstSection();
            identifierState.Text = sym->name();
            identifierState.Checked = Qt::Checked;
            states[sym->firstSection()] = identifierState;
            symIndex++;
        }
        return states;
    }

private:
    void createDefaultAggregation()
    {
        auto initAggregation = [this](Symbol::Type type) {
            AggregationSymbols items;
            Q_FOREACH(const auto& sym, mModelInstance->symbols(type)) {
                AggregationItem aggrItem;
                aggrItem.setText(sym->name());
                aggrItem.setSymbolIndex(sym->firstSection());
                aggrItem.setDomainLabels(sym->domainLabels());
                if (sym->isScalar() && type == Symbol::Equation) {
                    aggrItem.setCheckState(0, Qt::Checked);
                } else if (sym->isScalar() && type == Symbol::Variable) {
                    aggrItem.setCheckState(0, Qt::Unchecked);
                    aggrItem.setScalar(true);
                } else if (type == Symbol::Equation) {
                    for (int d=1; d<=sym->dimension(); ++d) {
                        aggrItem.setCheckState(d, Qt::Checked);
                    }
                } else {
                    for (int d=1; d<=sym->dimension(); ++d) {
                        aggrItem.setCheckState(d, Qt::Checked);
                    }
                }
                items[aggrItem.symbolIndex()] = aggrItem;
            }
            return items;
        };
        mDefaultAggregation.setType(Aggregation::MinMax);
        mDefaultAggregation.setViewType(mViewType);
        mDefaultAggregation.setAggregationSymbols(Qt::Horizontal, initAggregation(Symbol::Variable));
        mDefaultAggregation.setAggregationSymbols(Qt::Vertical, initAggregation(Symbol::Equation));
    }

    void createCurrentAggregation()
    {
        AggregationMap map;
        for (auto mapIter=mDefaultAggregation.aggregationMap().keyValueBegin(); mapIter!=mDefaultAggregation.aggregationMap().keyValueEnd(); ++mapIter) {
            if (mapIter->first == Qt::Horizontal) {
                int lastSymEndIndex = -1;
                Q_FOREACH(auto item, mapIter->second) {
                    auto symbol = mModelInstance->variable(item.symbolIndex());
                    item.setMappedSections({symbol->firstSection()});
                    item.setVisibleSectionCount(1);
                    lastSymEndIndex = lastSymEndIndex < 0 ? symbol->firstSection() : lastSymEndIndex + 1;
                    mCurrentAggregation.indexToSymbol(Qt::Horizontal)[lastSymEndIndex] = symbol;
                    map[mapIter->first][item.symbolIndex()] = item;
                }
            } else {
                int lastSymEndIndex = -1;
                for (auto item : mapIter->second) {
                    auto symbol = mModelInstance->equation(item.symbolIndex());
                    aggregateEquation(item, symbol, lastSymEndIndex);

                    map[mapIter->first][item.newSymbolIndex()] = item;
                    mCurrentAggregation.startSectionMapping()[item.symbolIndex()] = item.newSymbolIndex();
                    mCurrentAggregation.indexToSymbol(Qt::Vertical)[item.newSymbolIndex()] = symbol;
                    mCurrentAggregation.indexToSymbol(Qt::Vertical)[item.newSymbolIndex()+1] = symbol;
                }
            }
        }
        mCurrentAggregation.setUseAbsoluteValues(mDefaultAggregation.useAbsoluteValues());
        mCurrentAggregation.setType(mDefaultAggregation.type());
        mCurrentAggregation.setAggregationMap(map);
        mCurrentAggregation.setViewType(mViewType);
    }

    void aggregateEquation(AggregationItem &item, Symbol *symbol, int &lastSymEndIndex)
    {
        if (symbol->isScalar())
            item.setMappedSections({symbol->firstSection()});
        else
            item.setMappedSections(symbol->labelTree()->visibleSectionsSorted());
        auto labelTree = aggregateLabels(symbol, lastSymEndIndex);
        item.setVisibleSections(labelTree->visibleSectionsSorted());
        auto labels = labelTree->minMaxSectionLabels(item.text(), labelTree->sectionIndex());
        item.setLabels(labels);
        item.setAggregatedLabelTree(labelTree);
        item.setVisibleSectionCount(labelTree->sectionExtent());
        item.setUnitedSections(labelTree->unitedSections());
    }

    QSharedPointer<LabelTreeItem> aggregateLabels(Symbol *symbol, int &lastSymEndIndex)
    {
        auto root = new LabelTreeItem;
        auto maxItem = new LabelTreeItem("Max", root);
        int index = lastSymEndIndex < 0 ? symbol->firstSection() : lastSymEndIndex + 1;
        root->setSectionIndex(index);
        maxItem->setSectionIndex(index);
        maxItem->setSections({maxItem->sectionIndex()});
        root->append(maxItem);
        auto minItem = new LabelTreeItem("Min", root);
        minItem->setSectionIndex(index + 1);
        minItem->setSections({minItem->sectionIndex()});
        root->append(minItem);
        lastSymEndIndex = minItem->sectionIndex();
        return QSharedPointer<LabelTreeItem>(root);
    }
};

class SymbolViewConfiguration final : public AbstractViewConfiguration
{
public:
    SymbolViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {
        createLabelFilter();
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        mDefaultAggregation.setType(Aggregation::Symols);
        mDefaultAggregation.setViewType(mViewType);
        mCurrentAggregation.setType(Aggregation::Symols);
        mCurrentAggregation.setViewType(mViewType);
    }

    AbstractViewConfiguration* clone() override
    {
        return new SymbolViewConfiguration(*this);
    }

    void initialize(QAbstractItemModel *model) override
    {// TODO !!! needed?
        Q_UNUSED(model);
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        const auto& symbols = orientation == Qt::Horizontal ? mModelInstance->variables()
                                                            : mModelInstance->equations();
        int symIndex = 0;
        IdentifierStates states;
        for (const auto& sym : symbols) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SectionIndex = symIndex;
            identifierState.SymbolIndex = sym->firstSection();
            identifierState.Text = sym->name();
            identifierState.Checked = Qt::Checked;
            states[sym->firstSection()] = identifierState;
            symIndex++;
        }
        return states;
    }
};

AbstractViewConfiguration::AbstractViewConfiguration(ViewDataType viewType,
                                                     QSharedPointer<AbstractModelInstance> modelInstance)
    : mModelInstance(modelInstance)
    , mViewType(viewType)
{

}

void AbstractViewConfiguration::setModelInstance(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
}

void AbstractViewConfiguration::updateIdentifierFilter(const QList<Symbol *> &eqnFilter,
                                                       const QList<Symbol *> &varFilter)
{
    for (auto iter=mCurrentIdentifierFilter[Qt::Vertical].begin();
         iter!=mCurrentIdentifierFilter[Qt::Vertical].end(); ++iter) {
        for (auto symbol : eqnFilter) {
            if (iter->Text == symbol->name()) {
                iter->Checked = Qt::Checked;
                break;
            } else {
                iter->Checked = Qt::Unchecked;
            }
        }
    }
    for (auto iter=mCurrentIdentifierFilter[Qt::Horizontal].begin();
         iter!=mCurrentIdentifierFilter[Qt::Horizontal].end(); ++iter) {
        for (auto symbol : varFilter) {
            if (iter->Text == symbol->name()) {
                iter->Checked = Qt::Checked;
                break;
            } else {
                iter->Checked = Qt::Unchecked;
            }
        }
    }
    mDefaultIdentifierFilter = mCurrentIdentifierFilter;
}

//const Aggregation& AbstractViewConfiguration::appliedAggregation() const
//{
//    return mCurrentAggregation;
//
    // TODO !!!
    //AggregationMap map;
    //Aggregation appliedAggregation;
    //for (auto mapIter=mCurrentAggregation.aggregationMap().keyValueBegin(); mapIter!=mCurrentAggregation.aggregationMap().keyValueEnd(); ++mapIter) {
    //    if (mapIter->first == Qt::Horizontal) {
    //        int lastSymEndIndex = -1;
    //        Q_FOREACH(auto item, mapIter->second) {
    //            for (auto iter=item.checkStates().keyValueBegin(); iter!=item.checkStates().keyValueEnd(); ++iter) {
    //                if (iter->second != Qt::Unchecked) {
    //                    auto symbol = mModelInstance->variable(item.symbolIndex());
    //                    Aggregator aggregator(symbol);
    //                    aggregator.applyFilterStates(mCurrentIdentifierFilter.value(mapIter->first)[symbol->firstSection()],
    //                                                 mCurrentLabelFilter.LabelCheckStates.value(mapIter->first),
    //                                                 mCurrentLabelFilter.Any);
    //                    aggregator.aggregate(item, mCurrentAggregation.type(), mCurrentAggregation.typeText(), lastSymEndIndex);
    //                    appliedAggregation.indexToSymbol(Qt::Horizontal)[item.symbolIndex()] = symbol;
    //                    map[mapIter->first][item.symbolIndex()] = item;
    //                    break;
    //                }
    //            }
    //        }
    //    } else {
    //        int lastSymEndIndex = -1;
    //        Q_FOREACH(auto item, mapIter->second) {
    //            for (auto iter=item.checkStates().keyValueBegin(); iter!=item.checkStates().keyValueEnd(); ++iter) {
    //                if (iter->second != Qt::Unchecked) {
    //                    auto symbol = mModelInstance->equation(item.symbolIndex());
    //                    Aggregator aggregator(symbol);
    //                    aggregator.applyFilterStates(mCurrentIdentifierFilter.value(mapIter->first)[symbol->firstSection()],
    //                                                 mCurrentLabelFilter.LabelCheckStates.value(mapIter->first),
    //                                                 mCurrentLabelFilter.Any);
    //                    aggregator.aggregate(item, mCurrentAggregation.type(), mCurrentAggregation.typeText(), lastSymEndIndex);
    //                    appliedAggregation.indexToSymbol(Qt::Vertical)[item.symbolIndex()] = symbol;
    //                    map[mapIter->first][item.symbolIndex()] = item;
    //                    break;
    //                }
    //            }
    //        }
    //    }
    //}

    //appliedAggregation.setUseAbsoluteValues(mCurrentAggregation.useAbsoluteValues());
    //appliedAggregation.setType(mCurrentAggregation.type());
    //appliedAggregation.setAggregationMap(map);
    //appliedAggregation.setIdentifierFilter(mCurrentIdentifierFilter);
    //appliedAggregation.setValueFilter(mCurrentValueFilter);
    //appliedAggregation.setViewType(mViewType);
    //appliedAggregation.setView(mView);
    //return appliedAggregation;
//}

void AbstractViewConfiguration::createLabelFilter()
{
    for (const auto& label : mModelInstance->labels()) {
        mDefaultLabelFilter.LabelCheckStates[Qt::Horizontal][label] = Qt::Checked;
        mDefaultLabelFilter.LabelCheckStates[Qt::Vertical][label] = Qt::Checked;
        mCurrentLabelFilter.LabelCheckStates[Qt::Horizontal][label] = Qt::Checked;
        mCurrentLabelFilter.LabelCheckStates[Qt::Vertical][label] = Qt::Checked;
    }
}

AbstractViewConfiguration *ViewConfigurationProvider::configuration(ViewDataType viewType,
                                                                    QSharedPointer<AbstractModelInstance> modelInstance)
{
    switch (viewType) {
    case ViewDataType::EqnAttributes:
        return new EqnViewConfiguration(viewType, modelInstance);
    case ViewDataType::VarAttributes:
        return new VarViewConfiguration(viewType, modelInstance);
    case ViewDataType::MinMax:
        return new MinMaxViewConfiguration(viewType, modelInstance);
    case ViewDataType::Jaccobian:
        return new JaccobianViewConfiguration(viewType, modelInstance);
    case ViewDataType::SymbolView:
        return new SymbolViewConfiguration(viewType, modelInstance);
    default:
        return new DefaultViewConfiguration(viewType, modelInstance);
    }
}

}
}
}
