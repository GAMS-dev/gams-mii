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
        mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(viewType());
        mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(viewType());
        mCurrentValueFilter = mDefaultValueFilter;
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
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
        mDefaultAggregation.setType(Aggregation::None);
        mDefaultAggregation.setViewType(viewType());
        mDefaultAggregation.setAggregationSymbols(Qt::Horizontal, initAggregation(Symbol::Variable));
        mDefaultAggregation.setAggregationSymbols(Qt::Vertical, initAggregation(Symbol::Equation));
    }

    void createCurrentAggregation()
    {
        AggregationMap map;
        for (auto mapIter=mDefaultAggregation.aggregationMap().keyValueBegin();
             mapIter!=mDefaultAggregation.aggregationMap().keyValueEnd(); ++mapIter) {
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
        mCurrentAggregation.setViewType(viewType());
    }

    void aggregateEquation(AggregationItem &item, Symbol *symbol, int &lastSymEndIndex)
    {
        if (symbol->isScalar())
            item.setMappedSections({symbol->firstSection()});
        else
            item.setMappedSections(symbol->labelTree()->visibleSectionsSorted());
        auto labelTree = aggregateLabels(symbol, lastSymEndIndex);
        item.setVisibleSections(labelTree->visibleSectionsSorted());
        auto labels = sectionLabels(item.text(), labelTree->sectionIndex());
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

    SectionLabels sectionLabels(const QString &symName, int startSection)
    {
        SectionLabels sectionLabels;
        for (int d=1, s=startSection; s<=startSection+2; ++d, ++s) {
            auto data = sectionLabels[s];
            if (d == 1) {
                data << symName << "Max";
                sectionLabels[s] = data;
            } else if (d == 2) {
                data << QString() << "Min";
                sectionLabels[s] = data;
            }
        }
        return sectionLabels;
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
        mDefaultAggregation.setViewType(this->viewType());
        mCurrentAggregation.setViewType(this->viewType());
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

class BlockpicOverviewViewConfiguration final : public AbstractViewConfiguration
{
public:
    BlockpicOverviewViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {

    }

    AbstractViewConfiguration* clone() override
    {
        return new BlockpicOverviewViewConfiguration(*this);
    }

    void initialize(QAbstractItemModel *model) override
    {
        Q_UNUSED(model);
        createLabelFilter();
        mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(viewType());
        mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(viewType());
        mCurrentValueFilter = mDefaultValueFilter;
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
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

class BlockpicNegPosViewConfiguration final : public AbstractViewConfiguration
{
public:
    BlockpicNegPosViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {

    }

    AbstractViewConfiguration* clone() override
    {
        return new BlockpicNegPosViewConfiguration(*this);
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
        mDefaultAggregation.setType(Aggregation::None);
        mDefaultAggregation.setViewType(viewType());
        mDefaultAggregation.setAggregationSymbols(Qt::Horizontal, initAggregation(Symbol::Variable));
        mDefaultAggregation.setAggregationSymbols(Qt::Vertical, initAggregation(Symbol::Equation));
    }

    void createCurrentAggregation()
    {
        AggregationMap map;
        for (auto mapIter=mDefaultAggregation.aggregationMap().keyValueBegin();
             mapIter!=mDefaultAggregation.aggregationMap().keyValueEnd(); ++mapIter) {
            if (mapIter->first == Qt::Horizontal) {
                int lastSymEndIndex = -1;
                for (auto item : mapIter->second) {
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
        mCurrentAggregation.setViewType(viewType());
    }

    void aggregateEquation(AggregationItem &item, Symbol *symbol, int &lastSymEndIndex)
    {
        if (symbol->isScalar())
            item.setMappedSections({symbol->firstSection()});
        else
            item.setMappedSections(symbol->labelTree()->visibleSectionsSorted());
        auto labelTree = aggregateLabels(symbol, lastSymEndIndex);
        item.setVisibleSections(labelTree->visibleSectionsSorted());
        auto labels = sectionLabels(item.text(), labelTree->sectionIndex());
        item.setLabels(labels);
        item.setAggregatedLabelTree(labelTree);
        item.setVisibleSectionCount(labelTree->sectionExtent());
        item.setUnitedSections(labelTree->unitedSections());
    }

    QSharedPointer<LabelTreeItem> aggregateLabels(Symbol *symbol, int &lastSymEndIndex)
    {
        auto root = new LabelTreeItem;
        auto maxItem = new LabelTreeItem("Pos", root);
        int index = lastSymEndIndex < 0 ? symbol->firstSection() : lastSymEndIndex + 1;
        root->setSectionIndex(index);
        maxItem->setSectionIndex(index);
        maxItem->setSections({maxItem->sectionIndex()});
        root->append(maxItem);
        auto minItem = new LabelTreeItem("Neg", root);
        minItem->setSectionIndex(index + 1);
        minItem->setSections({minItem->sectionIndex()});
        root->append(minItem);
        lastSymEndIndex = minItem->sectionIndex();
        return QSharedPointer<LabelTreeItem>(root);
    }

    SectionLabels sectionLabels(const QString &symName, int startSection)
    {
        SectionLabels sectionLabels;
        for (int d=1, s=startSection; s<=startSection+2; ++d, ++s) {
            auto data = sectionLabels[s];
            if (d == 1) {
                data << symName << "Pos";
                sectionLabels[s] = data;
            } else if (d == 2) {
                data << QString() << "Neg";
                sectionLabels[s] = data;
            }
        }
        return sectionLabels;
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
    case ViewDataType::BP_Scaling:
        return new MinMaxViewConfiguration(viewType, modelInstance);
    case ViewDataType::Jaccobian:
        return new JaccobianViewConfiguration(viewType, modelInstance);
    case ViewDataType::Symbols:
        return new SymbolViewConfiguration(viewType, modelInstance);
    case ViewDataType::BP_Overview:
        return new BlockpicOverviewViewConfiguration(viewType, modelInstance);
    case ViewDataType::BP_Count:
    case ViewDataType::BP_Average:
        return new BlockpicNegPosViewConfiguration(viewType, modelInstance);
    default:
        return new DefaultViewConfiguration(viewType, modelInstance);
    }
}

}
}
}
