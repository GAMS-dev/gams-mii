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
#include "modelinspector/modelinspector.h"
#include "modelinspector/searchresultmodel.h"
#include "modelinspector/globalfilterdialog.h"

#include <QDir>
#include <QStandardItem>
#include <QStandardPaths>

#include <QDebug>

using namespace gams::studio;
using gams::studio::modelinspector::ModelInspector;
using gams::studio::modelinspector::SearchResultModel;
using gams::studio::modelinspector::GlobalFilterDialog;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mLibProcess(new GAMSLibProcess(this))
    , mProcess(new GAMSProcess)
    , mGlobalFilterDialog(new GlobalFilterDialog(this))
{
    ui->setupUi(this);
    ui->modelInspector->setWorkspace(workspace());
    ui->searchResultView->setModel(new SearchResultModel(ui->searchResultView));

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
    connect(mGlobalFilterDialog, &GlobalFilterDialog::filterUpdate,
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
    delete mLibProcess;
    delete mGlobalFilterDialog;
    delete ui;
}

void MainWindow::on_runButton_clicked(bool checked)
{
    Q_UNUSED(checked)
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

void MainWindow::appendLogMessage(const QString &message)
{
    ui->logEdit->append(message);
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
    // needed for KDE to raise the dialog when minimized
    if (mGlobalFilterDialog->isMinimized())
        mGlobalFilterDialog->setWindowState(Qt::WindowMaximized);
    if (mGlobalFilterDialog->isVisible()) {
        // needed for macOS to rasise dialog when minimized
        mGlobalFilterDialog->raise();
        mGlobalFilterDialog->activateWindow();
    } else {
        mGlobalFilterDialog->show();
    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::updateMenuEntries()
{
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

QString MainWindow::workspace() const
{
    auto path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
            "/modelinspector";
    return path;
}
