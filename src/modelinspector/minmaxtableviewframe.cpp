#include "minmaxtableviewframe.h"
#include "ui_standardtableviewframe.h"
#include "hierarchicalheaderview.h"
#include "valueformatproxymodel.h"
#include "minmaxmodelinstancetablemodel.h"
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
    , mModelInstanceModel(new MinMaxModelInstanceTableModel)
    , mSelectionMenu(new QMenu(this))
{
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(ViewDataType::MinMax));

    setupSelectionMenu();
    connect(ui->tableView, &QWidget::customContextMenuRequested,
            this, &MinMaxTableViewFrame::customMenuRequested);
}

void MinMaxTableViewFrame::aggregate(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mViewConfig->initialize(nullptr);
    mModelInstance->aggregate(mViewConfig);
}

AbstractTableViewFrame* MinMaxTableViewFrame::clone(int view)
{
    auto frame = new MinMaxTableViewFrame;
    frame->setView(view);
    frame->setViewConfig(QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view)));
    frame->setupView(mModelInstance);
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
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    mHorizontalHeader->setViewType(ViewDataType::MinMax);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &MinMaxTableViewFrame::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(ViewDataType::MinMax);
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &MinMaxTableViewFrame::setIdentifierLabelFilter);

    auto modelInstanceModel = new MinMaxModelInstanceTableModel(ui->tableView);
    modelInstanceModel->setModelInstance(mModelInstance);
    modelInstanceModel->setView(mViewConfig->view());
    mValueFormatModel = new MinMaxValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(modelInstanceModel);
    mIdentifierFilterModel = new MinMaxIdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mValueFormatModel);

    ui->tableView->setHorizontalHeader(mHorizontalHeader);
    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mIdentifierFilterModel);
    delete oldSelectionModel;
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mModelInstanceModel = QSharedPointer<MinMaxModelInstanceTableModel>(modelInstanceModel);

    const auto& applied = mViewConfig->currentAggregation();
    mHorizontalHeader->setAppliedAggregation(applied);
    mVerticalHeader->setAppliedAggregation(applied);
    emit mModelInstanceModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
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
    if (mModelInstanceModel && mViewConfig->currentAggregation().useAbsoluteValues() != aggregation.useAbsoluteValues()) {
        mViewConfig->currentAggregation().setUseAbsoluteValues(aggregation.useAbsoluteValues());
        mModelInstance->aggregate(mViewConfig);
        emit mModelInstanceModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});
        mViewConfig->currentValueFilter().UseAbsoluteValues = aggregation.useAbsoluteValues();
        mValueFormatModel->setValueFilter(mViewConfig->currentValueFilter());
        emit filtersChanged();
    }
}

void MinMaxTableViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mModelInstanceModel && mViewConfig->currentAggregation().useAbsoluteValues() != absoluteValues) {
        mViewConfig->currentAggregation().setUseAbsoluteValues(absoluteValues);
        mModelInstance->aggregate(mViewConfig);
        emit mModelInstanceModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});
        mViewConfig->currentValueFilter().UseAbsoluteValues = absoluteValues;
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
    mSelectionMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MinMaxTableViewFrame::setupSelectionMenu()
{
    auto action = new QAction("Show selected symbols", this);
    mSelectionMenu->addAction(action);
    connect(action, &QAction::triggered, this, [this]{handleRowColumnSelection();});
}

void MinMaxTableViewFrame::handleRowColumnSelection()
{
    QMap<int, Symbol*> rowSymbols, columnSymbols;
    auto indexes = ui->tableView->selectionModel()->selectedIndexes();
    int section;
    for (const auto& index : indexes) {
        section = ui->tableView->model()->headerData(index.row(), Qt::Vertical).toInt();
        if (mViewConfig->currentAggregation().indexToEquation().contains(section)) {
            auto symbol = mViewConfig->currentAggregation().indexToEquation()[section];
            rowSymbols[symbol->firstSection()] = symbol;
        } else {
            auto symbol = mModelInstance->equation(section);
            rowSymbols[symbol->firstSection()] = symbol;
        }
        section = ui->tableView->model()->headerData(index.column(), Qt::Horizontal).toInt();
        if (mViewConfig->currentAggregation().indexToVariable().contains(section)) {
            auto symbol = mViewConfig->currentAggregation().indexToVariable()[section];
            columnSymbols[symbol->firstSection()] = symbol;
        } else {
            auto symbol = mModelInstance->variable(section);
            columnSymbols[symbol->firstSection()] = symbol;
        }
    }
    mSelectedEquations = rowSymbols.values();
    mSelectedVariables = columnSymbols.values();
    emit newSymbolViewRequested();
}

void MinMaxTableViewFrame::setIdentifierFilterCheckState(int symbolIndex, Qt::CheckState state, Qt::Orientation orientation)
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
