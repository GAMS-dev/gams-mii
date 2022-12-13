#include "minmaxtableviewframe.h"
#include "ui_standardtableviewframe.h"
#include "hierarchicalheaderview.h"
#include "columnrowfiltermodel.h"
#include "valueformatproxymodel.h"
#include "minmaxmodelinstancetablemodel.h"
#include "abstractmodelinstance.h"

#include <QAction>
#include <QMenu>

namespace gams {
namespace studio{
namespace modelinspector {

MinMaxTableViewFrame::MinMaxTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractTableViewFrame(parent, f)
    , mModelInstanceModel(new MinMaxModelInstanceTableModel)
    , mSelectionMenu(new QMenu(this))
{
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    setupSelectionMenu();
    connect(ui->tableView, &QWidget::customContextMenuRequested,
            this, &MinMaxTableViewFrame::customMenuRequested);
}

AbstractTableViewFrame* MinMaxTableViewFrame::clone(int view)
{
    auto frame = new MinMaxTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

void MinMaxTableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    mCurrentIdentifierFilter = filter;
    if (mIdentifierFilterModel)
        mIdentifierFilterModel->setIdentifierFilter(filter,
                                                    mModelInstanceModel->appliedAggregation());
}

const Aggregation &MinMaxTableViewFrame::appliedAggregation() const
{
    return mModelInstanceModel->appliedAggregation();
}

void MinMaxTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    mHorizontalHeader->setViewType(PredefinedViewEnum::MinMax);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &MinMaxTableViewFrame::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(PredefinedViewEnum::MinMax);
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &MinMaxTableViewFrame::setIdentifierLabelFilter);

    auto modelInstanceModel = new MinMaxModelInstanceTableModel(ui->tableView);
    modelInstanceModel->setModelInstance(mModelInstance);
    modelInstanceModel->setView(view);
    mValueFormatModel = new ValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(modelInstanceModel);
    mIdentifierFilterModel = new MinMaxIdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mValueFormatModel);
    mColumnRowFilterModel = new ColumnRowFilterModel(ui->tableView);
    mColumnRowFilterModel->setSourceModel(mIdentifierFilterModel);

    ui->tableView->setHorizontalHeader(mHorizontalHeader);
    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mColumnRowFilterModel);
    delete oldSelectionModel;
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mModelInstanceModel = QSharedPointer<MinMaxModelInstanceTableModel>(modelInstanceModel);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

QList<Symbol> MinMaxTableViewFrame::selectedEquations() const
{
    return mSelectedEquations;
}

QList<Symbol> MinMaxTableViewFrame::selectedVariables() const
{
    return mSelectedVariables;
}

void MinMaxTableViewFrame::reset(PredefinedViewEnum view)
{
    Q_UNUSED(view);
    setIdentifierFilter(mDefaultIdentifierFilter);
    setValueFilter(mDefaultValueFilter);
    setLabelFilter(mDefaultLabelFilter);
    updateView();
}

void MinMaxTableViewFrame::updateView()
{
    if (mColumnRowFilterModel) mColumnRowFilterModel->invalidate();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    updateValueFilter();
    emit filtersChanged();
}

void MinMaxTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    AbstractTableViewFrame::setValueFilter(filter);
    if (mValueFormatModel) mValueFormatModel->setValueFilter(filter);
}

void MinMaxTableViewFrame::setAggregation(const Aggregation &aggregation, int view)
{
    mCurrentAggregation = aggregation;
    mCurrentAggregation.setView(view);
    if (mModelInstanceModel && mHorizontalHeader && mVerticalHeader) {
        auto applied = appliedAggregation(aggregation, view);
        mHorizontalHeader->setAppliedAggregation(applied);
        mVerticalHeader->setAppliedAggregation(applied);
        mModelInstanceModel->setAggregation(mCurrentAggregation, applied);
        mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(type());
        mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(type());
        mCurrentValueFilter.MinValue = mModelInstance->modelMinimum(type());
        mCurrentValueFilter.MaxValue = mModelInstance->modelMaximum(type());
    }
}

void MinMaxTableViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mModelInstanceModel && mModelInstanceModel->appliedAggregation().isActive() &&
            mModelInstanceModel->appliedAggregation().useAbsoluteValues() != absoluteValues) {
        auto aggregation = mModelInstanceModel->aggregation();
        aggregation.setUseAbsoluteValues(absoluteValues);
        aggregation.setUseAbsoluteValuesGlobal(absoluteValues);
        auto appliedAggregation = mModelInstanceModel->appliedAggregation();
        appliedAggregation.setUseAbsoluteValues(absoluteValues);
        appliedAggregation.setUseAbsoluteValuesGlobal(absoluteValues);
        mCurrentAggregation.setUseAbsoluteValues(absoluteValues);
        mCurrentAggregation.setUseAbsoluteValuesGlobal(absoluteValues);
        mModelInstanceModel->setAggregation(aggregation, appliedAggregation);
    } else {
        mCurrentAggregation.setUseAbsoluteValues(absoluteValues);
        mCurrentAggregation.setUseAbsoluteValuesGlobal(absoluteValues);
    }
    mCurrentValueFilter.UseAbsoluteValues = absoluteValues;
    mCurrentValueFilter.UseAbsoluteValuesGlobal = absoluteValues;
    mValueFormatModel->setValueFilter(mCurrentValueFilter);
    updateValueFilter();
}

void MinMaxTableViewFrame::setupFiltersAggregation(QAbstractItemModel *model, const LabelFilter &filter)
{
    mCurrentLabelFilter = filter;
    mDefaultLabelFilter = mCurrentLabelFilter;
    mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(PredefinedViewEnum::Full);
    mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(PredefinedViewEnum::Full);
    mCurrentValueFilter = mDefaultValueFilter;
    mDefaultIdentifierFilter[Qt::Horizontal] = defaultSymbolFilter(model, Qt::Horizontal);
    mDefaultIdentifierFilter[Qt::Vertical] = defaultSymbolFilter(model, Qt::Vertical);
    mCurrentIdentifierFilter = mDefaultIdentifierFilter;
    mDefaultAggregation = getDefaultAggregation();
    mCurrentAggregation = mDefaultAggregation;
}

void MinMaxTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                    Qt::Orientation orientation)
{
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter,
                                                    mModelInstanceModel->appliedAggregation());
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
    }
    updateView();
    emit filtersChanged();
}

void MinMaxTableViewFrame::customMenuRequested(const QPoint &pos)
{
    mSelectionMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

Aggregation MinMaxTableViewFrame::getDefaultAggregation() const
{
    auto initAggregation = [this](Symbol::Type type) {
        AggregationSymbols items;
        Q_FOREACH(const auto& sym, mModelInstance->symbols(type)) {
            AggregationItem aggrItem;
            aggrItem.setText(sym.name());
            aggrItem.setSymbolIndex(sym.firstSection());
            aggrItem.setDomainLabels(sym.domainLabels());
            if (sym.isScalar() && type == Symbol::Equation) {
                aggrItem.setCheckState(0, Qt::Checked);
            } else if (sym.isScalar() && type == Symbol::Variable) {
                aggrItem.setCheckState(0, Qt::Unchecked);
                aggrItem.setScalar(true);
            } else if (type == Symbol::Equation) {
                for (int d=1; d<=sym.dimension(); ++d) {
                    aggrItem.setCheckState(d, Qt::Checked);
                }
            } else {
                for (int d=1; d<=sym.dimension(); ++d) {
                    aggrItem.setCheckState(d, Qt::Checked);
                }
            }
            items[aggrItem.symbolIndex()] = aggrItem;
        }
        return items;
    };
    Aggregation aggregation;
    aggregation.setType(Aggregation::MinMax);
    aggregation.setViewType(type());
    aggregation.setAggregationSymbols(Qt::Horizontal, initAggregation(Symbol::Variable));
    aggregation.setAggregationSymbols(Qt::Vertical, initAggregation(Symbol::Equation));
    return aggregation;
}

Aggregation MinMaxTableViewFrame::appliedAggregation(const Aggregation &aggregation, int view) const
{
    AggregationMap map;
    Aggregation appliedAggregation;
    for (auto mapIter=aggregation.aggregationMap().keyValueBegin(); mapIter!=aggregation.aggregationMap().keyValueEnd(); ++mapIter) {
        if (mapIter->first == Qt::Horizontal) {
            int lastSymEndIndex = -1;
            Q_FOREACH(auto item, mapIter->second) {
                for (auto iter=item.checkStates().keyValueBegin(); iter!=item.checkStates().keyValueEnd(); ++iter) {
                    auto symbol = mModelInstance->variable(item.symbolIndex());
                    if (iter->second != Qt::Unchecked) {
                        Aggregator aggregator(symbol);
                        aggregator.applyFilterStates(identifierFilter().value(mapIter->first)[symbol.firstSection()],
                                                     labelFilter().LabelCheckStates.value(mapIter->first),
                                                     labelFilter().Any);
                        aggregator.aggregate(item, aggregation.type(), aggregation.typeText(), lastSymEndIndex);

                        if (lastSymEndIndex < 0) lastSymEndIndex = item.symbolIndex();
                        for (int i=lastSymEndIndex; i<lastSymEndIndex+item.visibleSectionCount(); ++i) {
                            appliedAggregation.indexToSymbol(Qt::Horizontal)[i] = symbol;
                        }
                        lastSymEndIndex += item.visibleSectionCount();
                        map[mapIter->first][item.symbolIndex()] = item;
                        break;
                    } else {
                        if (appliedAggregation.indexToSymbol(Qt::Horizontal).contains(symbol.firstSection()))
                            continue;
                        if (lastSymEndIndex < 0) lastSymEndIndex = item.symbolIndex();
                        int sectionCount = symbol.entries();
                        for (int i=lastSymEndIndex; i<lastSymEndIndex+sectionCount; ++i) {
                            appliedAggregation.indexToSymbol(Qt::Horizontal)[i] = symbol;
                        }
                        lastSymEndIndex += sectionCount;
                    }
                }
            }
        } else {
            int lastSymEndIndex = -1;
            Q_FOREACH(auto item, mapIter->second) {
                for (auto iter=item.checkStates().keyValueBegin(); iter!=item.checkStates().keyValueEnd(); ++iter) {
                    if (iter->second != Qt::Unchecked) {
                        auto symbol = mModelInstance->equation(item.symbolIndex());
                        Aggregator aggregator(symbol);
                        aggregator.applyFilterStates(identifierFilter().value(mapIter->first)[symbol.firstSection()],
                                                     labelFilter().LabelCheckStates.value(mapIter->first),
                                                     labelFilter().Any);
                        aggregator.aggregate(item, aggregation.type(), aggregation.typeText(), lastSymEndIndex);
                        map[mapIter->first][item.newSymbolIndex()] = item;
                        appliedAggregation.startSectionMapping()[item.symbolIndex()] = item.newSymbolIndex();
                        appliedAggregation.indexToSymbol(Qt::Vertical)[item.newSymbolIndex()] = symbol;
                        appliedAggregation.indexToSymbol(Qt::Vertical)[item.newSymbolIndex()+1] = symbol;
                        break;
                    }
                }
            }
        }
    }

    appliedAggregation.setUseAbsoluteValues(aggregation.useAbsoluteValues());
    appliedAggregation.setType(aggregation.type());
    appliedAggregation.setAggregationMap(map);
    appliedAggregation.setIdentifierFilter(identifierFilter());
    appliedAggregation.setValueFilter(valueFilter());
    appliedAggregation.setViewType(type());
    appliedAggregation.setView(view);
    return appliedAggregation;
}

void MinMaxTableViewFrame::setupSelectionMenu()
{
    auto action = new QAction("Show selected symbols", this);
    mSelectionMenu->addAction(action);
    connect(action, &QAction::triggered, this, [this]{handleRowColumnSelection(PredefinedViewEnum::SymbolView);});
    mSelectionMenu->addSeparator();
    action = new QAction("Show selected equations", this);
    mSelectionMenu->addAction(action);
    connect(action, &QAction::triggered, this, [this]{handleRowColumnSelection(PredefinedViewEnum::SymbolEqnView);});
    action = new QAction("Show selected variables", this);
    mSelectionMenu->addAction(action);
    connect(action, &QAction::triggered, this, [this]{handleRowColumnSelection(PredefinedViewEnum::SymbolVarView);});
}

void MinMaxTableViewFrame::handleRowColumnSelection(PredefinedViewEnum type)
{
    QMap<int, Symbol> rowSymbols, columnSymbols;
    auto indexes = ui->tableView->selectionModel()->selectedIndexes();
    if (PredefinedViewEnum::SymbolEqnView == type) {
        Q_FOREACH(auto index, indexes) {
            int section = ui->tableView->model()->headerData(index.row(), Qt::Vertical).toInt();
            if (mModelInstanceModel->appliedAggregation().indexToEquation().contains(section)) {
                auto symbol = mModelInstanceModel->appliedAggregation().indexToEquation()[section];
                rowSymbols[symbol.firstSection()] = symbol;
            } else {
                auto symbol = mModelInstance->equation(section);
                rowSymbols[symbol.firstSection()] = symbol;
            }
        }
    } else if (PredefinedViewEnum::SymbolVarView == type) {
        Q_FOREACH(auto index, indexes) {
            int section = ui->tableView->model()->headerData(index.column(), Qt::Horizontal).toInt();
            auto symbol = mModelInstance->variable(section);
            if (mModelInstanceModel->appliedAggregation().indexToVariable().contains(section)) {
                auto symbol = mModelInstanceModel->appliedAggregation().indexToVariable()[section];
                columnSymbols[symbol.firstSection()] = symbol;
            } else {
                auto symbol = mModelInstance->variable(section);
                columnSymbols[symbol.firstSection()] = symbol;
            }
        }
    } else if (PredefinedViewEnum::SymbolView == type) {
        int section;
        Q_FOREACH(auto index, indexes) {
            section = ui->tableView->model()->headerData(index.row(), Qt::Vertical).toInt();
            if (mModelInstanceModel->appliedAggregation().indexToEquation().contains(section)) {
                auto symbol = mModelInstanceModel->appliedAggregation().indexToEquation()[section];
                rowSymbols[symbol.firstSection()] = symbol;
            } else {
                auto symbol = mModelInstance->equation(section);
                rowSymbols[symbol.firstSection()] = symbol;
            }
            section = ui->tableView->model()->headerData(index.column(), Qt::Horizontal).toInt();
            if (mModelInstanceModel->appliedAggregation().indexToVariable().contains(section)) {
                auto symbol = mModelInstanceModel->appliedAggregation().indexToVariable()[section];
                columnSymbols[symbol.firstSection()] = symbol;
            } else {
                auto symbol = mModelInstance->variable(section);
                columnSymbols[symbol.firstSection()] = symbol;
            }
        }
    }
    mSelectedEquations = rowSymbols.values();
    mSelectedVariables = columnSymbols.values();
    emit newModelView(type);
}

void MinMaxTableViewFrame::updateValueFilter()
{
    if (mCurrentValueFilter.isUserInput()) return;

    bool ok;
    double min = 0.0;
    double max = 0.0;
    for (int r=0; r<ui->tableView->model()->rowCount(); ++r) {
        for (int c=0; c<ui->tableView->model()->columnCount(); ++c) {
            auto variant = ui->tableView->model()->index(r,c).data();
            if (ValueFilter::isSpecialValue(variant.toString()))
                continue;
            auto value = variant.toDouble(&ok);
            if (ok) {
                min = ValueFilter::minValue(min, value);
                max = ValueFilter::maxValue(max, value);
            }
        }
    }
    mCurrentValueFilter.MinValue = min;
    mCurrentValueFilter.MaxValue = max;
}

void MinMaxTableViewFrame::setIdentifierFilterCheckState(int symbolIndex, Qt::CheckState state, Qt::Orientation orientation)
{
    auto symbols = mCurrentIdentifierFilter[orientation];
    for (auto iter=symbols.begin(); iter!=symbols.end(); ++iter) {
        if (iter->SymbolIndex == symbolIndex) {
            iter->Checked = state;
            break;
        }
    }
    mCurrentIdentifierFilter[orientation] = symbols;
}

void MinMaxTableViewFrame::cloneFilterAndAggregation(MinMaxTableViewFrame *clone, int newView)
{
    clone->mDefaultValueFilter = mDefaultValueFilter;
    clone->setValueFilter(mCurrentValueFilter);
    clone->mDefaultLabelFilter = mDefaultLabelFilter;
    clone->setLabelFilter(mCurrentLabelFilter);
    clone->mDefaultIdentifierFilter = mDefaultIdentifierFilter;
    clone->setIdentifierFilter(mCurrentIdentifierFilter);
    clone->mDefaultAggregation = mDefaultAggregation;
    clone->setAggregation(mCurrentAggregation, newView);
}

}
}
}
