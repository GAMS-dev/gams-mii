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
#include "modelstatistic.h"
#include "sectiontreemodel.h"
#include "modelinstancetablemodel.h"
#include "hierarchicalheaderview.h"

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
    ui->sectionTreeView->setModel(mSectionModel);

    connect(ui->sectionTreeView, &SectionTreeView::currentItemChanged,
            this, &ModelInspector::setCurrentView);
    ui->sectionTreeView->expandAll();
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

void ModelInspector::loadModelInstance(int exitCode, QProcess::ExitStatus status)
{
    if (exitCode > 0) {
        emit newLogMessage("The GAMSProcess reported an issue. The exit code is " +
                           QString().number(exitCode));
        return;
    }

    releasePreviousModel();

    ui->statisticWidget->showStatistic(mModelInstance);
    emit newLogMessage(mModelInstance->logMessages());

    auto hHeader = new HierarchicalHeaderView(Qt::Horizontal, ui->modelInstanceView);
    ui->modelInstanceView->setHorizontalHeader(hHeader);
    ui->modelInstanceView->horizontalHeader()->setVisible(true);
    ui->modelInstanceView->horizontalHeader()->setHighlightSections(true);
    ui->modelInstanceView->horizontalHeader()->setSectionsClickable(true);

    auto vheader = new HierarchicalHeaderView(Qt::Vertical, ui->modelInstanceView);
    ui->modelInstanceView->setVerticalHeader(vheader);
    ui->modelInstanceView->verticalHeader()->setVisible(true);
    ui->modelInstanceView->verticalHeader()->setHighlightSections(true);
    ui->modelInstanceView->verticalHeader()->setSectionsClickable(true);

    ui->modelInstanceView->resizeColumnsToContents();
    ui->modelInstanceView->resizeRowsToContents();
}

void ModelInspector::releasePreviousModel()
{
    mModelInstance = QSharedPointer<ModelInstance>(new ModelInstance(mWorkspace, mScratchDir));
    mModelInstance->loadScratchData();

    auto tmpModelInstanceModel = new ModelInstanceTableModel;
    tmpModelInstanceModel->setModelInstance(mModelInstance);
    ui->modelInstanceView->setModel(tmpModelInstanceModel);
    mModelInstanceModel = QSharedPointer<ModelInstanceTableModel>(tmpModelInstanceModel);
}

void ModelInspector::setCurrentView(int index)
{
    Q_UNUSED(index);
    auto modelIndex = ui->sectionTreeView->currentIndex();
    auto* item = static_cast<ViewItem*>(modelIndex.internalPointer());
    ui->stackedWidget->setCurrentIndex(item->page());
}

}
}
}
