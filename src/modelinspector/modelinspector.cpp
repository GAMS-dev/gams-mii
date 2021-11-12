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
#include "valuefiltersettings.h"

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
    return mModelInstanceModel->searchHeaders(term, isRegEx);
}

QSharedPointer<ModelInstance> ModelInspector::modelInstance() const
{
    return mModelInstance;
}

void ModelInspector::loadModelInstance(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(status);
    if (exitCode > 0) {
        emit newLogMessage("The GAMSProcess reported an issue. The exit code is " +
                           QString().number(exitCode));
        return;
    }

    releasePreviousModel();

    ui->statisticWidget->showStatistic(mModelInstance);
    emit newLogMessage(mModelInstance->logMessages());

    auto hHeader = new HierarchicalHeaderView(Qt::Horizontal, ui->miView);
    hHeader->setModelInstance(mModelInstance);
    hHeader->setVisible(true);
    hHeader->setHighlightSections(true);
    hHeader->setSectionsClickable(true);
    hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->miView->setHorizontalHeader(hHeader);
    connect(hHeader, &HierarchicalHeaderView::filterChanged,
            this, &ModelInspector::applyHeaderLabelFilter);

    auto vHeader = new HierarchicalHeaderView(Qt::Vertical, ui->miView);
    vHeader->setModelInstance(mModelInstance);
    vHeader->setVisible(true);
    vHeader->setHighlightSections(true);
    vHeader->setSectionsClickable(true);
    vHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->miView->setVerticalHeader(vHeader);
    connect(vHeader, &HierarchicalHeaderView::filterChanged,
            this, &ModelInspector::applyHeaderLabelFilter);

    emit newLogMessage(QString("Table Row Count >> %1").arg(mModelInstance->rowCount()));
    emit newLogMessage(QString("Table Column Count >> %1").arg(mModelInstance->columnCount()));

    emit newModelInstance();
}

void ModelInspector::releasePreviousModel()
{
    mModelInstance = QSharedPointer<ModelInstance>(new ModelInstance(mWorkspace, mScratchDir));
    mModelInstance->loadScratchData();
    mModelInstance->loadSymbols();
    mModelInstance->loadMinMaxValues();

    auto modelInstanceModel = new ModelInstanceTableModel(ui->miView);
    modelInstanceModel->setModelInstance(mModelInstance);
    mValueFormatModel = new ValueFormatProxyModel(ui->miView);
    mValueFormatModel->setSettings(mModelInstance->valueFilterSettings());
    mValueFormatModel->setSourceModel(modelInstanceModel);
    ui->miView->setModel(mValueFormatModel);
    mModelInstanceModel = QSharedPointer<ModelInstanceTableModel>(modelInstanceModel);
}

void ModelInspector::processGlobalFilterUpdate()
{
    auto processUelCheckStates = [this](Qt::Orientation orientation,
                                        const QMap<QString, bool> &uels,
                                        const QList<QStandardItem*> &items){
        Q_FOREACH(auto item, items) {
            if (uels.contains(item->data(Qt::DisplayRole).toString()) &&
                item->parent()->checkState() != Qt::Unchecked) {
                auto branch = mModelInstance->setBranchState(item, uels[item->text()] ? Qt::Checked : Qt::Unchecked);
                if (orientation == Qt::Horizontal) {
                    Q_FOREACH(auto bItem, branch) {
                        ui->miView->setColumnHidden(mModelInstance->itemToIndex(orientation, bItem),
                                                    !bItem->checkState());
                    }
                } else {
                    Q_FOREACH(auto bItem, branch) {
                        ui->miView->setRowHidden(mModelInstance->itemToIndex(orientation, bItem),
                                                 !bItem->checkState());
                    }
                }
            }
        }
    };

    applyHeaderSymbolFilter();
    auto hItems = mModelInstance->horizontalUelItems();
    processUelCheckStates(Qt::Horizontal,
                          mModelInstance->uelStates(Qt::Horizontal), hItems);

    auto vItems = mModelInstance->verticalUelItems();
    processUelCheckStates(Qt::Vertical,
                          mModelInstance->uelStates(Qt::Vertical), vItems);

    mValueFormatModel->setSettings(mModelInstance->valueFilterSettings());
    emit filtersUpdated();
}

void ModelInspector::processAggregationUpdate()
{
    // TODO
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
    if (!result.isValid())
        return;
    if (result.Orientation == Qt::Horizontal) {
        ui->miView->selectColumn(result.Index);
    } else {
        ui->miView->selectRow(result.Index);
    }
}

void ModelInspector::applyHeaderLabelFilter(FilterTreeItem *root, Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal) {
        QList<FilterTreeItem*> filterItems { root };
        QList<QStandardItem*> subTree { mModelInstance->horizontalItem(root->logicalIndex()) };
        while (!filterItems.isEmpty()) {
            auto filterItem = filterItems.takeFirst();
            filterItems.append(filterItem->childs());
            auto subItem = subTree.takeFirst();
            for (int c=0; c<subItem->columnCount(); ++c)
                subTree.append(subItem->child(0, c));
            subItem->setCheckState(filterItem->checked());
            ui->miView->setColumnHidden(filterItem->logicalIndex(), !filterItem->checked());
        }
    } else {
        QList<FilterTreeItem*> filterItems { root };
        QList<QStandardItem*> subTree { mModelInstance->verticalItem(root->logicalIndex()) };
        while (!filterItems.isEmpty()) {
            auto filterItem = filterItems.takeFirst();
            filterItems.append(filterItem->childs());
            auto subItem = subTree.takeFirst();
            for (int c=0; c<subItem->columnCount(); ++c)
                subTree.append(subItem->child(0, c));
            subItem->setCheckState(filterItem->checked());
            ui->miView->setRowHidden(filterItem->logicalIndex(), !filterItem->checked());
        }
    }
    emit filtersUpdated();
}

void ModelInspector::applyHeaderSymbolFilter()
{
    Q_FOREACH(auto item, mModelInstance->horizontalSymbols()) {
        int logicaIndex = mModelInstance->horizontalIndex(item);
        if (logicaIndex < mModelInstance->predefinedHeaderLength()) {
            ui->miView->setColumnHidden(logicaIndex, !item->checkState());
            continue;
        }
        int symIndex = mModelInstance->symbolIndex(item->data(Qt::DisplayRole).toString());
        if (symIndex <= 0)
            continue;
        auto symbol = mModelInstance->symbol(symIndex);
        for (int i=logicaIndex; i<logicaIndex+symbol.Entries; ++i)
            ui->miView->setColumnHidden(i, !item->checkState());
    }
    Q_FOREACH(auto item, mModelInstance->verticalSymbols()) {
        int logicalIndex = mModelInstance->verticalIndex(item);
        if (logicalIndex < mModelInstance->predefinedHeaderLength()) {
            ui->miView->setRowHidden(logicalIndex, !item->checkState());
            continue;
        }
        int symIndex = mModelInstance->symbolIndex(item->data(Qt::DisplayRole).toString());
        if (symIndex <= 0)
            continue;
        auto symbol = mModelInstance->symbol(symIndex);
        for (int i=logicalIndex; i<logicalIndex+symbol.Entries; ++i)
            ui->miView->setRowHidden(i, !item->checkState());
    }
}

}
}
}
