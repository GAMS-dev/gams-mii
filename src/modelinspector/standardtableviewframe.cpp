#include "standardtableviewframe.h"
#include "ui_standardtableviewframe.h"
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

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

AbstractStandardTableViewFrame::AbstractStandardTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractTableViewFrame(parent, f)
{

}

void AbstractStandardTableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    mCurrentIdentifierFilter = filter;
    if (mIdentifierFilterModel) mIdentifierFilterModel->setIdentifierFilter(filter);
}

const Aggregation &AbstractStandardTableViewFrame::appliedAggregation() const
{
    return mAggregationModel ? mAggregationModel->appliedAggregation()
                             : mCurrentAggregation;
}

void AbstractStandardTableViewFrame::setAggregation(const Aggregation &aggregation, int view)
{
    mCurrentAggregation = aggregation;
    mCurrentAggregation.setView(view);
    if (mAggregationModel && mHorizontalHeader && mVerticalHeader) {
        auto applied = AbstractTableViewFrame::appliedAggregation(aggregation, view);
        mHorizontalHeader->setAppliedAggregation(applied);
        mVerticalHeader->setAppliedAggregation(applied);
        mAggregationModel->setAggregation(mCurrentAggregation, applied);
    }
}

void AbstractStandardTableViewFrame::setShowAbsoluteValues(bool absoluteValues)
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
    updateValueFilter();
}

void AbstractStandardTableViewFrame::setupFiltersAggregation(QAbstractItemModel *model,
                                                             const LabelFilter &filter)
{
    mCurrentLabelFilter = filter;
    mDefaultLabelFilter = mCurrentLabelFilter;
    mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(type());
    mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(type());
    mCurrentValueFilter = mDefaultValueFilter;
    mDefaultIdentifierFilter[Qt::Horizontal] = defaultSymbolFilter(model, Qt::Horizontal);
    mDefaultIdentifierFilter[Qt::Vertical] = defaultSymbolFilter(model, Qt::Vertical);
    mCurrentIdentifierFilter = mDefaultIdentifierFilter;
    mDefaultAggregation = getDefaultAggregation();
    mCurrentAggregation = mDefaultAggregation;
}

void AbstractStandardTableViewFrame::reset(int view)
{
    setIdentifierFilter(mDefaultIdentifierFilter);
    setValueFilter(mDefaultValueFilter);
    setLabelFilter(mDefaultLabelFilter);
    setAggregation(mDefaultAggregation, (int)view);
    updateView();
}

void AbstractStandardTableViewFrame::updateView()
{
    if (mColumnRowFilterModel) mColumnRowFilterModel->invalidate();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    updateValueFilter();
    emit filtersChanged();
}

void AbstractStandardTableViewFrame::setIdentifierFilterCheckState(int symbolIndex,
                                                                   Qt::CheckState state,
                                                                   Qt::Orientation orientation)
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

void AbstractStandardTableViewFrame::cloneFilterAndAggregation(AbstractStandardTableViewFrame *clone, int newView)
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

void AbstractStandardTableViewFrame::updateValueFilter()
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

EqnTableViewFrame::EqnTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{

}

AbstractTableViewFrame* EqnTableViewFrame::clone(int view)
{
    auto frame = new EqnTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

void EqnTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    mHorizontalHeader->setViewType(PredefinedViewEnum::EqnAttributes);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &EqnTableViewFrame::setIdentifierLabelFilter);
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(PredefinedViewEnum::EqnAttributes);
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

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

void EqnTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    AbstractStandardTableViewFrame::setLabelFilter(filter);
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
    AbstractStandardTableViewFrame::setValueFilter(filter);
    if (mValueFormatModel) mValueFormatModel->setValueFilter(filter);
}

Aggregation EqnTableViewFrame::getDefaultAggregation() const
{
    auto initAggregation = [this](Symbol::Type type) {
        AggregationSymbols items;
        Q_FOREACH(const auto& sym, mModelInstance->symbols(type)) {
            AggregationItem aggrItem;
            aggrItem.setText(sym.name());
            aggrItem.setSymbolIndex(sym.firstSection());
            aggrItem.setDomainLabels(sym.domainLabels());
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
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter);
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    updateView();
    emit filtersChanged();
}

VarTableViewFrame::VarTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{

}

AbstractTableViewFrame* VarTableViewFrame::clone(int view)
{
    auto frame = new VarTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

void VarTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    mHorizontalHeader->setViewType(PredefinedViewEnum::VarAttributes);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &VarTableViewFrame::setIdentifierLabelFilter);
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(PredefinedViewEnum::VarAttributes);
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

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

void VarTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    AbstractStandardTableViewFrame::setLabelFilter(filter);
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

void VarTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    AbstractStandardTableViewFrame::setValueFilter(filter);
    if (mValueFormatModel) mValueFormatModel->setValueFilter(filter);
}

void VarTableViewFrame::setupFiltersAggregation(QAbstractItemModel *model,
                                                const LabelFilter &filter)
{
    mCurrentLabelFilter = filter;
    mDefaultLabelFilter = mCurrentLabelFilter;
    mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(type());
    mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(type());
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
            aggrItem.setDomainLabels(sym.domainLabels());
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
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter);
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    updateView();
    emit filtersChanged();
}

JaccTableViewFrame::JaccTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
    , mBaseModel(new JaccobianTableModel)
{

}

AbstractTableViewFrame* JaccTableViewFrame::clone(int view)
{
    auto frame = new JaccTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

void JaccTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    mHorizontalHeader->setViewType(PredefinedViewEnum::Jaccobian);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &JaccTableViewFrame::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(PredefinedViewEnum::Jaccobian);
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

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

void JaccTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    AbstractStandardTableViewFrame::setLabelFilter(filter);
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

void JaccTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    AbstractStandardTableViewFrame::setValueFilter(filter);
    if (mValueFormatModel) mValueFormatModel->setValueFilter(filter);
}

void JaccTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                  Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter);
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    updateView();
    emit filtersChanged();
}

FullTableViewFrame::FullTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
    , mModelInstanceModel(new ModelInstanceTableModel)
{

}

AbstractTableViewFrame* FullTableViewFrame::clone(int view)
{
    auto frame = new FullTableViewFrame;
    frame->setupView(mModelInstance, view);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

void FullTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view)
{
    mModelInstance = modelInstance;
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    mHorizontalHeader->setViewType(PredefinedViewEnum::Full);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &FullTableViewFrame::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(PredefinedViewEnum::Full);
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

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

void FullTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    AbstractStandardTableViewFrame::setLabelFilter(filter);
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

void FullTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    AbstractStandardTableViewFrame::setValueFilter(filter);
    if (mValueFormatModel) mValueFormatModel->setValueFilter(filter);
}

void FullTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                  Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
        mIdentifierFilterModel->setIdentifierFilter(mCurrentIdentifierFilter);
    } else {
        mCurrentIdentifierFilter[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    updateView();
    emit filtersChanged();
}

}
}
}
