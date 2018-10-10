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
#include "modelinstance.h"
#include "commonpaths.h"

#include <QDir>
#include <QDebug>
#include <QString>

namespace gams {
namespace studio {
namespace modelinspector {

ModelInstance::ModelInstance(const QString &workingDir)
    : mScratchDir(""),
      mWorkingDir(workingDir)
{
    char msg[GMS_SSSIZE];
    if (!gevCreateD(&mGEV,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        qDebug() << "ERROR: " << msg; // TODO(AF): execption/syslog
    if (!gmoCreateD(&mGMO,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        qDebug() << "ERROR: " << msg; // TODO(AF): execption/syslog
    if (!dctCreateD(&mDCT,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        qDebug() << "ERROR: " << msg; // TODO(AF): execption/syslog
}

ModelInstance::~ModelInstance()
{
    if (mGMO) gmoFree(&mGMO);
    if (mGEV) gevFree(&mGEV);
}

void ModelInstance::setScratchDir(const QString &scratchDir)
{
    QDir dir(mWorkingDir + "/" + scratchDir);
    if (!dir.exists())
        dir.mkdir(dir.absolutePath());
    mScratchDir = scratchDir;
}

void ModelInstance::instantiate()
{
    QString ctrlFile = mScratchDir + "/gamscntr.dat";
    if (gevInitEnvironmentLegacy(mGEV, ctrlFile.toStdString().c_str()))
        qDebug() << "ERROR: " << "Could not initialize model instance"; // TODO(AF): execption/syslog

    // Increase solve velocity... fill scratch directory only
    //int integer1 = optGetIntStr(mOPT, "Integer1"); // TODO(AF): use GAMSOption class?
    //gevSetIntOpt(mGEV, "Integer1", integer1);

    char buffer[GMS_SSSIZE];
    gmoRegisterEnvironment(mGMO, mGEV, buffer);

    if (gmoLoadDataLegacy(mGMO, buffer) != 0)
        qDebug() << "ERROR: " << "Could not load model instance: " << QString(buffer); // TODO(AF): execption/syslog

    auto rowCount = gmoM(mGMO);
    qDebug() << "row count >> " << rowCount;

    qDebug() << "absolute scratch path >> " << mScratchDir;
    qDebug() << "lala";
    qDebug() << "lala";
    qDebug() << "lala";
    qDebug() << "lala";
    qDebug() << "lala";
}

}
}
}
