#include "blockpicviewframe.h"
#include "abstractmodelinstance.h"
#include "viewconfigurationprovider.h"
#include "comprehensivetablemodel.h"
#include "ui_standardtableviewframe.h"
#include "standardtableviewframe.h"
#include "hierarchicalheaderview.h"

#include <QSharedPointer>

namespace gams {
namespace studio{
namespace modelinspector {

BlockpicOverviewViewFrame::BlockpicOverviewViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BlockpicOverviewViewFrame::BlockpicOverviewViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                                     QSharedPointer<AbstractViewConfiguration> viewConfig,
                                                     QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mModelInstance = modelInstance;
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *BlockpicOverviewViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Overview,
                                                                                                        mModelInstance));
    auto frame = new BlockpicOverviewViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void BlockpicOverviewViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

void BlockpicOverviewViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void BlockpicOverviewViewFrame::setIdentifierLabelFilter(const IdentifierState &state,
                                                         Qt::Orientation orientation)
{// TODO !!! finalize
    //if (state.disabled() && mIdentifierFilterModel) {
    //    setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
    //    //mIdentifierFilterModel->setIdentifierFilter(mViewConfig->currentIdentifierFilter(),
    //    //                                            mViewConfig->currentAggregation());
    //} else {
    //    mViewConfig->currentIdentifierFilter()[orientation][state.SymbolIndex] = state;
    //}
    //updateView();
    //emit filtersChanged();
}

void BlockpicOverviewViewFrame::setupView()
{
    auto baseModel = new BlockpicOverviewTableModel(mViewConfig->view(),
                                                    mModelInstance,
                                                    ui->tableView);

    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(baseModel);
    delete oldSelectionModel;

    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setVisible(true);
    mBaseModel = QSharedPointer<BlockpicOverviewTableModel>(baseModel);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

BlockpicCountViewFrame::BlockpicCountViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BlockpicCountViewFrame::BlockpicCountViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                               QSharedPointer<AbstractViewConfiguration> viewConfig,
                                               QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mModelInstance = modelInstance;
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *BlockpicCountViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Count,
                                                                                                        mModelInstance));
    auto frame = new BlockpicCountViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void BlockpicCountViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mViewConfig->initialize(nullptr);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

void BlockpicCountViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void BlockpicCountViewFrame::setIdentifierLabelFilter(const IdentifierState &state, Qt::Orientation orientation)
{// TODO !!! finalize
    //if (state.disabled() && mIdentifierFilterModel) {
    //    setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
    //    //mIdentifierFilterModel->setIdentifierFilter(mViewConfig->currentIdentifierFilter(),
    //    //                                            mViewConfig->currentAggregation());
    //} else {
    //    mViewConfig->currentIdentifierFilter()[orientation][state.SymbolIndex] = state;
    //}
    //updateView();
    //emit filtersChanged();
}

void BlockpicCountViewFrame::setupView()
{
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(ViewDataType::BP_Count);
    mVerticalHeader->setView(mViewConfig->view());
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &BlockpicCountViewFrame::setIdentifierLabelFilter);

    auto baseModel = new BlockpicCountTableModel(mViewConfig->view(),
                                                 mModelInstance,
                                                 ui->tableView);

    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(baseModel);
    delete oldSelectionModel;
    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setVisible(true);
    mBaseModel = QSharedPointer<ComprehensiveTableModel>(baseModel);

    const auto& applied = mViewConfig->currentAggregation();
    mVerticalHeader->setAppliedAggregation(applied);
    emit mBaseModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

BlockpicAverageViewFrame::BlockpicAverageViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BlockpicAverageViewFrame::BlockpicAverageViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                                   QSharedPointer<AbstractViewConfiguration> viewConfig,
                                                   QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mModelInstance = modelInstance;
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *BlockpicAverageViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Average,
                                                                                                        mModelInstance));
    auto frame = new BlockpicAverageViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void BlockpicAverageViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mViewConfig->initialize(nullptr);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

void BlockpicAverageViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void BlockpicAverageViewFrame::setIdentifierLabelFilter(const IdentifierState &state, Qt::Orientation orientation)
{
    // TODO !!! finalize
    //if (state.disabled() && mIdentifierFilterModel) {
    //    setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
    //    //mIdentifierFilterModel->setIdentifierFilter(mViewConfig->currentIdentifierFilter(),
    //    //                                            mViewConfig->currentAggregation());
    //} else {
    //    mViewConfig->currentIdentifierFilter()[orientation][state.SymbolIndex] = state;
    //}
    //updateView();
    //emit filtersChanged();
}

void BlockpicAverageViewFrame::setupView()
{
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(ViewDataType::BP_Average);
    mVerticalHeader->setView(mViewConfig->view());
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &BlockpicAverageViewFrame::setIdentifierLabelFilter);

    auto baseModel = new BlockpicAverageTableModel(mViewConfig->view(),
                                                   mModelInstance,
                                                   ui->tableView);

    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(baseModel);
    delete oldSelectionModel;
    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setVisible(true);
    mBaseModel = QSharedPointer<ComprehensiveTableModel>(baseModel);

    const auto& applied = mViewConfig->currentAggregation();
    mVerticalHeader->setAppliedAggregation(applied);
    emit mBaseModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

}
}
}
