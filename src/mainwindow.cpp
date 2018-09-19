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
#include "commonpaths.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CommonPaths::setSystemDir();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_runButton_clicked(bool checked)
{
    Q_UNUSED(checked)

    loadModel();
    GAMSProcess proc;
    QStringList params = ui->paramsEdit->text().split(" ",
                                                      QString::SkipEmptyParts,
                                                      Qt::CaseInsensitive);
    proc.setParameters(params);
    proc.setWorkingDir(".");
    proc.execute();
    proc.printOutputToDebug();
}

void MainWindow::loadModel()
{
    GAMSLibProcess proc;
    proc.setTargetDir(".");
    proc.setModelName("trnsport");
    proc.execute();
    proc.printOutputToDebug();
}
