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
                mModelInstance->initialSymbolFilter(ui->modelInstanceView->model(),
                                                     Qt::Horizontal);
        mIdentifierFilterModel->identifierFilter()[Qt::Vertical] =
                mModelInstance->initialSymbolFilter(ui->modelInstanceView->model(),
                                                     Qt::Vertical);
    }
    return mIdentifierFilterModel->identifierFilter();
}

IdentifierFilter ModelInspector::defaultIdentifierFilter()
{
    IdentifierFilter filter;
    if (mIdentifierFilterModel) {
        filter[Qt::Horizontal] = mModelInstance->initialSymbolFilter(ui->modelInstanceView->model(),
                                                                      Qt::Horizontal);
        filter[Qt::Vertical] = mModelInstance->initialSymbolFilter(ui->modelInstanceView->model(),
                                                                    Qt::Vertical);
    }
    return filter;
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
    return mModelInstance->initalValueFilter();
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
    if (mLabelFilterModel->labelFilter().isEmpty()) {
        mLabelFilterModel->setLabelFilter(defaultLabelFilter());
    }
    return mLabelFilterModel->labelFilter();
}

LabelFilter ModelInspector::defaultLabelFilter() const
{
    return mModelInstance->initialLabelFilter();
}

void ModelInspector::setLabelFilter(const LabelFilter &filter)
{
    if (mLabelFilterModel) {
        mLabelFilterModel->setLabelFilter(filter);
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

void ModelInspector::resetIdentifierLabelFilter()
{
    if (mHorizontalHeader)
        mHorizontalHeader->resetSymbolLabelFilters();
    if (mVerticalHeader)
        mVerticalHeader->resetSymbolLabelFilters();
    if (mIdentifierLabelFilterModel)
        mIdentifierLabelFilterModel->clearIdentifierFilter();
}

void ModelInspector::setCurrentView(int index)
{
    Q_UNUSED(index);
    auto modelIndex = ui->sectionView->currentIndex();
    auto* item = static_cast<SectionTreeItem*>(modelIndex.internalPointer());
    ui->stackedWidget->setCurrentIndex(item->page());
}

void ModelInspector::setSearchSelection(const SearchResult &result)
{
    if (result.Index < 0)
        return;
    if (result.Orientation == Qt::Horizontal) {
        ui->modelInstanceView->selectColumn(result.Index);
    } else {
        ui->modelInstanceView->selectRow(result.Index);
    }
}

void ModelInspector::printDebugStuff()
{// TODO remove
    Q_FOREACH(auto sym, mModelInstance->variables()) {
        qDebug() << sym.name() << sym.firstSection();
    }
    qDebug() << "##########";
    auto sym = mModelInstance->variable(11);
    qDebug() << sym.name() << sym.firstSection();
}

void ModelInspector::setIdentifierLabelFilter(const IdentifierState &state,
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
                                                                     mScratchDir));
    mModelInstance->loadScratchData();
    mModelInstance->loadTableData();
    mModelInstance->loadMinMaxValues();

    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                              mModelInstance,
                                              ui->modelInstanceView);
    // TODO ResizeToContents is extreamly slow... because it affecst all rows/columns...
    //      find a better way to implement it
    //hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &ModelInspector::setIdentifierLabelFilter);

    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                              mModelInstance,
                                              ui->modelInstanceView);
    // TODO ResizeToContents is extreamly slow... because it affecst all rows/columns...
    //      find a better way to implement it
    //vHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &ModelInspector::setIdentifierLabelFilter);

    auto modelInstanceModel = new ModelInstanceTableModel(ui->modelInstanceView);
    modelInstanceModel->setModelInstance(mModelInstance);
    mValueFormatModel = new ValueFormatProxyModel(ui->modelInstanceView);
    mValueFormatModel->setValueFilter(mModelInstance->initalValueFilter());
    mValueFormatModel->setSourceModel(modelInstanceModel);
    mIdentifierFilterModel = new IdentifierFilterModel(mModelInstance, ui->modelInstanceView);
    mIdentifierFilterModel->setSourceModel(mValueFormatModel);
    mLabelFilterModel = new LabelFilterModel(mModelInstance, ui->modelInstanceView);
    mLabelFilterModel->setSourceModel(mIdentifierFilterModel);
    mIdentifierLabelFilterModel = new IdentifierLabelFilterModel(mModelInstance, ui->modelInstanceView);
    mIdentifierLabelFilterModel->setSourceModel(mLabelFilterModel);
    mAggregationModel = new AggregationProxyModel(mModelInstance, ui->modelInstanceView);
    mAggregationModel->setSourceModel(mIdentifierLabelFilterModel);
    //mColumnRowFilterModel = new ColumnRowFilterModel(ui->modelInstanceView);
    //mColumnRowFilterModel->setSourceModel(mAggregationModel);

    ui->modelInstanceView->setHorizontalHeader(mHorizontalHeader);
    ui->modelInstanceView->setVerticalHeader(mVerticalHeader);
    //ui->modelInstanceView->setModel(mColumnRowFilterModel); // TODO fix broken header when aggregation is used
    ui->modelInstanceView->setModel(mAggregationModel);
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mModelInstanceModel = QSharedPointer<ModelInstanceTableModel>(modelInstanceModel);
}

void ModelInspector::searchHeader(const QString &term, bool isRegEx,
                                  Qt::Orientation orientation,
                                  QList<SearchResult> &result)
{
    bool ok;
    int sections = orientation == Qt::Horizontal ? ui->modelInstanceView->model()->columnCount() :
                                                   ui->modelInstanceView->model()->rowCount();
    for (int s=0; s<sections; ++s) {
        int realSection = ui->modelInstanceView->model()->headerData(s, orientation).toInt(&ok);
        if (!ok)
            continue;
        mModelInstance->searchSymbolData(s, realSection, term, isRegEx, orientation, result);
    }
}

Aggregation ModelInspector::appliedAggregation(const Aggregation &aggregation) const
{
    AggregationMap map;
    for (auto mapIter=aggregation.aggregationMap().keyValueBegin();
         mapIter!=aggregation.aggregationMap().keyValueEnd(); ++mapIter) {
        Q_FOREACH(auto item, mapIter->second) {
            for (auto iter=item.checkState().keyValueBegin(); iter!=item.checkState().keyValueEnd(); ++iter) {
                if (iter->second != Qt::Unchecked) {
                    auto symbol = mapIter->first == Qt::Horizontal ? mModelInstance->variable(item.symbolIndex()) :
                                                                     mModelInstance->equation(item.symbolIndex());
                    Aggregator aggregator(symbol);
                    aggregator.applyFilterStates(identifierFilter().value(mapIter->first)[symbol.firstSection()],
                                                 mIdentifierLabelFilterModel->identifierState(symbol.firstSection(), mapIter->first),
                                                 labelFilter().value(mapIter->first));
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

}
}
}
