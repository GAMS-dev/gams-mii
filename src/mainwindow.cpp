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
#include "commonpaths.h"
#include "gamsprocess.h"
#include "gamslibprocess.h"
#include "modelinspector/aggregationdialog.h"
#include "modelinspector/filterdialog.h"
#include "modelinspector/modelinspector.h"
#include "modelinspector/searchresultmodel.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include <QDebug>

using namespace gams::studio;
using gams::studio::modelinspector::AggregationDialog;
using gams::studio::modelinspector::FilterDialog;
using gams::studio::modelinspector::ModelInspector;
using gams::studio::modelinspector::SearchResultModel;
using gams::studio::modelinspector::ViewDataType;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mLibProcess(new GAMSLibProcess(this))
    , mProcess(new GAMSProcess(this))
    , mAggregationDialog(new AggregationDialog(this))
    , mFilterDialog(new FilterDialog(this))
    , mAggregationStatusLabel(new QLabel(QString(), this))
{
    ui->setupUi(this);
    ui->modelInspector->setWorkspace(workspace());
    ui->modelInspector->setSystemDirectory(CommonPaths::systemDir());
    ui->searchResultView->setModel(new SearchResultModel(ui->searchResultView));
    ui->statusBar->addPermanentWidget(mAggregationStatusLabel);
    ui->actionAggregation->setEnabled(false);
    setWindowTitle(windowTitle() + " " + QApplication::applicationVersion());
    mAggregationStatusLabel->setText(mAggregationDialog->aggregation().typeText());
    setupConnections();
    setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    createProjectDirectory();

    ui->actionZoom_In->setShortcut(QKeySequence::ZoomIn);
    ui->actionZoom_Out->setShortcut(QKeySequence::ZoomOut);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendLogMessage(const QString &message)
{
    if (message.contains("ERROR:", Qt::CaseSensitive)) {
        ui->logEdit->setTextColor(QColor("red"));
        ui->logEdit->append(message);
    }
    ui->logEdit->append(message);
}

void MainWindow::on_actionOpen_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open Model File"),
                                                 workspace(),
                                                 tr("GAMS source (*.gms *.dmp *.dat)"));
    if (fileName.isEmpty())
        return;
    ui->modelEdit->setText(fileName);
    ui->gamslibCheckBox->setChecked(false);
    QFileInfo fi(ui->modelEdit->text());
    ui->logEdit->append("Loading scatch data from: " + fi.dir().path());
}

void MainWindow::on_actionOpen_Project_triggered()
{
    QMessageBox::information(this, "T.B.D.", "T.B.D.");
}

void MainWindow::on_actionRun_triggered()
{
    auto path = workspace();
    QDir dir(path);
    if (!dir.mkpath(path))
        ui->logEdit->append("Error: Could not create workspace " + path);

    if (ui->modelEdit->text().endsWith(".dat")) {
        QFileInfo fi(ui->modelEdit->text());
        auto dir = fi.dir().dirName();
        ui->modelInspector->setWorkspace(fi.dir().path()+"/..");
        ui->modelInspector->setScratchDir(dir);
        loadModelInstance(0, QProcess::NormalExit);
    } else {
        QStringList params = ui->paramsEdit->text().split(" ",
                                                          Qt::SkipEmptyParts,
                                                          Qt::CaseInsensitive);
        auto index = params.indexOf(QRegularExpression("scrdir.*"));
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
    ui->searchResultView->resizeColumnsToContents();
    ui->searchResultView->resizeRowsToContents();
}

void MainWindow::editMenuAboutToShow()
{
    auto states = ui->modelInspector->viewActionStates();
    ui->actionSaveView->setEnabled(states.SaveEnabled);
    ui->actionRemoveView->setEnabled(states.RemoveEnabled);
    ui->actionRenameView->setEnabled(states.RenameEnabled);
}

void MainWindow::on_actionFilters_triggered()
{
    setGlobalFiltersData();
    showDialog(mFilterDialog);
}

void MainWindow::on_actionAggregation_triggered()
{
    setAggregationData();
    showDialog(mAggregationDialog);
}

void MainWindow::on_actionReset_default_views_triggered()
{
    ui->modelInspector->resetDefaultViews();
    ui->searchResultView->resetZoom();
    ui->logEdit->resetZoom();
}

void MainWindow::on_actionShow_search_result_triggered()
{
    ui->dockWidget->show();
}

void MainWindow::showAbsoluteValues()
{
    ui->modelInspector->setShowAbsoluteValues(ui->actionShow_Absolute->isChecked());
    if (!ui->actionShow_Absolute->isChecked()) {
        auto defaultFilter = ui->modelInspector->defaultValueFilter();
        auto currentFilter = ui->modelInspector->valueFilter();
        currentFilter.MinValue = defaultFilter.MinValue;
        currentFilter.MaxValue = defaultFilter.MaxValue;
        ui->modelInspector->setValueFilter(currentFilter);
    }
    setGlobalFiltersData();
    setAggregationData();
}

void MainWindow::on_actionShow_Output_triggered()
{
    ui->modelInspector->setShowOutput(ui->actionShow_Output->isChecked());
    ui->modelInspector->reloadModelInstance();
    ui->modelInspector->setShowAbsoluteValues(ui->actionShow_Absolute->isChecked());
    setGlobalFiltersData();
    setAggregationData();
}

void MainWindow::on_actionZoom_In_triggered()
{
    auto widget = qApp->focusWidget();
    if (widget == ui->logEdit) {
        ui->logEdit->zoomIn(2);
    } else if (widget == ui->searchResultView) {
        ui->searchResultView->zoomIn(2);
    } else {
        ui->modelInspector->zoomIn();
    }
}

void MainWindow::on_actionZoom_Out_triggered()
{
    auto widget = qApp->focusWidget();
    if (widget == ui->logEdit) {
        ui->logEdit->zoomOut(2);
    } else if (widget == ui->searchResultView) {
        ui->searchResultView->zoomOut(2);
    } else {
        ui->modelInspector->zoomOut();
    }
}

void MainWindow::on_action_Actual_Size_triggered()
{
    auto widget = qApp->focusWidget();
    if (widget == ui->logEdit) {
        ui->logEdit->resetZoom();
    } else if (widget == ui->searchResultView) {
        ui->searchResultView->resetZoom();
    } else {
        ui->modelInspector->resetZoom();
    }
}

void MainWindow::on_actionAbout_Model_Inspector_triggered()
{
    QMessageBox about(this);
    about.setWindowTitle(tr("GAMS Model Inspector"));
    about.setTextFormat(Qt::RichText);
    about.setText("<b><big>GAMS Model Inspector " + QApplication::applicationVersion() + "</big></b>");
    about.setInformativeText(aboutModelInspector());
    about.setIconPixmap(QIcon(":/img/gams-w24").pixmap(QSize(64, 64)));
    about.addButton(QMessageBox::Ok);
    about.exec();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::loadModelInstance(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);
    if (exitCode > 0) {
        appendLogMessage("The GAMSProcess reported an issue. The exit code is " +
                         QString().number(exitCode));
        return;
    }

    ui->modelInspector->loadModelInstance();
    setGlobalFiltersData();
    setAggregationData();
    emit ui->actionShow_Absolute->triggered();
}

void MainWindow::aggregationUpdate()
{
    static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
    mAggregationStatusLabel->setText(mAggregationDialog->aggregation().typeText());
    ui->modelInspector->setAggregation(mAggregationDialog->aggregation());
}

void MainWindow::globalFilterUpdate()
{
    static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
    ui->modelInspector->setIdentifierFilter(mFilterDialog->idendifierFilter());
    ui->modelInspector->setValueFilter(mFilterDialog->valueFilter());
    ui->modelInspector->setLabelFilter(mFilterDialog->labelFilter());
    ui->modelInspector->resetColumnRowFilter();
    setAggregationData();
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
    static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
}

void MainWindow::viewChanged(int viewType)
{
    if (viewType == (int)ViewDataType::Statistic || viewType == (int)ViewDataType::Unknown) {
        ui->action_Search->setEnabled(false);
        ui->searchEdit->setEnabled(false);
        ui->actionFilters->setEnabled(false);
        //ui->actionAggregation->setEnabled(false);
    } else { // TODO !!! activate and fix crashes
        ui->action_Search->setEnabled(true);
        ui->searchEdit->setEnabled(true);
        ui->actionFilters->setEnabled(true);
        //ui->actionAggregation->setEnabled(false);
        static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
        setGlobalFiltersData();
        //setAggregationData();
        //mAggregationStatusLabel->setText(mAggregationDialog->aggregation().typeText());
        //ui->modelInspector->setShowAbsoluteValues(ui->actionShow_Absolute->isChecked());
    }
}

void MainWindow::setupConnections()
{
    connect(mProcess->process(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::loadModelInstance);
    connect(ui->modelInspector, &ModelInspector::newLogMessage,
            this, &MainWindow::appendLogMessage);
    connect(mLibProcess, &GAMSLibProcess::newStdChannelData,
            this, &MainWindow::appendLogMessage);
    connect(mProcess.get(), &GAMSProcess::newStdChannelData,
            this, &MainWindow::appendLogMessage);
    connect(ui->searchEdit, &QLineEdit::returnPressed,
            this, &MainWindow::searchHeaders);
    connect(ui->openButton, &QPushButton::clicked,
            this, &MainWindow::on_actionOpen_triggered);
    connect(ui->runButton, &QPushButton::clicked,
            this, &MainWindow::on_actionRun_triggered);
    connect(mAggregationDialog, &AggregationDialog::aggregationUpdated,
             this, &MainWindow::aggregationUpdate);
    connect(mFilterDialog, &FilterDialog::filterUpdated,
            this, &MainWindow::globalFilterUpdate);
    connect(ui->modelInspector, &ModelInspector::viewChanged,
            this, &MainWindow::viewChanged);
    connect(ui->modelInspector, &ModelInspector::filtersChanged,
            this, [this]{
        static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
        setGlobalFiltersData();
        setAggregationData();
    });
    connect(ui->searchResultView, &QTableView::doubleClicked,
            this, &MainWindow::searchResultSelectionChanged);
    connect(ui->actionSaveView, &QAction::triggered,
            ui->modelInspector, &ModelInspector::saveModelView);
    connect(ui->actionRemoveView, &QAction::triggered,
            ui->modelInspector, &ModelInspector::removeModelView);
    connect(ui->menu_Edit, &QMenu::aboutToShow,
            this, &MainWindow::editMenuAboutToShow);
    connect(ui->actionShow_Absolute, &QAction::triggered,
            this, &MainWindow::showAbsoluteValues);
}

void MainWindow::createProjectDirectory()
{
    auto path = projectDirectory();
    QDir dir(path);
    if (!dir.mkpath(path))
        ui->logEdit->append("Error: Could not create project directory " + path);
}

QString MainWindow::aboutModelInspector() const
{
    QString about = "Release: GAMS Model Inspector " + QApplication::applicationVersion() + " ";
    about += QString(sizeof(void*)==8 ? "64" : "32") + " bit<br/>";
    about += "Build Date: " __DATE__ " " __TIME__ "<br/><br/>";
    about += "Copyright (c) 2017-2022 GAMS Software GmbH <support@gams.com><br/>";
    about += "Copyright (c) 2017-2022 GAMS Development Corp. <support@gams.com><br/><br/>";
    about += "This program is free software: you can redistribute it and/or modify ";
    about += "it under the terms of the GNU General Public License as published by ";
    about += "the Free Software Foundation, either version 3 of the License, or ";
    about += "(at your option) any later version.<br/><br/>";
    about += "This program is distributed in the hope that it will be useful, ";
    about += "but WITHOUT ANY WARRANTY; without even the implied warranty of ";
    about += "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the ";
    about += "GNU General Public License for more details.<br/><br/>";
    about += "You should have received a copy of the GNU General Public License ";
    about += "along with this program. If not, see ";
    about += "<a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a>.<br/><br/>";
    about += "The source code of the program can be accessed at <b>T.B.D.</b>";
    //about += "<a href=\"https://github.com/GAMS-dev/studio\">https://github.com/GAMS-dev/studio/</a></p>.";
    return about;
}

void MainWindow::loadGAMSModel(const QString &path)
{
    mLibProcess->setTargetDir(path);
    QString model = ui->modelEdit->text();
    mLibProcess->setModelName(model.replace(".gms", "").trimmed());
    mLibProcess->execute();
}

void MainWindow::setGlobalFiltersData()
{
    mFilterDialog->setViewType(ui->modelInspector->viewType());
    auto filter = ui->modelInspector->valueFilter();
    filter.UseAbsoluteValuesGlobal = ui->actionShow_Absolute->isChecked();
    mFilterDialog->setDefaultValueFilter(ui->modelInspector->defaultValueFilter());
    mFilterDialog->setValueFilter(filter);
    mFilterDialog->setDefaultIdentifierFilter(ui->modelInspector->defaultIdentifierFilter());
    mFilterDialog->setIdentifierFilter(ui->modelInspector->identifierFilter());
    mFilterDialog->setDefaultLabelFilter(ui->modelInspector->defaultLabelFilter());
    mFilterDialog->setLabelFilter(ui->modelInspector->labelFilter());
}

void MainWindow::setAggregationData()
{
    mAggregationDialog->setAggregation(ui->modelInspector->aggregation(),
                                       ui->modelInspector->identifierFilter(),
                                       ui->actionShow_Absolute->isChecked());
    mAggregationDialog->setDefaultAggregation(ui->modelInspector->defaultAggregation());
    mAggregationStatusLabel->setText(mAggregationDialog->aggregation().typeText());
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
            "/GAMS/ModelInspector/workspace";
    return path;
}

QString MainWindow::projectDirectory() const
{
    auto path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
            "/GAMS/ModelInspector/projects";
    return path;
}
