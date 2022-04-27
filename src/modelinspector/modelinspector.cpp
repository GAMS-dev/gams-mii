/*
 * This file is part of the GAMS Studio project.
 *
 * Copyright (c) 2017-2019 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2017-2019 GAMS Development Corp. <support@gams.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "modelinspector.h"
#include "ui_modelinspector.h"
#include "modelinstance.h"
#include "sectiontreemodel.h"
#include "modelinstancetablemodel.h"
#include "hierarchicalheaderview.h"
#include "searchresultmodel.h"
#include "filtertreeitem.h"
#include "valueformatproxymodel.h"
#include "columnrowfiltermodel.h"
#include "identifierfiltermodel.h"
#include "labelfiltermodel.h"
#include "aggregationproxymodel.h"
#include "sectiontreeitem.h"
#include "attributetablemodel.h"

#include "gclgms.h"

#include <QDir>

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

ModelInspector::ModelInspector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelInspector)
    , mSectionModel(new SectionTreeModel)
    , mModelInstance(new ModelInstance)
    , mModelInstanceModel(new ModelInstanceTableModel)
{
    ui->setupUi(this);
    mSectionModel->loadModelData();
    ui->sectionView->setModel(mSectionModel);

    connect(ui->sectionView, &SectionTreeView::currentItemChanged,
            this, &ModelInspector::setCurrentView);
    ui->sectionView->expandAll();
}

ModelInspector::~ModelInspector()
{
    delete ui;
}

QString ModelInspector::scratchDir() const
{
    return mScratchDir;
}

void ModelInspector::setScratchDir(const QString &scratchDir)
{
    mScratchDir = scratchDir;
    QDir dir(mWorkspace + "/" + mScratchDir);
    if (!dir.exists())
        dir.mkdir(dir.absolutePath());
}

QString ModelInspector::workspace() const
{
    return mWorkspace;
}

void ModelInspector::setWorkspace(const QString &workingDir)
{
    mWorkspace = workingDir;
}

QString ModelInspector::systemDirectory() const
{
    return mSystemDir;
}

void ModelInspector::setSystemDirectory(const QString &systemDir)
{
    mSystemDir = systemDir;
}

bool ModelInspector::showOutput() const
{
    return mModelInstance->useOutput();
}

void ModelInspector::setShowOutput(bool showOutput)
{
    mShowOutput = showOutput;
    mModelInstance->setUseOutput(showOutput);
}

QList<SearchResult> ModelInspector::searchHeaders(const QString &term,
                                                  bool isRegEx)
{
    if (term.isEmpty())
        return QList<SearchResult>();
    QList<SearchResult> result;
    searchHeader(term, isRegEx, Qt::Horizontal, result);
    searchHeader(term, isRegEx, Qt::Vertical, result);
    return result;
}

void ModelInspector::loadModelInstance()
{
    setupModelInstanceView();
    ui->statisticWidget->showStatistic(mModelInstance);
    emit newLogMessage(mModelInstance->logMessages());
}

void ModelInspector::reloadModelInstance()
{
    if (mModelInstance->isInitialized())
        loadModelInstance();
}

QSharedPointer<ModelInstance> ModelInspector::modelInstance() const
{
    return mModelInstance;
}

IdentifierFilter ModelInspector::identifierFilter() const
{
    if (!mIdentifierFilterModel) {
        return IdentifierFilter();
    }
    if (mIdentifierFilterModel->identifierFilter().isEmpty()) {
        mIdentifierFilterModel->identifierFilter()[Qt::Horizontal] =
                initialSymbolFilter(ui->modelInstanceView->model(), Qt::Horizontal);
        mIdentifierFilterModel->identifierFilter()[Qt::Vertical] =
                initialSymbolFilter(ui->modelInstanceView->model(), Qt::Vertical);
    }
    return mIdentifierFilterModel->identifierFilter();
}

IdentifierFilter ModelInspector::defaultIdentifierFilter()
{
    if (!mIdentifierFilterModel) {
        return IdentifierFilter();
    }
    if (mIdentifierFilterModel->defaultIdentifierFilter().isEmpty()) {
        mIdentifierFilterModel->defaultIdentifierFilter()[Qt::Horizontal] =
                initialSymbolFilter(ui->modelInstanceView->model(), Qt::Horizontal);
        mIdentifierFilterModel->defaultIdentifierFilter()[Qt::Vertical] =
                initialSymbolFilter(ui->modelInstanceView->model(), Qt::Vertical);
    }
    return mIdentifierFilterModel->defaultIdentifierFilter();
}

void ModelInspector::setIdentifierFilter(const QMap<Qt::Orientation,
                                         IdentifierStates> &filter)
{
    if (mIdentifierFilterModel) {
        mIdentifierFilterModel->setIdentifierFilter(filter);
    }
}

ValueFilter ModelInspector::valueFilter() const
{
    if (mValueFormatModel) {
        return mValueFormatModel->valueFilter();
    }
    return ValueFilter();
}

ValueFilter ModelInspector::defaultValueFilter() const
{
    return initalValueFilter();
}

void ModelInspector::setValueFilter(const ValueFilter &filter)
{
    if (mValueFormatModel) {
        mValueFormatModel->setValueFilter(filter);
    }
}

LabelFilter ModelInspector::labelFilter() const
{
    if (!mLabelFilterModel) {
        return LabelFilter();
    }
    if (mLabelFilterModel->labelFilter().LabelCheckStates.isEmpty()) {
        mLabelFilterModel->setLabelFilter(defaultLabelFilter());
    }
    return mLabelFilterModel->labelFilter();
}

LabelFilter ModelInspector::defaultLabelFilter() const
{
    return initialLabelFilter();
}

void ModelInspector::setLabelFilter(const LabelFilter &filter)
{
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

Aggregation ModelInspector::aggregation() const
{
    if (!mAggregationModel) {
        return Aggregation();
    }
    if (mAggregationModel->aggregation().aggregationMap().isEmpty()) {
        auto defaultAggr = defaultAggregation();
        auto appliedAggr = appliedAggregation(defaultAggregation());
        mHorizontalHeader->setAppliedAggregation(appliedAggr.aggregationSymbols(Qt::Horizontal));
        mVerticalHeader->setAppliedAggregation(appliedAggr.aggregationSymbols(Qt::Vertical));
        mAggregationModel->setAggregation(defaultAggr, appliedAggr);
    }
    return mAggregationModel->aggregation();
}

Aggregation ModelInspector::defaultAggregation() const
{
    return initialAggregation();
}

void ModelInspector::setAggregation(const Aggregation &aggregation)
{
    if (mAggregationModel && mHorizontalHeader && mVerticalHeader) {
        auto applied = appliedAggregation(aggregation);
        mHorizontalHeader->setAppliedAggregation(applied.aggregationSymbols(Qt::Horizontal));
        mVerticalHeader->setAppliedAggregation(applied.aggregationSymbols(Qt::Vertical));
        mAggregationModel->setAggregation(aggregation, applied);
    }
}

void ModelInspector::resetColumnRowFilter()
{
    if (mColumnRowFilterModel) {
        mColumnRowFilterModel->invalidate();
    }
}

void ModelInspector::setCurrentView(int index)
{
    Q_UNUSED(index);
    auto modelIndex = ui->sectionView->currentIndex();
    auto* item = static_cast<SectionTreeItem*>(modelIndex.internalPointer());
    if (item->page() < 0 || item->page() >= ui->stackedWidget->count())
        return;
    ui->stackedWidget->setCurrentIndex(item->page());
    emit viewChanged(item->type());
}

void ModelInspector::setSearchSelection(const gams::studio::modelinspector::SearchResult &result)
{
    if (result.Index < 0)
        return;
    if (result.Orientation == Qt::Horizontal) {
        ui->modelInstanceView->selectColumn(result.Index);
    } else {
        ui->modelInstanceView->selectRow(result.Index);
    }
}

void ModelInspector::setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                              Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        mIdentifierFilterModel->setIdentifierCheckState(state.SymbolIndex,
                                                        Qt::Unchecked,
                                                        orientation);
    } else {
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    emit filtersChanged();
}

void ModelInspector::setupModelInstanceView()
{
    mModelInstance = QSharedPointer<ModelInstance>(new ModelInstance(mWorkspace,
                                                                     mSystemDir,
                                                                     mScratchDir));
    mModelInstance->initialize();
    mModelInstance->loadScratchData(mShowOutput);
    mModelInstance->loadTableData(mInitialLabelFilter);
    mInitialValueFilter.MinValue = mModelInstance->modelMinimum();
    mInitialValueFilter.MaxValue = mModelInstance->modelMaximum();

    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->modelInstanceView);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &ModelInspector::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->modelInstanceView);
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &ModelInspector::setIdentifierLabelFilter);

    auto modelInstanceModel = new ModelInstanceTableModel(ui->modelInstanceView);
    modelInstanceModel->setModelInstance(mModelInstance);
    mValueFormatModel = new ValueFormatProxyModel(ui->modelInstanceView);
    mValueFormatModel->setValueFilter(initalValueFilter());
    mValueFormatModel->setSourceModel(modelInstanceModel);
    mLabelFilterModel = new LabelFilterModel(mModelInstance, ui->modelInstanceView);
    mLabelFilterModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new IdentifierFilterModel(mModelInstance, ui->modelInstanceView);
    mIdentifierFilterModel->setSourceModel(mLabelFilterModel);
    mIdentifierLabelFilterModel = new IdentifierLabelFilterModel(mModelInstance, ui->modelInstanceView);
    mIdentifierLabelFilterModel->setSourceModel(mIdentifierFilterModel);
    mAggregationModel = new AggregationProxyModel(mModelInstance, ui->modelInstanceView);
    mAggregationModel->setSourceModel(mIdentifierLabelFilterModel);
    mColumnRowFilterModel = new ColumnRowFilterModel(ui->modelInstanceView);
    mColumnRowFilterModel->setSourceModel(mAggregationModel);
    ui->modelInstanceView->setHorizontalHeader(mHorizontalHeader);
    ui->modelInstanceView->setVerticalHeader(mVerticalHeader);
    ui->modelInstanceView->setModel(mColumnRowFilterModel);
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    ui->eqnAttributeView->setHorizontalHeader(new HierarchicalHeaderView(Qt::Horizontal,
                                                                      mModelInstance,
                                                                      ui->eqnAttributeView));
    ui->eqnAttributeView->horizontalHeader()->setVisible(true);
    ui->eqnAttributeView->setVerticalHeader(new HierarchicalHeaderView(Qt::Vertical,
                                                                    mModelInstance,
                                                                    ui->eqnAttributeView));
    ui->eqnAttributeView->verticalHeader()->setVisible(true);
    auto eqnModel = new EquationAttributeTableModel(ui->eqnAttributeView);
    eqnModel->setModelInstance(mModelInstance);
    auto eqnFilter = new ColumnRowFilterModel(ui->eqnAttributeView);
    eqnFilter->setSourceModel(eqnModel);
    ui->eqnAttributeView->setModel(eqnFilter);

    auto hHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                              mModelInstance,
                                              ui->varAttributeView);
    hHeader->setDataSource(HierarchicalHeaderView::EquationData);
    ui->varAttributeView->setHorizontalHeader(hHeader);
    ui->varAttributeView->horizontalHeader()->setVisible(true);
    auto vHeader = new HierarchicalHeaderView(Qt::Vertical,
                                             mModelInstance,
                                             ui->varAttributeView);
    vHeader->setDataSource(HierarchicalHeaderView::VariableData);
    ui->varAttributeView->setVerticalHeader(vHeader);
    ui->varAttributeView->verticalHeader()->setVisible(true);
    auto varModel = new VariableAttributeTableModel(ui->varAttributeView);
    varModel->setModelInstance(mModelInstance);
    auto varFilter = new ColumnRowFilterModel(ui->varAttributeView);
    varFilter->setSourceModel(varModel);
    ui->varAttributeView->setModel(varFilter);

    mModelInstanceModel = QSharedPointer<ModelInstanceTableModel>(modelInstanceModel);
}

void ModelInspector::searchHeader(const QString &term, bool isRegEx,
                                  Qt::Orientation orientation,
                                  QList<SearchResult> &result)
{
    if (!mModelInstance->isInitialized()) return;
    bool ok;
    int sections = orientation == Qt::Horizontal ? ui->modelInstanceView->model()->columnCount() :
                                                   ui->modelInstanceView->model()->rowCount();
    for (int s=0; s<sections; ++s) {
        int realSection = ui->modelInstanceView->model()->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        mModelInstance->searchSymbolData(s, realSection, term, isRegEx, orientation, result);
    }
}

Aggregation ModelInspector::appliedAggregation(const Aggregation &aggregation) const
{
    AggregationMap map;
    for (auto mapIter=aggregation.aggregationMap().keyValueBegin();
         mapIter!=aggregation.aggregationMap().keyValueEnd(); ++mapIter) {
        Q_FOREACH(auto item, mapIter->second) {
            for (auto iter=item.checkStates().keyValueBegin(); iter!=item.checkStates().keyValueEnd(); ++iter) {
                if (iter->second != Qt::Unchecked) {
                    auto symbol = mapIter->first == Qt::Horizontal ? mModelInstance->variable(item.symbolIndex()) :
                                                                     mModelInstance->equation(item.symbolIndex());
                    Aggregator aggregator(symbol);
                    aggregator.applyFilterStates(identifierFilter().value(mapIter->first)[symbol.firstSection()],
                                                 mIdentifierLabelFilterModel->identifierState(symbol.firstSection(), mapIter->first),
                                                 labelFilter().LabelCheckStates.value(mapIter->first),
                                                 labelFilter().Any);
                    aggregator.aggregate(item, aggregation.typeText());
                    map[mapIter->first][item.symbolIndex()] = item;
                    break;
                }
            }
        }
    }

    Aggregation appliedAggregation;
    appliedAggregation.setUseAbsoluteValues(aggregation.useAbsoluteValues());
    appliedAggregation.setType(aggregation.type());
    appliedAggregation.setAggregationMap(map);
    appliedAggregation.setIdentifierFilter(identifierFilter());
    appliedAggregation.setIdentifierLabelFilter(mIdentifierLabelFilterModel->identifierFilter());
    appliedAggregation.setValueFilter(valueFilter());
    return appliedAggregation;
}

Aggregation ModelInspector::initialAggregation() const
{
    auto initAggregation = [this](int type) {
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
    aggregation.setAggregationSymbols(Qt::Horizontal, initAggregation(dctvarSymType));
    aggregation.setAggregationSymbols(Qt::Vertical, initAggregation(dcteqnSymType));
    return aggregation;
}

LabelFilter ModelInspector::initialLabelFilter() const
{
    return mInitialLabelFilter;
}

IdentifierStates ModelInspector::initialSymbolFilter(QAbstractItemModel *model,
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

ValueFilter ModelInspector::initalValueFilter() const
{
    return mInitialValueFilter;
}

}
}
}
