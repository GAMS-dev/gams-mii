#include "bpviewframe.h"
#include "abstractmodelinstance.h"
#include "viewconfigurationprovider.h"
#include "comprehensivetablemodel.h"
#include "ui_standardtableviewframe.h"
#include "standardtableviewframe.h"
#include "hierarchicalheaderview.h"
#include "bpscalingidentifierfiltermodel.h"
#include "valueformatproxymodel.h"

#include <QAction>
#include <QMenu>
#include <QSharedPointer>

namespace gams {
namespace studio{
namespace modelinspector {

BPOverviewViewFrame::BPOverviewViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BPOverviewViewFrame::BPOverviewViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                                     QSharedPointer<AbstractViewConfiguration> viewConfig,
                                                     QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mModelInstance = modelInstance;
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *BPOverviewViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Overview,
                                                                                                        mModelInstance));
    auto frame = new BPOverviewViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void BPOverviewViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

void BPOverviewViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void BPOverviewViewFrame::setIdentifierLabelFilter(const IdentifierState &state,
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

void BPOverviewViewFrame::setupView()
{
    auto baseModel = new BPOverviewTableModel(mViewConfig->view(),
                                                    mModelInstance,
                                                    ui->tableView);

    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(baseModel);
    delete oldSelectionModel;

    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setVisible(true);
    mBaseModel = QSharedPointer<BPOverviewTableModel>(baseModel);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

BPCountViewFrame::BPCountViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BPCountViewFrame::BPCountViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                               QSharedPointer<AbstractViewConfiguration> viewConfig,
                                               QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mModelInstance = modelInstance;
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *BPCountViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Count,
                                                                                                        mModelInstance));
    auto frame = new BPCountViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void BPCountViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mViewConfig->initialize(nullptr);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

void BPCountViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void BPCountViewFrame::setIdentifierLabelFilter(const IdentifierState &state, Qt::Orientation orientation)
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

void BPCountViewFrame::setupView()
{
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(ViewDataType::BP_Count);
    mVerticalHeader->setView(mViewConfig->view());
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &BPCountViewFrame::setIdentifierLabelFilter);
    
    auto baseModel = new BPCountTableModel(mViewConfig->view(),
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

BPAverageViewFrame::BPAverageViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BPAverageViewFrame::BPAverageViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                                   QSharedPointer<AbstractViewConfiguration> viewConfig,
                                                   QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mModelInstance = modelInstance;
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *BPAverageViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Average,
                                                                                                        mModelInstance));
    auto frame = new BPAverageViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void BPAverageViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mViewConfig->initialize(nullptr);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

void BPAverageViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void BPAverageViewFrame::setIdentifierLabelFilter(const IdentifierState &state, Qt::Orientation orientation)
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

void BPAverageViewFrame::setupView()
{
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(ViewDataType::BP_Average);
    mVerticalHeader->setView(mViewConfig->view());
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &BPAverageViewFrame::setIdentifierLabelFilter);
    
    auto baseModel = new BPAverageTableModel(mViewConfig->view(),
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

BPScalingViewFrame::BPScalingViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractTableViewFrame(parent, f)
    , mBaseModel(new ComprehensiveTableModel)
    , mSelectionMenu(new QMenu(this))
{
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Scaling));
    mSelectionMenu->addAction(mSymbolAction);
    connect(mSymbolAction, &QAction::triggered, this, [this]{handleRowColumnSelection();});
    connect(ui->tableView, &QWidget::customContextMenuRequested,
            this, &BPScalingViewFrame::customMenuRequested);
}

BPScalingViewFrame::BPScalingViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                       QSharedPointer<AbstractViewConfiguration> viewConfig,
                                       QWidget *parent, Qt::WindowFlags f)
    : AbstractTableViewFrame(parent, f)
    , mBaseModel(new ComprehensiveTableModel)
    , mSelectionMenu(new QMenu(this))
{
    mModelInstance = modelInstance;
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = viewConfig;
    mSelectionMenu->addAction(mSymbolAction);
    connect(mSymbolAction, &QAction::triggered, this, [this]{handleRowColumnSelection();});
    connect(ui->tableView, &QWidget::customContextMenuRequested,
            this, &BPScalingViewFrame::customMenuRequested);
}

AbstractTableViewFrame* BPScalingViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Scaling,
                                                                                                        mModelInstance));
    auto frame = new BPScalingViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void BPScalingViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    mViewConfig->setCurrentIdentifierFilter(filter);
    if (mIdentifierFilterModel)
        mIdentifierFilterModel->setIdentifierFilter(filter,
                                                    mViewConfig->currentAggregation());
}

void BPScalingViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mViewConfig->initialize(nullptr);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

const QList<Symbol*>& BPScalingViewFrame::selectedEquations() const
{
    return mSelectedEquations;
}

const QList<Symbol*>& BPScalingViewFrame::selectedVariables() const
{
    return mSelectedVariables;
}

void BPScalingViewFrame::reset()
{
    setIdentifierFilter(mViewConfig->defaultIdentifierFilter());
    setValueFilter(mViewConfig->defaultValueFilter());
    setLabelFilter(mViewConfig->defaultLabelFilter());
    updateView();
}

void BPScalingViewFrame::updateView()
{
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void BPScalingViewFrame::setValueFilter(const ValueFilter &filter)
{
    mViewConfig->setCurrentValueFilter(filter);
    if (mViewConfig->currentAggregation().useAbsoluteValues() != mViewConfig->currentValueFilter().UseAbsoluteValues) {
        setShowAbsoluteValues(mViewConfig->currentValueFilter().UseAbsoluteValues);
    } else if (mValueFormatModel) {
        mValueFormatModel->setValueFilter(filter);
    }
}

void BPScalingViewFrame::setAggregation(const Aggregation &aggregation)
{
    if (mBaseModel && mViewConfig->currentAggregation().useAbsoluteValues() != aggregation.useAbsoluteValues()) {
        mViewConfig->currentAggregation().setUseAbsoluteValues(aggregation.useAbsoluteValues());
        mModelInstance->loadData(mViewConfig);
        emit mBaseModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});
        mViewConfig->currentValueFilter().UseAbsoluteValues = aggregation.useAbsoluteValues();
        mValueFormatModel->setValueFilter(mViewConfig->currentValueFilter());
        emit filtersChanged();
    }
}

void BPScalingViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mBaseModel && mViewConfig->currentValueFilter().isAbsolute() != absoluteValues) {
        mViewConfig->currentAggregation().setUseAbsoluteValues(absoluteValues);
        mViewConfig->currentValueFilter().UseAbsoluteValues = absoluteValues;
        mModelInstance->loadData(mViewConfig);
        emit mBaseModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});
        mValueFormatModel->setValueFilter(mViewConfig->currentValueFilter());
    }
}

void BPScalingViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                                  Qt::Orientation orientation)
{
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
        mIdentifierFilterModel->setIdentifierFilter(mViewConfig->currentIdentifierFilter(),
                                                    mViewConfig->currentAggregation());
    } else {
        mViewConfig->currentIdentifierFilter()[orientation][state.SymbolIndex] = state;
    }
    updateView();
    emit filtersChanged();
}

void BPScalingViewFrame::customMenuRequested(const QPoint &pos)
{
    if (ui->tableView->selectionModel()->selectedIndexes().empty())
        return;
    if (ui->tableView->selectionModel()->selectedIndexes().first().row() < mModelInstance->equationCount()*2 &&
        ui->tableView->selectionModel()->selectedIndexes().first().column() < mModelInstance->variableCount()) {
        mSymbolAction->setEnabled(true);
    } else {
        mSymbolAction->setEnabled(false);
    }
    mSelectionMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void BPScalingViewFrame::setupView()
{
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(ViewDataType::BP_Scaling);
    mVerticalHeader->setView(mViewConfig->view());
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &BPScalingViewFrame::setIdentifierLabelFilter);

    auto baseModel = new ComprehensiveTableModel(ui->tableView);
    baseModel->setModelInstance(mModelInstance);
    baseModel->setView(mViewConfig->view());
    mValueFormatModel = new MinMaxValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(baseModel);
    mIdentifierFilterModel = new BPScalingIdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mValueFormatModel);

    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mIdentifierFilterModel);
    delete oldSelectionModel;
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->verticalHeader()->setVisible(true);
    mBaseModel = QSharedPointer<ComprehensiveTableModel>(baseModel);

    const auto& applied = mViewConfig->currentAggregation();
    mVerticalHeader->setAppliedAggregation(applied);
    emit mBaseModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

void BPScalingViewFrame::handleRowColumnSelection()
{
    QMap<int, Symbol*> rowSymbols, columnSymbols;
    auto indexes = ui->tableView->selectionModel()->selectedIndexes();
    int section;
    for (const auto& index : indexes) {
        // TODO !!! drop this aggregation stuff (indexToVariable)...
        // not needed anymore or fix this... check HHeader implementation
        section = ui->tableView->model()->headerData(index.row(), Qt::Vertical,
                                                     Mi::IndexDataRole).toInt();
        if (mViewConfig->currentAggregation().indexToEquation().contains(section)) {
            auto symbol = mViewConfig->currentAggregation().indexToEquation()[section];
            rowSymbols[symbol->firstSection()] = symbol;
        } else {
            auto symbol = mModelInstance->equation(section);
            rowSymbols[symbol->firstSection()] = symbol;
        }
        section = ui->tableView->model()->headerData(index.column(), Qt::Horizontal,
                                                     Mi::IndexDataRole).toInt();
        auto symbol = mModelInstance->variable(section);
        columnSymbols[symbol->firstSection()] = symbol;
    }
    mSelectedEquations = rowSymbols.values();
    mSelectedVariables = columnSymbols.values();
    emit newSymbolViewRequested();
}

void BPScalingViewFrame::setIdentifierFilterCheckState(int symbolIndex,
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

}
}
}
