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

#include <QDebug>
#include <QString>

namespace gams {
namespace studio {
namespace modelinspector {

ModelInstance::ModelInstance(const QString &workingDir)
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

//    QString scratchDir = workingDir; // model instance name needed?
//    QString logFile = workingDir + "/gamslog.dat";
//    char ctrlFile[GMS_SSSIZE];
//    if (gevDuplicateScratchDir(mGEV,
//                               scratchDir.toStdString().c_str(),
//                               logFile.toStdString().c_str(),
//                               ctrlFile))
//        qDebug() << "ERROR: " << "Problem duplicating scratch directory"; // TODO(AF): execption/syslog
//    if (gevInitEnvironmentLegacy(mGEV, ctrlFile))
//        qDebug() << "ERROR: " << "Could not initialize model instance"; // TODO(AF): execption/syslog
}

ModelInstance::~ModelInstance()
{
    if (mGMO) gmoFree(&mGMO);
    if (mGEV) gevFree(&mGEV);
}

}
}
}
