#include "standardtableviewframe.h"
#include "ui_standardtableviewframe.h"
#include "hierarchicalheaderview.h"
#include "identifierfiltermodel.h"
#include "labelfiltermodel.h"
#include "valueformatproxymodel.h"
#include "jaccobiantablemodel.h"
#include "abstractmodelinstance.h"
#include "viewconfigurationprovider.h"

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
    mViewConfig->setCurrentIdentifierFilter(filter);
    if (mIdentifierFilterModel) mIdentifierFilterModel->setIdentifierFilter(filter);
}

void AbstractStandardTableViewFrame::setAggregation(const Aggregation &aggregation)
{
    Q_UNUSED(aggregation);
    //mViewConfig->setCurrentAggregation(aggregation);
    //if (mAggregationModel && mHorizontalHeader && mVerticalHeader) {
    //    const auto& applied = mViewConfig->currentAggregation();
    //    mHorizontalHeader->setAppliedAggregation(applied);
    //    mVerticalHeader->setAppliedAggregation(applied);
    //    mAggregationModel->setAggregation(mViewConfig->currentAggregation(), applied);
    //}
}

void AbstractStandardTableViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    Q_UNUSED(absoluteValues);
    //if (mAggregationModel && mAggregationModel->appliedAggregation().isActive() &&
    //        mAggregationModel->appliedAggregation().useAbsoluteValues() != absoluteValues) {
    //    auto aggregation = mAggregationModel->aggregation();
    //    aggregation.setUseAbsoluteValues(absoluteValues);
    //    auto appliedAggregation = mAggregationModel->appliedAggregation();
    //    appliedAggregation.setUseAbsoluteValues(absoluteValues);
    //    mViewConfig->currentAggregation().setUseAbsoluteValues(absoluteValues);
    //    mAggregationModel->setAggregation(aggregation, appliedAggregation);
    //} else {
    //    mViewConfig->currentAggregation().setUseAbsoluteValues(absoluteValues);
    //}
    //mViewConfig->currentValueFilter().UseAbsoluteValues = absoluteValues;
    //mViewConfig->currentValueFilter().UseAbsoluteValuesGlobal = absoluteValues;
    //mValueFormatModel->setValueFilter(mViewConfig->currentValueFilter());
    //updateValueFilter(); TODO VF
}

void AbstractStandardTableViewFrame::reset()
{
    setIdentifierFilter(mViewConfig->defaultIdentifierFilter());
    setValueFilter(mViewConfig->defaultValueFilter());
    setLabelFilter(mViewConfig->defaultLabelFilter());
    setAggregation(mViewConfig->defaultAggregation());
    updateView();
}

void AbstractStandardTableViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void AbstractStandardTableViewFrame::setIdentifierFilterCheckState(int symbolIndex,
                                                                   Qt::CheckState state,
                                                                   Qt::Orientation orientation)
{
    auto symbols = mViewConfig->currentIdentifierFilter()[orientation];
    for (auto iter=symbols.begin(); iter!=symbols.end(); ++iter) {
        if (iter->SymbolIndex == symbolIndex) {
            iter->Checked = state;
            break;
        }
    }
    mViewConfig->currentIdentifierFilter()[orientation] = symbols;
}

void AbstractStandardTableViewFrame::cloneFilterAndAggregation(AbstractStandardTableViewFrame *clone, int newView)
{// TODO !!! remove, like MinMax/Symbol view?
    if (!clone) return;
    clone->mViewConfig->setView(newView);
    clone->setValueFilter(mViewConfig->currentValueFilter());
    clone->setDefaultValueFilter(mViewConfig->defaultValueFilter());
    clone->setLabelFilter(mViewConfig->currentLabelFiler());
    clone->setDefaultLabelFilter(mViewConfig->defaultLabelFilter());
    clone->setIdentifierFilter(mViewConfig->currentIdentifierFilter());
    clone->setDefaultIdentifierFilter(mViewConfig->defaultIdentifierFilter());
    clone->setAggregation(mViewConfig->currentAggregation());
    clone->setDefaultAggregation(mViewConfig->defaultAggregation());
}

JaccTableViewFrame::JaccTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
    , mBaseModel(new JaccobianTableModel)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::defaultConfiguration());
}

AbstractTableViewFrame* JaccTableViewFrame::clone(int view)
{
    auto frame = new JaccTableViewFrame;
    frame->setView(view);
    frame->setupView(mModelInstance);
    cloneFilterAndAggregation(frame, view);
    return frame;
}

void JaccTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type(), mModelInstance));
    mModelInstance->loadData(mViewConfig);
    setupView();
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

void JaccTableViewFrame::updateView()
{
    if (mIdentifierFilterModel) mIdentifierFilterModel->setIdentifierFilter(mViewConfig->currentIdentifierFilter());
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void JaccTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                  Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
        mIdentifierFilterModel->setIdentifierFilter(mViewConfig->currentIdentifierFilter());
    } else {
        mViewConfig->currentIdentifierFilter()[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    updateView();
    emit filtersChanged();
}

void JaccTableViewFrame::setupView()
{
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    mHorizontalHeader->setViewType(type());
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &JaccTableViewFrame::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(type());
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &JaccTableViewFrame::setIdentifierLabelFilter);

    auto baseModel = new JaccobianTableModel(ui->tableView);
    baseModel->setModelInstance(mModelInstance);
    baseModel->setView(mViewConfig->view());
    mValueFormatModel = new JaccobianValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(baseModel);
    mLabelFilterModel = new LabelFilterModel(mModelInstance, ui->tableView);
    mLabelFilterModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new IdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mLabelFilterModel);
    mIdentifierLabelFilterModel = new IdentifierLabelFilterModel(mModelInstance, ui->tableView);
    mIdentifierLabelFilterModel->setSourceModel(mIdentifierFilterModel);

    ui->tableView->setHorizontalHeader(mHorizontalHeader);
    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mIdentifierLabelFilterModel);
    delete oldSelectionModel;
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mBaseModel = QSharedPointer<JaccobianTableModel>(baseModel);

    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
}

}
}
}
