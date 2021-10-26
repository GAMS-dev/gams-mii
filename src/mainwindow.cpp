/*
 * This file is part of the GAMS Studio project.
 *
 * Copyright (c) 2017-2018 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2017-2018 GAMS Development Corp. <support@gams.com>
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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamsprocess.h"
#include "gamslibprocess.h"
#include "modelinspector/aggregationdialog.h"
#include "modelinspector/globalfilterdialog.h"
#include "modelinspector/modelinspector.h"
#include "modelinspector/searchresultmodel.h"

#include <QDir>
#include <QStandardItem>
#include <QStandardPaths>

#include <QDebug>

using namespace gams::studio;
using gams::studio::modelinspector::AggregationDialog;
using gams::studio::modelinspector::GlobalFilterDialog;
using gams::studio::modelinspector::ModelInspector;
using gams::studio::modelinspector::SearchResultModel;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mLibProcess(new GAMSLibProcess(this))
    , mProcess(new GAMSProcess(this))
    , mAggregationDialog(new AggregationDialog(this))
    , mGlobalFilterDialog(new GlobalFilterDialog(this))
    , mAggregationStatusLabel(new QLabel(QString(), this))
{
    ui->setupUi(this);
    ui->modelInspector->setWorkspace(workspace());
    ui->searchResultView->setModel(new SearchResultModel(ui->searchResultView));
    ui->statusBar->addPermanentWidget(mAggregationStatusLabel);
    mAggregationStatusLabel->setText(mAggregationDialog->statusText());

    connect(mProcess->process(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            ui->modelInspector, &ModelInspector::loadModelInstance);
    connect(ui->modelInspector, &modelinspector::ModelInspector::newLogMessage,
            this, &MainWindow::appendLogMessage);
    connect(mLibProcess, &GAMSLibProcess::newStdChannelData,
            this, &MainWindow::appendLogMessage);
    connect(mProcess.get(), &GAMSProcess::newStdChannelData,
            this, &MainWindow::appendLogMessage);
    connect(ui->searchEdit, &QLineEdit::returnPressed,
            this, &MainWindow::searchHeaders);
    connect(ui->runButton, &QPushButton::clicked,
            this, &MainWindow::on_actionRun_triggered);
    connect(mAggregationDialog, &AggregationDialog::updated,
            ui->modelInspector, &ModelInspector::processAggregationUpdate);
    connect(mAggregationDialog, &AggregationDialog::updated,
            this, [this]{ mAggregationStatusLabel->setText(mAggregationDialog->statusText()); });
    connect(mGlobalFilterDialog, &GlobalFilterDialog::updated,
            ui->modelInspector, &ModelInspector::processGlobalFilterUpdate);
    connect(ui->modelInspector, &ModelInspector::filtersUpdated,
            this, &MainWindow::handleFilterUpdate);
    connect(ui->searchResultView, &QTableView::doubleClicked,
            this, &MainWindow::searchResultSelectionChanged);
    connect(ui->modelInspector, &ModelInspector::newModelInstance,
            this, &MainWindow::updateModelInstance);
    connect(ui->menuView, &QMenu::aboutToShow,
            this, &MainWindow::updateMenuEntries);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendLogMessage(const QString &message)
{
    ui->logEdit->append(message);
}

void MainWindow::on_actionRun_triggered()
{
    auto path = workspace();
    QDir dir(path);
    if (!dir.mkpath(path))
        ui->logEdit->append("Error: Could not create path " + path);

    QStringList params = ui->paramsEdit->text().split(" ",
                                                      Qt::SkipEmptyParts,
                                                      Qt::CaseInsensitive);
    auto index = params.indexOf(QRegExp("scrdir.*"));
    if (index >= 0) {
        auto scrdir = params.at(index);
        scrdir = scrdir.replace("scrdir=", "").trimmed();
        ui->modelInspector->setScratchDir(scrdir);
    }

    if (ui->gamslibCheckBox->isChecked())
        loadGAMSModel(path);

    mProcess->setModel(ui->modelEdit->text());
    mProcess->setParameters(params);
    mProcess->setWorkingDir(path);
    mProcess->execute();
}

void MainWindow::on_action_Quit_triggered()
{
    close();
}

void MainWindow::on_action_Search_triggered()
{
    ui->searchEdit->setFocus();
}

void MainWindow::searchHeaders()
{
    auto result = ui->modelInspector->searchHeaders(ui->searchEdit->text(),
                                                    ui->regexBox->isChecked());
    static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData(result);
}

void MainWindow::on_actionGlobal_Filters_triggered()
{
    if (!mGlobalFilterDialog->isInitialized())
        return; // For some reason disabeling the action has no effect on the shortcut on macOS
    showDialog(mGlobalFilterDialog);
}

void MainWindow::on_actionAggregation_triggered()
{
    if (!mAggregationDialog->isInitialized())
        return; // For some reason disabeling the action has no effect on the shortcut on macOS
    showDialog(mAggregationDialog);
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::updateMenuEntries()
{
    ui->actionAggregation->setEnabled(mAggregationDialog->isInitialized());
    ui->actionGlobal_Filters->setEnabled(mGlobalFilterDialog->isInitialized());
}

void MainWindow::handleFilterUpdate()
{
    mGlobalFilterDialog->reloadUpdatedFilterData();
    static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
}

void MainWindow::searchResultSelectionChanged(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    auto entry = static_cast<SearchResultModel*>(ui->searchResultView->model())->entry(index.row());
    ui->modelInspector->setSearchSelection(entry);
}

void MainWindow::updateModelInstance()
{
    mAggregationDialog->setModelInstance(ui->modelInspector->modelInstance());
    mGlobalFilterDialog->setModelInstance(ui->modelInspector->modelInstance());
    static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
}

void MainWindow::loadGAMSModel(const QString &path)
{
    mLibProcess->setTargetDir(path);
    QString model = ui->modelEdit->text();
    mLibProcess->setModelName(model.replace(".gms", "").trimmed());
    mLibProcess->execute();
}

void MainWindow::showDialog(QDialog *dialog)
{
    // needed for KDE to raise the dialog when minimized
    if (dialog->isMinimized())
        dialog->setWindowState(Qt::WindowMaximized);
    if (dialog->isVisible()) {
        // needed for macOS to rasise dialog when minimized
        dialog->raise();
        dialog->activateWindow();
    } else {
        dialog->show();
    }
}

QString MainWindow::workspace() const
{
    auto path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
            "/modelinspector";
    return path;
}
