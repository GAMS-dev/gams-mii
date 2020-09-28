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

#include <QDebug>
#include <QDir>
#include <QStandardPaths>

using namespace gams::studio;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mLibProcess(new GAMSLibProcess(this))
    , mProcess(new GAMSProcess)
{
    ui->setupUi(this);
    ui->modelInspector->setWorkspace(workspace());
    connect(mProcess->process(), SIGNAL(finished(int, QProcess::ExitStatus)),
            ui->modelInspector, SLOT(showModelStatisics()));
    connect(mProcess->process(), SIGNAL(finished(int, QProcess::ExitStatus)),
            ui->modelInspector, SLOT(showBlockpic()));
    connect(ui->modelInspector, &modelinspector::ModelInspector::newLogMessage,
            this, &MainWindow::appendLogMessage);
    connect(mLibProcess, &GAMSLibProcess::newStdChannelData,
            this, &MainWindow::appendLogMessage);
    connect(mProcess.get(), &GAMSProcess::newStdChannelData,
            this, &MainWindow::appendLogMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_runButton_clicked(bool checked)
{
    Q_UNUSED(checked)
    auto path = workspace();
    QDir dir(path);
    if (!dir.mkpath(path))
        ui->logEdit->append("Error: Could not create path " + path);

    ui->modelInspector->releasePreviousModel();

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
