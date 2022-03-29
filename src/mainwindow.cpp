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
#include <QFileDialog>
#include <QMessageBox>
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
    setWindowTitle(windowTitle() + " " + QApplication::applicationVersion());
    mAggregationStatusLabel->setText(mAggregationDialog->aggregation().typeText());

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
    connect(mGlobalFilterDialog, &GlobalFilterDialog::filterUpdated,
            this, &MainWindow::globalFilterUpdate);
    connect(ui->modelInspector, &ModelInspector::filtersChanged,
            this, [this]{
        static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
        setGlobalFiltersData();
        //ui->modelInspector->setAggregation(ui->modelInspector->defaultAggregation());
        //setAggregationData(); // TODO filter states reset
    });
    connect(ui->searchResultView, &QTableView::doubleClicked,
            this, &MainWindow::searchResultSelectionChanged);

    setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendLogMessage(const QString &message)
{
    ui->logEdit->append(message);
}

void MainWindow::on_actionOpen_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open Model File"),
                                                 workspace(),
                                                 tr("GAMS source (*.gms *.dmp)"));
    if (fileName.isEmpty())
        return;
    ui->modelEdit->setText(fileName);
    ui->gamslibCheckBox->setChecked(false);
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
    setGlobalFiltersData();
    showDialog(mGlobalFilterDialog);
}

void MainWindow::on_actionAggregation_triggered()
{
    setAggregationData();
    showDialog(mAggregationDialog);
}

void MainWindow::on_actionShow_search_result_triggered()
{
    ui->dockWidget->show();
}

void MainWindow::on_actionPrint_DBG_Stuff_triggered()
{
    ui->modelInspector->printDebugStuff();
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
}

void MainWindow::aggregationUpdate()
{
    static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
    mAggregationStatusLabel->setText(mAggregationDialog->aggregation().typeText());
    ui->modelInspector->setAggregation(mAggregationDialog->aggregation());
}

void MainWindow::globalFilterUpdate()
{
    ui->modelInspector->resetIdentifierLabelFilter();
    static_cast<SearchResultModel*>(ui->searchResultView->model())->updateData({});
    ui->modelInspector->setAggregation(ui->modelInspector->defaultAggregation());
    ui->modelInspector->setIdentifierFilter(mGlobalFilterDialog->idendifierFilter());
    ui->modelInspector->setValueFilter(mGlobalFilterDialog->valueFilter());
    ui->modelInspector->setLabelFilter(mGlobalFilterDialog->labelFilter());
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
    mGlobalFilterDialog->setValueFilter(ui->modelInspector->valueFilter());
    mGlobalFilterDialog->setDefaultValueFilter(ui->modelInspector->defaultValueFilter());
    mGlobalFilterDialog->setIdentifierFilter(ui->modelInspector->identifierFilter());
    mGlobalFilterDialog->setDefaultIdentifierFilter(ui->modelInspector->defaultIdentifierFilter());
    mGlobalFilterDialog->setLabelFilter(ui->modelInspector->labelFilter());
    mGlobalFilterDialog->setDefaultLabelFilter(ui->modelInspector->defaultLabelFilter());
}

void MainWindow::setAggregationData()
{
    mAggregationDialog->setAggregation(ui->modelInspector->aggregation(),
                                       ui->modelInspector->identifierFilter());
    mAggregationDialog->setDefaultAggregation(ui->modelInspector->defaultAggregation());
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
