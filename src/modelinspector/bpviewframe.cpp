#include "bpviewframe.h"
#include "abstractmodelinstance.h"
#include "viewconfigurationprovider.h"
#include "comprehensivetablemodel.h"
#include "ui_standardtableviewframe.h"
#include "hierarchicalheaderview.h"
#include "bpidentifierfiltermodel.h"
#include "valueformatproxymodel.h"

#include <QAction>
#include <QMenu>
#include <QSharedPointer>

namespace gams {
namespace studio{
namespace modelinspector {

AbstractBPViewFrame::AbstractBPViewFrame(ComprehensiveTableModel *model,
                                         QWidget *parent,
                                         Qt::WindowFlags f)
    : AbstractTableViewFrame(parent, f)
    , mBaseModel(model)
    , mSelectionMenu(new QMenu(this))
{
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mSelectionMenu->addAction(mSymbolAction);
    connect(mSymbolAction, &QAction::triggered, this, [this]{handleRowColumnSelection();});
    connect(ui->tableView, &QWidget::customContextMenuRequested,
            this, &AbstractBPViewFrame::customMenuRequested);
}

AbstractBPViewFrame::~AbstractBPViewFrame()
{

}

const QList<Symbol*>& AbstractBPViewFrame::selectedEquations() const
{
    return mSelectedEquations;
}

const QList<Symbol*>& AbstractBPViewFrame::selectedVariables() const
{
    return mSelectedVariables;
}

void AbstractBPViewFrame::setAggregation(const Aggregation &aggregation)
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

void AbstractBPViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    mViewConfig->setCurrentIdentifierFilter(filter);
    if (mIdentifierFilterModel)
        mIdentifierFilterModel->setIdentifierFilter(filter);
}

void AbstractBPViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mBaseModel && mViewConfig->currentValueFilter().isAbsolute() != absoluteValues) {
        mViewConfig->currentAggregation().setUseAbsoluteValues(absoluteValues);
        mViewConfig->currentValueFilter().UseAbsoluteValues = absoluteValues;
        mModelInstance->loadData(mViewConfig);
        emit mBaseModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});
        mValueFormatModel->setValueFilter(mViewConfig->currentValueFilter());
    }
}

void AbstractBPViewFrame::setValueFilter(const ValueFilter &filter)
{
    mViewConfig->setCurrentValueFilter(filter);
    mValueFormatModel->setValueFilter(filter);
}

void AbstractBPViewFrame::reset()
{
    setIdentifierFilter(mViewConfig->defaultIdentifierFilter());
    setValueFilter(mViewConfig->defaultValueFilter());
    setLabelFilter(mViewConfig->defaultLabelFilter());
    updateView();
}

void AbstractBPViewFrame::customMenuRequested(const QPoint &pos)
{
    if (ui->tableView->selectionModel()->selectedIndexes().empty())
        return;
    auto view = mViewConfig->view();
    if (ui->tableView->selectionModel()->selectedIndexes().first().row() < mModelInstance->symbolRowCount(view) &&
        ui->tableView->selectionModel()->selectedIndexes().first().column() < mModelInstance->symbolColumnCount(view)) {
        mSymbolAction->setEnabled(true);
    } else {
        mSymbolAction->setEnabled(false);
    }
    mSelectionMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void AbstractBPViewFrame::setIdentifierLabelFilter(const IdentifierState &state, Qt::Orientation orientation)
{
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
        mIdentifierFilterModel->setIdentifierFilter(mViewConfig->currentIdentifierFilter());
    } else {
        mViewConfig->currentIdentifierFilter()[orientation][state.SymbolIndex] = state;
    }
    updateView();
    emit filtersChanged();
}

void AbstractBPViewFrame::handleRowColumnSelection()
{
    QMap<int, Symbol*> rowSymbols, columnSymbols;
    auto indexes = ui->tableView->selectionModel()->selectedIndexes();
    int section;
    for (const auto& index : indexes) {
        if (!index.isValid())
            continue;
        section = ui->tableView->model()->headerData(index.row(), Qt::Vertical,
                                                     Mi::IndexDataRole).toInt();
        auto equation = mModelInstance->equation(section);
        rowSymbols[section] = equation;
        section = ui->tableView->model()->headerData(index.column(), Qt::Horizontal,
                                                     Mi::IndexDataRole).toInt();
        auto variable = mModelInstance->variable(section);
        columnSymbols[section] = variable;
        break;
    }
    mSelectedEquations = rowSymbols.values();
    mSelectedVariables = columnSymbols.values();
    emit newSymbolViewRequested();
}

void AbstractBPViewFrame::setIdentifierFilterCheckState(int symbolIndex,
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

BPOverviewViewFrame::BPOverviewViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractBPViewFrame(new BPOverviewTableModel, parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BPOverviewViewFrame::BPOverviewViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                         QSharedPointer<AbstractViewConfiguration> viewConfig,
                                         QWidget *parent, Qt::WindowFlags f)
    : AbstractBPViewFrame(new BPOverviewTableModel, parent, f)
{
    mModelInstance = modelInstance;
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
    mViewConfig->initialize(nullptr);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

void BPOverviewViewFrame::setValueFilter(const ValueFilter &filter)
{
    Q_UNUSED(filter);
}

ViewDataType BPOverviewViewFrame::type() const
{
    return ViewDataType::BP_Overview;
}

void BPOverviewViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void BPOverviewViewFrame::setupView()
{
    auto baseModel = new BPOverviewTableModel(mViewConfig->view(),
                                              mModelInstance,
                                              ui->tableView);
    mIdentifierFilterModel = new BPIdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(baseModel);

    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mIdentifierFilterModel);
    delete oldSelectionModel;

    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setVisible(true);
    mBaseModel = QSharedPointer<BPOverviewTableModel>(baseModel);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

BPCountViewFrame::BPCountViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractBPViewFrame(new ComprehensiveTableModel, parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BPCountViewFrame::BPCountViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                   QSharedPointer<AbstractViewConfiguration> viewConfig,
                                   QWidget *parent, Qt::WindowFlags f)
    : AbstractBPViewFrame(new ComprehensiveTableModel, parent, f)
{
    mModelInstance = modelInstance;
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *BPCountViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type(),
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

void BPCountViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mAbsFormatModel)
        mAbsFormatModel->setAbsFormat(absoluteValues);
}

void BPCountViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
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
    mValueFormatModel = new BPValueFormatTypeProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(baseModel);
    mAbsFormatModel = new AbsFormatProxyModel(ui->tableView);
    mAbsFormatModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new BPIdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mAbsFormatModel);

    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mIdentifierFilterModel);
    delete oldSelectionModel;
    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setVisible(true);
    mBaseModel = QSharedPointer<ComprehensiveTableModel>(baseModel);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

BPAverageViewFrame::BPAverageViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractBPViewFrame(new BPAverageTableModel, parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BPAverageViewFrame::BPAverageViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                                   QSharedPointer<AbstractViewConfiguration> viewConfig,
                                                   QWidget *parent, Qt::WindowFlags f)
    : AbstractBPViewFrame(new BPAverageTableModel, parent, f)
{
    mModelInstance = modelInstance;
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *BPAverageViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type(),
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

void BPAverageViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mAbsFormatModel)
        mAbsFormatModel->setAbsFormat(absoluteValues);
}

void BPAverageViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
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
    mValueFormatModel = new BPValueFormatTypeProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(baseModel);
    mAbsFormatModel = new AbsFormatProxyModel(ui->tableView);
    mAbsFormatModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new BPIdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mAbsFormatModel);

    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mIdentifierFilterModel);
    delete oldSelectionModel;
    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setVisible(true);
    mBaseModel = QSharedPointer<ComprehensiveTableModel>(baseModel);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

BPScalingViewFrame::BPScalingViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractBPViewFrame(new ComprehensiveTableModel, parent, f)
{
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type()));
}

BPScalingViewFrame::BPScalingViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                       QSharedPointer<AbstractViewConfiguration> viewConfig,
                                       QWidget *parent, Qt::WindowFlags f)
    : AbstractBPViewFrame(new ComprehensiveTableModel, parent, f)
{
    mModelInstance = modelInstance;
    mViewConfig = viewConfig;
}

AbstractTableViewFrame* BPScalingViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type(),
                                                                                                        mModelInstance));
    auto frame = new BPScalingViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void BPScalingViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mViewConfig->initialize(nullptr);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

void BPScalingViewFrame::updateView()
{
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    emit filtersChanged();
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

    auto baseModel = new ComprehensiveTableModel(mViewConfig->view(),
                                                 mModelInstance,
                                                 ui->tableView);
    mValueFormatModel = new BPValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(baseModel);
    mIdentifierFilterModel = new BPIdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mValueFormatModel);

    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mIdentifierFilterModel);
    delete oldSelectionModel;
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->verticalHeader()->setVisible(true);
    mBaseModel = QSharedPointer<ComprehensiveTableModel>(baseModel);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

}
}
}
