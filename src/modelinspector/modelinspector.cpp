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
#include "modelinspector.h"
#include "ui_modelinspector.h"
#include "modelinstance.h"

namespace gams {
namespace studio{
namespace modelinspector {

// TODO(AF): test models
//  o indus89.gms

ModelInspector::ModelInspector(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ModelInspector),
      mModelInstance(new ModelInstance(".")) // TODO(AF): make dynamic
{
    ui->setupUi(this);
}

ModelInspector::~ModelInspector()
{
    delete ui;
}

QString ModelInspector::scratchDir() const
{
    return mModelInstance->scratchDir();
}

void ModelInspector::setScratchDir(const QString &scratchDir)
{
    mModelInstance->setScratchDir(scratchDir);
}

void ModelInspector::modelDataAvailable()
{
    mModelInstance->instantiate();

    ui->textEdit->clear();
    ui->textEdit->append("MODEL STATISTICS");
    auto statistic = mModelInstance->statistic();
    ui->textEdit->append("Row Count: " + QString::number(statistic.RowCount));
    ui->textEdit->append("Column Count: " + QString::number(statistic.ColumnCount));
    ui->textEdit->append("Largest Dimension: " + QString::number(statistic.LargestDimension));
    ui->textEdit->append("Symbol Count: " + QString::number(statistic.SymbolCount));
    ui->textEdit->append("Unique Element Count: " + QString::number(statistic.UniqueElementCount));
    ui->textEdit->append("Used Memory: " + QString::number(statistic.UsedMemory));

    ui->textEdit->append("\nSYMBOL NAMES");
    for (auto name : statistic.SymbolNames) {
        ui->textEdit->append(name);
    }

    ui->textEdit->append("\nSYMBOL DIMENSIONS");
    for (auto dimension : statistic.SymbolDimensions) {
        ui->textEdit->append(dimension);
    }

    ui->textEdit->append("\nSYMBOL DOMAIN NAMES");
    for (auto domain : statistic.SymbolDomainNames) {
        ui->textEdit->append(domain);
    }

    ui->textEdit->append("\nUNIQUE IDENTIFIERS");
    for (auto name : statistic.UniqueIdentifiers) {
        ui->textEdit->append(name);
    }
}

}
}
}
