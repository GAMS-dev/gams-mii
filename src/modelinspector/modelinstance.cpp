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

void gevCallback(const char *msg, int mode, void *usrmem)
{
    Q_UNUSED(usrmem);
    qDebug() << "gevCallback";
    qDebug() << "Mode >> " << QString::number(mode);
    qDebug() << msg;
}

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

    gevRegisterWriteCallback(mGEV,
                             gevCallback,
                             1 /* enable log */,
                             nullptr /* no call identifier */);
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
    //QString logFile = mScratchDir + "/gamslog.dat";
    QString ctrlFile = mScratchDir + "/gamscntr.dat";
    if (gevInitEnvironmentLegacy(mGEV, ctrlFile.toStdString().c_str()))
        qDebug() << "ERROR: " << "Could not initialize model instance"; // TODO(AF): execption/syslog

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
