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

void ModelInspector::setScratchDir(const QString &scratchDir)
{
    mModelInstance->setScratchDir(scratchDir);
}

void ModelInspector::modelDataAvailable()
{
    mModelInstance->instantiate();
}

}
}
}
