#include "minmaxtableviewframe.h"
#include "ui_standardtableviewframe.h"
#include "hierarchicalheaderview.h"
#include "valueformatproxymodel.h"
#include "comprehensivetablemodel.h"
#include "minmaxidentifierfiltermodel.h"
#include "abstractmodelinstance.h"
#include "viewconfigurationprovider.h"

#include <QAction>
#include <QMenu>

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

MinMaxTableViewFrame::MinMaxTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractTableViewFrame(parent, f)
    , mBaseModel(new ComprehensiveTableModel)
    , mSelectionMenu(new QMenu(this))
{
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Scaling));
    mSelectionMenu->addAction(mSymbolAction);
    connect(mSymbolAction, &QAction::triggered, this, [this]{handleRowColumnSelection();});
    connect(ui->tableView, &QWidget::customContextMenuRequested,
            this, &MinMaxTableViewFrame::customMenuRequested);
}

MinMaxTableViewFrame::MinMaxTableViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
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
            this, &MinMaxTableViewFrame::customMenuRequested);
}

AbstractTableViewFrame* MinMaxTableViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::BP_Scaling,
                                                                                                        mModelInstance));
    auto frame = new MinMaxTableViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void MinMaxTableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    mViewConfig->setCurrentIdentifierFilter(filter);
    if (mIdentifierFilterModel)
        mIdentifierFilterModel->setIdentifierFilter(filter,
                                                    mViewConfig->currentAggregation());
}

void MinMaxTableViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mViewConfig->initialize(nullptr);
    mModelInstance->loadData(mViewConfig);
    setupView();
}

const QList<Symbol*>& MinMaxTableViewFrame::selectedEquations() const
{
    return mSelectedEquations;
}

const QList<Symbol*>& MinMaxTableViewFrame::selectedVariables() const
{
    return mSelectedVariables;
}

void MinMaxTableViewFrame::reset()
{
    setIdentifierFilter(mViewConfig->defaultIdentifierFilter());
    setValueFilter(mViewConfig->defaultValueFilter());
    setLabelFilter(mViewConfig->defaultLabelFilter());
    updateView();
}

void MinMaxTableViewFrame::updateView()
{
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void MinMaxTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    mViewConfig->setCurrentValueFilter(filter);
    if (mViewConfig->currentAggregation().useAbsoluteValues() != mViewConfig->currentValueFilter().UseAbsoluteValues) {
        setShowAbsoluteValues(mViewConfig->currentValueFilter().UseAbsoluteValues);
    } else if (mValueFormatModel) {
        mValueFormatModel->setValueFilter(filter);
    }
}

void MinMaxTableViewFrame::setAggregation(const Aggregation &aggregation)
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

void MinMaxTableViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mBaseModel && mViewConfig->currentValueFilter().isAbsolute() != absoluteValues) {
        mViewConfig->currentAggregation().setUseAbsoluteValues(absoluteValues);
        mViewConfig->currentValueFilter().UseAbsoluteValues = absoluteValues;
        mModelInstance->loadData(mViewConfig);
        emit mBaseModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});
        mValueFormatModel->setValueFilter(mViewConfig->currentValueFilter());
    }
}

void MinMaxTableViewFrame::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
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

void MinMaxTableViewFrame::customMenuRequested(const QPoint &pos)
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

void MinMaxTableViewFrame::setupView()
{
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(ViewDataType::BP_Scaling);
    mVerticalHeader->setView(mViewConfig->view());
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &MinMaxTableViewFrame::setIdentifierLabelFilter);

    auto baseModel = new ComprehensiveTableModel(ui->tableView);
    baseModel->setModelInstance(mModelInstance);
    baseModel->setView(mViewConfig->view());
    mValueFormatModel = new MinMaxValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(baseModel);
    mIdentifierFilterModel = new MinMaxIdentifierFilterModel(mModelInstance, ui->tableView);
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

void MinMaxTableViewFrame::handleRowColumnSelection()
{
    QMap<int, Symbol*> rowSymbols, columnSymbols;
    auto indexes = ui->tableView->selectionModel()->selectedIndexes();
    int section;
    for (const auto& index : indexes) {
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
        // TODO !!! drop this aggregation stuff (indexToVariable)... not needed anymore or fix this
        //if (mViewConfig->currentAggregation().indexToVariable().contains(section)) {
        //    auto symbol = mViewConfig->currentAggregation().indexToVariable()[section];
        //    qDebug() << "A" << section << symbol->name();
        //    columnSymbols[symbol->firstSection()] = symbol;
        //} else {
            auto symbol = mModelInstance->variable(section);
            //qDebug() << "B" << section << symbol->name();
            columnSymbols[symbol->firstSection()] = symbol;
        //}
    }
    mSelectedEquations = rowSymbols.values();
    mSelectedVariables = columnSymbols.values();
    emit newSymbolViewRequested();
}

void MinMaxTableViewFrame::setIdentifierFilterCheckState(int symbolIndex,
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
