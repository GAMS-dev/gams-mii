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
#include "symbolfiltermodel.h"
#include "uelfiltermodel.h"

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

QList<SearchResult> ModelInspector::searchHeaders(const QString &term, bool isRegEx)
{
    if (term.isEmpty())
        return QList<SearchResult>();
    QList<SearchResult> result;
    searchHeader(term, isRegEx, Qt::Horizontal, result);
    searchHeader(term, isRegEx, Qt::Vertical, result);
    return result;
}

QSharedPointer<ModelInstance> ModelInspector::modelInstance() const
{
    return mModelInstance;
}

SymbolFilterMap ModelInspector::symbolFilter()
{
    if (!mSymbolFilterModel) {
        return SymbolFilterMap();
    }
    if (mSymbolFilterModel->symbolFilter().isEmpty()) {
        mSymbolFilterModel->symbolFilter()[Qt::Horizontal] =
                mModelInstance->initialSymboldFilter(ui->modelInstanceView->model(), Qt::Horizontal);
        mSymbolFilterModel->symbolFilter()[Qt::Vertical] =
                mModelInstance->initialSymboldFilter(ui->modelInstanceView->model(), Qt::Vertical);
    }
    return mSymbolFilterModel->symbolFilter();
}

SymbolFilterMap ModelInspector::defaultSymbolFilter()
{
    SymbolFilterMap filter;
    filter[Qt::Horizontal] = mModelInstance->initialSymboldFilter(ui->modelInstanceView->model(), Qt::Horizontal);
    filter[Qt::Vertical] = mModelInstance->initialSymboldFilter(ui->modelInstanceView->model(), Qt::Vertical);
    return filter;
}

void ModelInspector::setSymbolFilter(const QMap<Qt::Orientation, SymbolFilter> &filter)
{
    if (mSymbolFilterModel) {
        mSymbolFilterModel->setSymbolFilter(filter);
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

UelFilterMap ModelInspector::uelFilter() const
{
    if (!mUelFilterModel) {
        return UelFilterMap();
    }
    if (mUelFilterModel->uelFilter().isEmpty()) {
        mUelFilterModel->setUelFilter(mModelInstance->initialUelFilter());
    }
    return mUelFilterModel->uelFilter();
}

UelFilterMap ModelInspector::defaultUelFilter() const
{
    return mModelInstance->initialUelFilter();
}

void ModelInspector::setUelFilter(const UelFilterMap &filter)
{
    if (mUelFilterModel) {
        mUelFilterModel->setUelFilter(filter);
    }
}

void ModelInspector::loadModelInstance(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(status);
    if (exitCode > 0) {
        emit newLogMessage("The GAMSProcess reported an issue. The exit code is " +
                           QString().number(exitCode));
        return;
    }

    setupModelInstanceView();
    ui->statisticWidget->showStatistic(mModelInstance);

    emit newLogMessage(mModelInstance->logMessages());
    emit newLogMessage(QString("Table Row Count >> %1").arg(mModelInstance->rowCount()));
    emit newLogMessage(QString("Table Column Count >> %1").arg(mModelInstance->columnCount()));

    emit newModelInstance();
}

void ModelInspector::setupModelInstanceView()
{
    mModelInstance = QSharedPointer<ModelInstance>(new ModelInstance(mWorkspace, mScratchDir));
    mModelInstance->loadScratchData();
    mModelInstance->loadTableData();
    mModelInstance->loadMinMaxValues();

    auto hHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                              mModelInstance,
                                              ui->modelInstanceView);
    // TODO ResizeToContents is extreamly slow... because it affecst all rows/columns...
    //      find a better way to implement it
    //hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    //connect(hHeader, &HierarchicalHeaderView::filterChanged, // TODO Symbol Icon filter
    //        this, &ModelInspector::applyHeaderLabelFilter);

    auto vHeader = new HierarchicalHeaderView(Qt::Vertical,
                                              mModelInstance,
                                              ui->modelInstanceView);
    // TODO ResizeToContents is extreamly slow... because it affecst all rows/columns...
    //      find a better way to implement it
    //vHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    //connect(vHeader, &HierarchicalHeaderView::filterChanged, // TODO Symbol Icon filter
    //        this, &ModelInspector::applyHeaderLabelFilter);

    auto modelInstanceModel = new ModelInstanceTableModel(ui->modelInstanceView);
    modelInstanceModel->setModelInstance(mModelInstance);
    mValueFormatModel = new ValueFormatProxyModel(ui->modelInstanceView);
    mValueFormatModel->setValueFilter(mModelInstance->initalValueFilter());
    mValueFormatModel->setSourceModel(modelInstanceModel);
    mSymbolFilterModel = new SymbolFilterModel(mModelInstance, ui->modelInstanceView);
    mSymbolFilterModel->setSourceModel(mValueFormatModel);
    mUelFilterModel = new UelFilterModel(mModelInstance, ui->modelInstanceView);
    mUelFilterModel->setSourceModel(mSymbolFilterModel);
    mColumnRowFilterModel = new ColumnRowFilterModel(ui->modelInstanceView);
    mColumnRowFilterModel->setSourceModel(mUelFilterModel);

    ui->modelInstanceView->setHorizontalHeader(hHeader);
    ui->modelInstanceView->setVerticalHeader(vHeader);
    ui->modelInstanceView->setModel(mColumnRowFilterModel);
    hHeader->setVisible(true);
    vHeader->setVisible(true);

    mModelInstanceModel = QSharedPointer<ModelInstanceTableModel>(modelInstanceModel);
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
{
    Q_FOREACH(auto sym, mModelInstance->variables()) {
        qDebug() << sym.Name << sym.firstSection();
    }
    qDebug() << "##########";
    auto sym = mModelInstance->variable(11);
    qDebug() << sym.Name << sym.firstSection();
}

//void ModelInspector::applyHeaderLabelFilter(FilterTreeItem *root, Qt::Orientation orientation)
//{
//    if (orientation == Qt::Horizontal) {
//        QList<FilterTreeItem*> filterItems { root };
//        QList<QStandardItem*> subTree { mModelInstance->horizontalItem(root->index()) };
//        while (!filterItems.isEmpty()) {
//            auto filterItem = filterItems.takeFirst();
//            filterItems.append(filterItem->childs());
//            auto subItem = subTree.takeFirst();
//            for (int c=0; c<subItem->columnCount(); ++c)
//                subTree.append(subItem->child(0, c));
//            subItem->setCheckState(filterItem->checked());
//            ui->modelInstanceView->setColumnHidden(filterItem->index(), !filterItem->checked());
//        }
//    } else {
//        QList<FilterTreeItem*> filterItems { root };
//        QList<QStandardItem*> subTree { mModelInstance->verticalItem(root->index()) };
//        while (!filterItems.isEmpty()) {
//            auto filterItem = filterItems.takeFirst();
//            filterItems.append(filterItem->childs());
//            auto subItem = subTree.takeFirst();
//            for (int c=0; c<subItem->columnCount(); ++c)
//                subTree.append(subItem->child(0, c));
//            subItem->setCheckState(filterItem->checked());
//            ui->modelInstanceView->setRowHidden(filterItem->index(), !filterItem->checked());
//        }
//    }
//    emit filtersUpdated();
//}

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

}
}
}
