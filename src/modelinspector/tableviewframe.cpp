#include "tableviewframe.h"
#include "ui_tableviewframe.h"
#include "hierarchicalheaderview.h"
#include "attributetablemodel.h"
#include "columnrowfiltermodel.h"
#include "identifierfiltermodel.h"
#include "labelfiltermodel.h"
#include "aggregationproxymodel.h"
#include "valueformatproxymodel.h"
#include "jaccobiantablemodel.h"
#include "modelinstancetablemodel.h"
#include "abstractmodelinstance.h"

#include <QAction>
#include <QMenu>

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

TableViewFrame::TableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : QFrame(parent, f)
    , ui(new Ui::TableViewFrame)
{
    ui->setupUi(this);
}

const IdentifierFilter &TableViewFrame::identifierFilter() const
{
    return mCurrentIdentifierFilter;
}

const IdentifierFilter &TableViewFrame::defaultIdentifierFilter() const
{
    return mDefaultIdentifierFilter;
}

void TableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    mCurrentIdentifierFilter = filter;
}

const ValueFilter &TableViewFrame::valueFilter() const
{
    return mCurrentValueFilter;
}

const ValueFilter &TableViewFrame::defaultValueFilter() const
{
    return mDefaultValueFilter;
}

void TableViewFrame::setValueFilter(const ValueFilter &filter)
{
    mCurrentValueFilter = filter;
}

const LabelFilter &TableViewFrame::labelFilter() const
{
    return mCurrentLabelFilter;
}

const LabelFilter &TableViewFrame::defaultLabelFilter() const
{
    return mDefaultLabelFilter;
}

void TableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    mCurrentLabelFilter = filter;
}

const Aggregation &TableViewFrame::aggregation() const
{
    return mCurrentAggregation;
}

const Aggregation &TableViewFrame::defaultAggregation() const
{
    return mDefaultAggregation;
}

void TableViewFrame::setAggregation(const Aggregation &aggregation, int view)
{
    mCurrentAggregation = aggregation;
    mCurrentAggregation.setView(view);
    if (mAggregationModel && mHorizontalHeader && mVerticalHeader) {
        auto applied = appliedAggregation(aggregation, view);
        mHorizontalHeader->setAppliedAggregation(applied);
        mVerticalHeader->setAppliedAggregation(applied);
        mAggregationModel->setAggregation(mCurrentAggregation, applied);
    }
}

PredefinedViewEnum TableViewFrame::type() const
{
    return PredefinedViewEnum::Unknown;
}

DataSource TableViewFrame::horizontalDataSource() const
{
    return DataSource::VariableData;
}

DataSource TableViewFrame::verticalDataSource() const
{
    return DataSource::EquationData;
}

void TableViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mAggregationModel && mAggregationModel->appliedAggregation().isActive() &&
            mAggregationModel->appliedAggregation().useAbsoluteValues() != absoluteValues) {
        auto aggregation = mAggregationModel->aggregation();
        aggregation.setUseAbsoluteValues(absoluteValues);
        aggregation.setUseAbsoluteValuesGlobal(absoluteValues);
        auto appliedAggregation = mAggregationModel->appliedAggregation();
        appliedAggregation.setUseAbsoluteValues(absoluteValues);
        appliedAggregation.setUseAbsoluteValuesGlobal(absoluteValues);
        mCurrentAggregation.setUseAbsoluteValues(absoluteValues);
        mCurrentAggregation.setUseAbsoluteValuesGlobal(absoluteValues);
        mAggregationModel->setAggregation(aggregation, appliedAggregation);
    } else {
        mCurrentAggregation.setUseAbsoluteValues(absoluteValues);
        mCurrentAggregation.setUseAbsoluteValuesGlobal(absoluteValues);
    }
    mCurrentValueFilter.UseAbsoluteValues = absoluteValues;
    mCurrentValueFilter.UseAbsoluteValuesGlobal = absoluteValues;
    mValueFormatModel->setValueFilter(mCurrentValueFilter);
}

void TableViewFrame::setSearchSelection(const gams::studio::modelinspector::SearchResult &result)
{
    if (result.Index < 0) return;
    if (result.Orientation == Qt::Horizontal) {
        ui->tableView->selectColumn(result.Index);
    } else {
        ui->tableView->selectRow(result.Index);
    }
}

void TableViewFrame::setupFiltersAggregation(QAbstractItemModel *model,
                                             const LabelFilter &filter)
{
    mCurrentLabelFilter = filter;
    mDefaultLabelFilter = mCurrentLabelFilter;
    mDefaultValueFilter.MinValue = mModelInstance->modelMinimum();
    mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum();
    mCurrentValueFilter = mDefaultValueFilter;
    mDefaultIdentifierFilter[Qt::Horizontal] = defaultSymbolFilter(model, Qt::Horizontal);
    mDefaultIdentifierFilter[Qt::Vertical] = defaultSymbolFilter(model, Qt::Vertical);
    mCurrentIdentifierFilter = mDefaultIdentifierFilter;
    mDefaultAggregation = getDefaultAggregation();
    mCurrentAggregation = mDefaultAggregation;
}

void TableViewFrame::reset(PredefinedViewEnum view)
{
    setIdentifierFilter(mDefaultIdentifierFilter);
    setValueFilter(mDefaultValueFilter);
    setLabelFilter(mDefaultLabelFilter);
    setAggregation(mDefaultAggregation, (int)view);
    resetColumnRowFilter();
}

IdentifierStates TableViewFrame::defaultSymbolFilter(QAbstractItemModel *model,
                                                     Qt::Orientation orientation) const
{
    int sections = orientation == Qt::Horizontal ? model->columnCount() :
                                                   model->rowCount();
    bool ok;
    IdentifierStates states;
    QSet<QString> symNames;
    for (int s=0; s<sections; ++s) {
        int realSection = model->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        auto data = mModelInstance->headerData(realSection, -1, orientation);
        if (realSection < PredefinedHeaderLength) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SymbolIndex = realSection;
            identifierState.Text = data;
            identifierState.Checked = Qt::Checked;
            states[realSection] = identifierState;
        } else if (!symNames.contains(data)) {
            symNames.insert(data);
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SymbolIndex = realSection;
            identifierState.Text = data;
            identifierState.Checked = Qt::Checked;
            states[realSection] = identifierState;
        }
    }
    return states;
}

Aggregation TableViewFrame::appliedAggregation(const Aggregation &aggregation, int view) const
{
    AggregationMap map;
    Aggregation appliedAggregation;
    for (auto mapIter=aggregation.aggregationMap().keyValueBegin(); mapIter!=aggregation.aggregationMap().keyValueEnd(); ++mapIter) {
        if (mapIter->first == Qt::Horizontal) {
            int lastSymEndIndex = -1;
            Q_FOREACH(auto item, mapIter->second) {
                for (auto iter=item.checkStates().keyValueBegin(); iter!=item.checkStates().keyValueEnd(); ++iter) {
                    if (iter->second != Qt::Unchecked) {
                        auto symbol = mModelInstance->variable(item.symbolIndex());
                        Aggregator aggregator(symbol);
                        aggregator.applyFilterStates(identifierFilter().value(mapIter->first)[symbol.firstSection()],
                                                     labelFilter().LabelCheckStates.value(mapIter->first),
                                                     labelFilter().Any);
                        aggregator.aggregate(item, aggregation.type(), aggregation.typeText(), lastSymEndIndex);
                        map[mapIter->first][item.symbolIndex()] = item;
                        break;
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
                        map[mapIter->first][item.symbolIndex()] = item;
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

void TableViewFrame::cloneFilterAndAggregation(TableViewFrame *clone, int newView)
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

Aggregation TableViewFrame::getDefaultAggregation() const
{
    auto initAggregation = [this](Symbol::Type type) {
        AggregationSymbols items;
        Q_FOREACH(const auto& sym, mModelInstance->symbols(type)) {
            AggregationItem aggrItem;
            aggrItem.setText(sym.name());
            aggrItem.setSymbolIndex(sym.firstSection());
            for (int d=1; d<=sym.dimension(); ++d) {
                aggrItem.setCheckState(d, Qt::Unchecked);
            }
            items[aggrItem.symbolIndex()] = aggrItem;
        }
        return items;
    };
    Aggregation aggregation;
    aggregation.setViewType(type());
    aggregation.setAggregationSymbols(Qt::Horizontal, initAggregation(Symbol::Variable));
    aggregation.setAggregationSymbols(Qt::Vertical, initAggregation(Symbol::Equation));
    return aggregation;
}

void TableViewFrame::setIdentifierFilterCheckState(int symbolIndex,
                                                   Qt::CheckState state,
                                                   Qt::Orientation orientation,
                                                   DataSource dataSource)
{
    auto symbols = mCurrentIdentifierFilter[orientation];
    for (auto iter=symbols.begin(); iter!=symbols.end(); ++iter) {
        iter->SymbolType = dataSource;
        if (iter->SymbolIndex == symbolIndex) {
            iter->Checked = state;
            break;
        }
    }
    mCurrentIdentifierFilter[orientation] = symbols;
}

EqnTableViewFrame::EqnTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : TableViewFrame(parent, f)
{

}

TableViewFrame* EqnTableViewFrame::clone(int view)
{
    auto frame = new EqnTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

QAbstractItemModel* EqnTableViewFrame::model() const
{
    return ui->tableView->model();
}

void EqnTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &EqnTableViewFrame::setIdentifierLabelFilter);
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &EqnTableViewFrame::setIdentifierLabelFilter);

    auto eqnModel = new EquationAttributeTableModel(ui->tableView);
    eqnModel->setModelInstance(mModelInstance);
    eqnModel->setView(view);
    mValueFormatModel = new ValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(eqnModel);
    mLabelFilterModel = new LabelFilterModel(mModelInstance, ui->tableView);
    mLabelFilterModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new IdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mLabelFilterModel);
    mIdentifierLabelFilterModel = new IdentifierLabelFilterModel(mModelInstance, ui->tableView);
    mIdentifierLabelFilterModel->setSourceModel(mIdentifierFilterModel);
    mAggregationModel = new AggregationProxyModel(mModelInstance, ui->tableView);
    mAggregationModel->setSourceModel(mIdentifierLabelFilterModel);
    mAggregationModel->setView(view);
    mColumnRowFilterModel = new ColumnRowFilterModel(ui->tableView);
    mColumnRowFilterModel->setSourceModel(mAggregationModel);

    ui->tableView->setHorizontalHeader(mHorizontalHeader);
    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mColumnRowFilterModel);
    delete oldSelectionModel;
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mBaseModel = QSharedPointer<EquationAttributeTableModel>(eqnModel);
}

QList<SearchResult> EqnTableViewFrame::searchHeaders(const QString &term, bool isRegEx)
{
    if (term.isEmpty())
        return QList<SearchResult>();
    QList<SearchResult> result;
    searchHeader(term, isRegEx, mHorizontalHeader->dataSource(), Qt::Horizontal, result);
    searchHeader(term, isRegEx, mVerticalHeader->dataSource(), Qt::Vertical, result);
    return result;
}

void EqnTableViewFrame::searchHeader(const QString &term, bool isRegEx,
                                     DataSource dataSource,
                                     Qt::Orientation orientation,
                                     QList<SearchResult> &result)
{
    if (!mModelInstance) return;
    bool ok;
    int sections = orientation == Qt::Horizontal ? ui->tableView->model()->columnCount() :
                                                   ui->tableView->model()->rowCount();
    for (int s=0; s<sections; ++s) {
        int realSection = ui->tableView->model()->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        mModelInstance->searchHeaderData(s, realSection, term, isRegEx, dataSource, orientation, result);
    }
}

void EqnTableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    TableViewFrame::setIdentifierFilter(filter);
    if (mIdentifierFilterModel) mIdentifierFilterModel->setIdentifierFilter(filter);
}

void EqnTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    TableViewFrame::setLabelFilter(filter);
    if (mLabelFilterModel)
        mLabelFilterModel->setLabelFilter(filter);
    if (mHorizontalHeader)
        mHorizontalHeader->resetSymbolLabelFilters();
    if (mVerticalHeader)
        mVerticalHeader->resetSymbolLabelFilters();
    if (mIdentifierLabelFilterModel) {
        mIdentifierLabelFilterModel->clearIdentifierFilter();
        mIdentifierLabelFilterModel->invalidate();
    }
}

void EqnTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    TableViewFrame::setValueFilter(filter);
    if (mValueFormatModel) mValueFormatModel->setValueFilter(filter);
}

void EqnTableViewFrame::resetColumnRowFilter()
{
    mColumnRowFilterModel->invalidate();
}

Aggregation EqnTableViewFrame::getDefaultAggregation() const
{
    auto initAggregation = [this](Symbol::Type type) {
        AggregationSymbols items;
        Q_FOREACH(const auto& sym, mModelInstance->symbols(type)) {
            AggregationItem aggrItem;
            aggrItem.setText(sym.name());
            aggrItem.setSymbolIndex(sym.firstSection());
            for (int d=1; d<=sym.dimension(); ++d) {
                aggrItem.setCheckState(d, Qt::Unchecked);
            }
            items[aggrItem.symbolIndex()] = aggrItem;
        }
        return items;
    };
    Aggregation aggregation;
    aggregation.setAggregationSymbols(Qt::Vertical, initAggregation(Symbol::Equation));
    aggregation.setViewType(PredefinedViewEnum::EqnAttributes);
    return aggregation;
}

void EqnTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                 Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked,
                                      orientation, state.SymbolType);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter);
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    resetColumnRowFilter();
    emit filtersChanged();
}

VarTableViewFrame::VarTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : TableViewFrame(parent, f)
{

}

TableViewFrame* VarTableViewFrame::clone(int view)
{
    auto frame = new VarTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

QAbstractItemModel* VarTableViewFrame::model() const
{
    return ui->tableView->model();
}

void VarTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    mHorizontalHeader->setDataSource(DataSource::VariableData);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &VarTableViewFrame::setIdentifierLabelFilter);
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setDataSource(DataSource::EquationData);
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &VarTableViewFrame::setIdentifierLabelFilter);

    auto varModel = new VariableAttributeTableModel(ui->tableView);
    varModel->setModelInstance(mModelInstance);
    varModel->setView(view);
    mValueFormatModel = new ValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(varModel);
    mLabelFilterModel = new LabelFilterModel(mModelInstance, ui->tableView);
    mLabelFilterModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new IdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mLabelFilterModel);
    mIdentifierLabelFilterModel = new IdentifierLabelFilterModel(mModelInstance, ui->tableView);
    mIdentifierLabelFilterModel->setSourceModel(mIdentifierFilterModel);
    mAggregationModel = new AggregationProxyModel(mModelInstance, ui->tableView);
    mAggregationModel->setView(view);
    mAggregationModel->setSourceModel(mIdentifierLabelFilterModel);
    mColumnRowFilterModel = new ColumnRowFilterModel(ui->tableView);
    mColumnRowFilterModel->setSourceModel(mAggregationModel);

    ui->tableView->setHorizontalHeader(mHorizontalHeader);
    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mColumnRowFilterModel);
    delete oldSelectionModel;
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mBaseModel = QSharedPointer<VariableAttributeTableModel>(varModel);
}

QList<SearchResult> VarTableViewFrame::searchHeaders(const QString &term, bool isRegEx)
{
    if (term.isEmpty())
        return QList<SearchResult>();
    QList<SearchResult> result;
    searchHeader(term, isRegEx, mHorizontalHeader->dataSource(), Qt::Horizontal, result);
    searchHeader(term, isRegEx, mVerticalHeader->dataSource(), Qt::Vertical, result);
    return result;
}

void VarTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    TableViewFrame::setLabelFilter(filter);
    if (mLabelFilterModel)
        mLabelFilterModel->setLabelFilter(filter);
    if (mHorizontalHeader)
        mHorizontalHeader->resetSymbolLabelFilters();
    if (mVerticalHeader)
        mVerticalHeader->resetSymbolLabelFilters();
    if (mIdentifierLabelFilterModel) {
        mIdentifierLabelFilterModel->clearIdentifierFilter();
        mIdentifierLabelFilterModel->invalidate();
    }
}

void VarTableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    TableViewFrame::setIdentifierFilter(filter);
    if (mIdentifierFilterModel) mIdentifierFilterModel->setIdentifierFilter(filter);
}

void VarTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    TableViewFrame::setValueFilter(filter);
    if (mValueFormatModel) mValueFormatModel->setValueFilter(filter);
}

void VarTableViewFrame::resetColumnRowFilter()
{
    mColumnRowFilterModel->invalidate();
}

void VarTableViewFrame::setupFiltersAggregation(QAbstractItemModel *model,
                                                const LabelFilter &filter)
{
    mCurrentLabelFilter = filter;
    mDefaultLabelFilter = mCurrentLabelFilter;
    mDefaultValueFilter.MinValue = mModelInstance->modelMinimum();
    mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum();
    mCurrentValueFilter = mDefaultValueFilter;
    mDefaultIdentifierFilter[Qt::Horizontal] = defaultSymbolFilter(model, Qt::Horizontal);
    mDefaultIdentifierFilter[Qt::Vertical] = defaultSymbolFilter(model, Qt::Vertical);
    mCurrentIdentifierFilter = mDefaultIdentifierFilter;
    mDefaultAggregation = getDefaultAggregation();
    mCurrentAggregation = mDefaultAggregation;
}

Aggregation VarTableViewFrame::getDefaultAggregation() const
{
    auto initAggregation = [this](Symbol::Type type) {
        AggregationSymbols items;
        Q_FOREACH(const auto& sym, mModelInstance->symbols(type)) {
            AggregationItem aggrItem;
            aggrItem.setText(sym.name());
            aggrItem.setSymbolIndex(sym.firstSection());
            for (int d=1; d<=sym.dimension(); ++d) {
                aggrItem.setCheckState(d, Qt::Unchecked);
            }
            items[aggrItem.symbolIndex()] = aggrItem;
        }
        return items;
    };
    Aggregation aggregation;
    aggregation.setAggregationSymbols(Qt::Horizontal, initAggregation(Symbol::Variable));
    aggregation.setViewType(PredefinedViewEnum::VarAttributes);
    return aggregation;
}

void VarTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                 Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked,
                                      orientation, state.SymbolType);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter);
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    resetColumnRowFilter();
    emit filtersChanged();
}

void VarTableViewFrame::searchHeader(const QString &term, bool isRegEx,
                                     DataSource dataSource,
                                     Qt::Orientation orientation,
                                     QList<SearchResult> &result)
{
    if (!mModelInstance) return;
    bool ok;
    int sections = orientation == Qt::Horizontal ? ui->tableView->model()->columnCount() :
                                                   ui->tableView->model()->rowCount();
    for (int s=0; s<sections; ++s) {
        int realSection = ui->tableView->model()->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        mModelInstance->searchHeaderData(s, realSection, term, isRegEx, dataSource, orientation, result);
    }
}

JaccTableViewFrame::JaccTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : TableViewFrame(parent, f)
    , mBaseModel(new JaccobianTableModel)
{

}

TableViewFrame* JaccTableViewFrame::clone(int view)
{
    auto frame = new JaccTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

QAbstractItemModel* JaccTableViewFrame::model() const
{
    return ui->tableView->model();
}

void JaccTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &JaccTableViewFrame::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &JaccTableViewFrame::setIdentifierLabelFilter);

    auto baseModel = new JaccobianTableModel(ui->tableView);
    baseModel->setModelInstance(mModelInstance);
    baseModel->setView(view);
    mValueFormatModel = new ValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(baseModel);
    mLabelFilterModel = new LabelFilterModel(mModelInstance, ui->tableView);
    mLabelFilterModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new IdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mLabelFilterModel);
    mIdentifierLabelFilterModel = new IdentifierLabelFilterModel(mModelInstance, ui->tableView);
    mIdentifierLabelFilterModel->setSourceModel(mIdentifierFilterModel);
    mAggregationModel = new AggregationProxyModel(mModelInstance, ui->tableView);
    mAggregationModel->setView(view);
    mAggregationModel->setSourceModel(mIdentifierLabelFilterModel);
    mColumnRowFilterModel = new ColumnRowFilterModel(ui->tableView);
    mColumnRowFilterModel->setSourceModel(mAggregationModel);

    ui->tableView->setHorizontalHeader(mHorizontalHeader);
    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mColumnRowFilterModel);
    delete oldSelectionModel;
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mBaseModel = QSharedPointer<JaccobianTableModel>(baseModel);
}

QList<SearchResult> JaccTableViewFrame::searchHeaders(const QString &term, bool isRegEx)
{
    if (term.isEmpty())
        return QList<SearchResult>();
    QList<SearchResult> result;
    searchHeader(term, isRegEx, mHorizontalHeader->dataSource(), Qt::Horizontal, result);
    searchHeader(term, isRegEx, mVerticalHeader->dataSource(), Qt::Vertical, result);
    return result;
}

void JaccTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    TableViewFrame::setLabelFilter(filter);
    if (mLabelFilterModel)
        mLabelFilterModel->setLabelFilter(filter);
    if (mHorizontalHeader)
        mHorizontalHeader->resetSymbolLabelFilters();
    if (mVerticalHeader)
        mVerticalHeader->resetSymbolLabelFilters();
    if (mIdentifierLabelFilterModel) {
        mIdentifierLabelFilterModel->clearIdentifierFilter();
        mIdentifierLabelFilterModel->invalidate();
    }
}

void JaccTableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    TableViewFrame::setIdentifierFilter(filter);
    if (mIdentifierFilterModel) mIdentifierFilterModel->setIdentifierFilter(filter);
}

void JaccTableViewFrame::resetColumnRowFilter()
{
    mColumnRowFilterModel->invalidate();
}

void JaccTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    TableViewFrame::setValueFilter(filter);
    if (mValueFormatModel) mValueFormatModel->setValueFilter(filter);
}

void JaccTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                  Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked,
                                      orientation, state.SymbolType);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter);
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    resetColumnRowFilter();
    emit filtersChanged();
}

void JaccTableViewFrame::searchHeader(const QString &term, bool isRegEx,
                                      DataSource dataSource,
                                      Qt::Orientation orientation,
                                      QList<SearchResult> &result)
{
    if (!mModelInstance) return;
    bool ok;
    int sections = orientation == Qt::Horizontal ? ui->tableView->model()->columnCount() :
                                                   ui->tableView->model()->rowCount();
    for (int s=0; s<sections; ++s) {
        int realSection = ui->tableView->model()->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        mModelInstance->searchHeaderData(s, realSection, term, isRegEx, dataSource, orientation, result);
    }
}

FullTableViewFrame::FullTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : TableViewFrame(parent, f)
    , mModelInstanceModel(new ModelInstanceTableModel)
{

}

TableViewFrame* FullTableViewFrame::clone(int view)
{
    auto frame = new FullTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

QAbstractItemModel* FullTableViewFrame::model() const
{
    return ui->tableView->model();
}

void FullTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &FullTableViewFrame::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &FullTableViewFrame::setIdentifierLabelFilter);

    auto modelInstanceModel = new ModelInstanceTableModel(ui->tableView);
    modelInstanceModel->setModelInstance(mModelInstance);
    modelInstanceModel->setView(view);
    mValueFormatModel = new ValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(modelInstanceModel);
    mLabelFilterModel = new LabelFilterModel(mModelInstance, ui->tableView);
    mLabelFilterModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new IdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mLabelFilterModel);
    mIdentifierLabelFilterModel = new IdentifierLabelFilterModel(mModelInstance, ui->tableView);
    mIdentifierLabelFilterModel->setSourceModel(mIdentifierFilterModel);
    mAggregationModel = new AggregationProxyModel(mModelInstance, ui->tableView);
    mAggregationModel->setView(view);
    mAggregationModel->setSourceModel(mIdentifierLabelFilterModel);
    mColumnRowFilterModel = new ColumnRowFilterModel(ui->tableView);
    mColumnRowFilterModel->setSourceModel(mAggregationModel);

    ui->tableView->setHorizontalHeader(mHorizontalHeader);
    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mColumnRowFilterModel);
    delete oldSelectionModel;
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mModelInstanceModel = QSharedPointer<ModelInstanceTableModel>(modelInstanceModel);
}

QList<SearchResult> FullTableViewFrame::searchHeaders(const QString &term, bool isRegEx)
{
    if (term.isEmpty())
        return QList<SearchResult>();
    QList<SearchResult> result;
    searchHeader(term, isRegEx, mHorizontalHeader->dataSource(), Qt::Horizontal, result);
    searchHeader(term, isRegEx, mVerticalHeader->dataSource(), Qt::Vertical, result);
    return result;
}

void FullTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    TableViewFrame::setLabelFilter(filter);
    if (mLabelFilterModel)
        mLabelFilterModel->setLabelFilter(filter);
    if (mHorizontalHeader)
        mHorizontalHeader->resetSymbolLabelFilters();
    if (mVerticalHeader)
        mVerticalHeader->resetSymbolLabelFilters();
    if (mIdentifierLabelFilterModel) {
        mIdentifierLabelFilterModel->clearIdentifierFilter();
        mIdentifierLabelFilterModel->invalidate();
    }
}

void FullTableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    TableViewFrame::setIdentifierFilter(filter);
    if (mIdentifierFilterModel) mIdentifierFilterModel->setIdentifierFilter(filter);
}

void FullTableViewFrame::resetColumnRowFilter()
{
    mColumnRowFilterModel->invalidate();
}

void FullTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    TableViewFrame::setValueFilter(filter);
    if (mValueFormatModel) mValueFormatModel->setValueFilter(filter);
}

void FullTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                  Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked,
                                      orientation, state.SymbolType);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter);
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    resetColumnRowFilter();
    emit filtersChanged();
}

void FullTableViewFrame::searchHeader(const QString &term, bool isRegEx,
                                      DataSource dataSource,
                                      Qt::Orientation orientation,
                                      QList<SearchResult> &result)
{
    if (!mModelInstance) return;
    bool ok;
    int sections = orientation == Qt::Horizontal ? ui->tableView->model()->columnCount() :
                                                   ui->tableView->model()->rowCount();
    for (int s=0; s<sections; ++s) {
        int realSection = ui->tableView->model()->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        mModelInstance->searchHeaderData(s, realSection, term, isRegEx, dataSource, orientation, result);
    }
}

MinMaxTableViewFrame::MinMaxTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : TableViewFrame(parent, f)
    , mModelInstanceModel(new MinMaxModelInstanceTableModel)
    , mSelectionMenu(new QMenu(this))
{
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    setupSelectionMenu();
    connect(ui->tableView, &QWidget::customContextMenuRequested,
            this, &MinMaxTableViewFrame::customMenuRequested);
}

TableViewFrame* MinMaxTableViewFrame::clone(int view)
{
    auto frame = new MinMaxTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

QAbstractItemModel* MinMaxTableViewFrame::model() const
{
    return ui->tableView->model();
}

void MinMaxTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &MinMaxTableViewFrame::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &MinMaxTableViewFrame::setIdentifierLabelFilter);

    auto modelInstanceModel = new MinMaxModelInstanceTableModel(ui->tableView);
    modelInstanceModel->setModelInstance(mModelInstance);
    modelInstanceModel->setView(view);
    mValueFormatModel = new ValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(modelInstanceModel);
    mLabelFilterModel = new LabelFilterModel(mModelInstance, ui->tableView);
    mLabelFilterModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new IdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mLabelFilterModel);
    mIdentifierLabelFilterModel = new IdentifierLabelFilterModel(mModelInstance, ui->tableView);
    mIdentifierLabelFilterModel->setSourceModel(mIdentifierFilterModel);
    mColumnRowFilterModel = new ColumnRowFilterModel(ui->tableView);
    mColumnRowFilterModel->setSourceModel(mIdentifierLabelFilterModel);

    ui->tableView->setHorizontalHeader(mHorizontalHeader);
    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mColumnRowFilterModel);
    delete oldSelectionModel;
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mModelInstanceModel = QSharedPointer<MinMaxModelInstanceTableModel>(modelInstanceModel);
}

QList<SearchResult> MinMaxTableViewFrame::searchHeaders(const QString &term, bool isRegEx)
{
    if (term.isEmpty())
        return QList<SearchResult>();
    QList<SearchResult> result;
    searchHeader(term, isRegEx, mHorizontalHeader->dataSource(), Qt::Horizontal, result);
    searchHeader(term, isRegEx, mVerticalHeader->dataSource(), Qt::Vertical, result);
    return result;
}

void MinMaxTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    TableViewFrame::setLabelFilter(filter);
    if (mLabelFilterModel)
        mLabelFilterModel->setLabelFilter(filter);
    if (mHorizontalHeader)
        mHorizontalHeader->resetSymbolLabelFilters();
    if (mVerticalHeader)
        mVerticalHeader->resetSymbolLabelFilters();
    if (mIdentifierLabelFilterModel) {
        mIdentifierLabelFilterModel->clearIdentifierFilter();
        mIdentifierLabelFilterModel->invalidate();
    }
}

void MinMaxTableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    TableViewFrame::setIdentifierFilter(filter);
    if (mIdentifierFilterModel) mIdentifierFilterModel->setIdentifierFilter(filter);
}

void MinMaxTableViewFrame::resetColumnRowFilter()
{
    if (mColumnRowFilterModel) mColumnRowFilterModel->invalidate();
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
    resetColumnRowFilter();
}

void MinMaxTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    TableViewFrame::setValueFilter(filter);
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
}

void MinMaxTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                    Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked,
                                      orientation, state.SymbolType);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter);
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    resetColumnRowFilter();
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
                            appliedAggregation.indexToSymbol(DataSource::VariableData)[i] = symbol;
                        }
                        lastSymEndIndex += item.visibleSectionCount();
                        map[mapIter->first][item.symbolIndex()] = item;
                        break;
                    } else {
                        if (appliedAggregation.indexToSymbol(DataSource::VariableData).contains(symbol.firstSection()))
                            continue;
                        if (lastSymEndIndex < 0) lastSymEndIndex = item.symbolIndex();
                        int sectionCount = symbol.entries();
                        for (int i=lastSymEndIndex; i<lastSymEndIndex+sectionCount; ++i) {
                            appliedAggregation.indexToSymbol(DataSource::VariableData)[i] = symbol;
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
                        appliedAggregation.indexToSymbol(DataSource::EquationData)[item.newSymbolIndex()] = symbol;
                        appliedAggregation.indexToSymbol(DataSource::EquationData)[item.newSymbolIndex()+1] = symbol;
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

void MinMaxTableViewFrame::searchHeader(const QString &term, bool isRegEx,
                                        DataSource dataSource,
                                        Qt::Orientation orientation,
                                        QList<SearchResult> &result)
{
    if (!mModelInstance) return;
    bool ok;
    int sections = orientation == Qt::Horizontal ? ui->tableView->model()->columnCount() :
                                                   ui->tableView->model()->rowCount();
    for (int s=0; s<sections; ++s) {
        int realSection = ui->tableView->model()->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        mModelInstance->searchHeaderData(s, realSection, term, isRegEx, dataSource, orientation, result);
    }
}
}
}
}
