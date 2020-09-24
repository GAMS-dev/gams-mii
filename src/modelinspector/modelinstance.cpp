/*
 * This file is part of the GAMS Studio project.
 *
 * Copyright (c) 2017-2019 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2017-2019 GAMS Development Corp. <support@gams.com>
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
#include <QVector>
#include <QVariant>

#include <cstdlib>
#include <vector>

using namespace std;

namespace gams {
namespace studio {
namespace modelinspector {

ModelInstance::ModelInstance(const QString &workspace)
    : mScratchDir("")
    , mWorkspace(QDir(workspace).absolutePath())
{
    initialize();
}

ModelInstance::ModelInstance(const ModelInstance &modelInstance)
{
    *this = modelInstance;
}

ModelInstance::~ModelInstance()
{
    if (mGMO) gmoFree(&mGMO);
    if (mGEV) gevFree(&mGEV);
    if (mDCT) dctFree(&mDCT);
}

QString ModelInstance::scratchDir() const
{
    return mScratchDir;
}

void ModelInstance::setScratchDir(const QString &scratchDir)
{
    QDir dir(mWorkspace + "/" + scratchDir);
    if (!dir.exists())
        dir.mkdir(dir.absolutePath());
    mScratchDir = scratchDir;
}

void ModelInstance::loadScratchData()
{
    mLogMessages << "Model Workspace: " + mWorkspace;
    QString ctrlFile = mWorkspace + "/" + mScratchDir + "/gamscntr.dat";
    mLogMessages << "CTRL File: " + ctrlFile;
    if (gevInitEnvironmentLegacy(mGEV, ctrlFile.toStdString().c_str())) {
        mLogMessages << "ERROR: Could not initialize model instance";
        return;
    }

    char msg[GMS_SSSIZE];
    gmoRegisterEnvironment(mGMO, mGEV, msg);
    if (gmoLoadDataLegacy(mGMO, msg)) {
        mLogMessages << "ERROR: Could not load model instance: " + QString(msg);
        return;
    }

    // TODO specifc error message that model insp. needs the dct... because it can be switched off by the user
    // TODO check it gmodict() can be used and this could be droped then...
    QString dictFile = mWorkspace + "/" + mScratchDir + "/gamsdict.dat";
    mLogMessages << "DCT File: " + dictFile;
    if (dctLoadEx(mDCT, dictFile.toStdString().c_str(), msg, sizeof(msg))) {
        mLogMessages << "ERROR: Could not load dictionary file. " + QString(msg);
        return;
    }

    // TODO usage... ask michael
    // o break equation/variable values
    //gmoObjStyleSet(mGMO, gmoObjType_Fun);
    mLogMessages << "Absolute Scrach Path: " + mScratchDir;
}

QStringList ModelInstance::symbolNames() const
{
    QStringList names;
    for (int i=1; i<=symbolCount(); ++i) {
        char name[GMS_SSSIZE];
        dctSymName(mDCT, i, name, GMS_SSSIZE);
        names << name;
    }
    return names;
}

SymbolInfo ModelInstance::symbol(int index) const
{
    SymbolInfo info;
    if (index > symbolCount())
        return info;

    info.Index = index;
    info.Dimension = dctSymDim(mDCT, index);

    char symbolName[GMS_SSSIZE];
    dctSymName(mDCT, index, symbolName, GMS_SSSIZE);
    info.Name = symbolName;

    info.Type = dctSymType(mDCT, index);

    return info;
}

ModelInstance& ModelInstance::operator=(const ModelInstance &modelInstance)
{
    mScratchDir = modelInstance.mScratchDir;
    mWorkspace = modelInstance.mWorkspace;
    this->initialize();
    return *this;
}

void ModelInstance::initialize()
{
    gevSetExitIndicator(0); // switch of lib exit() call
    gevSetScreenIndicator(0); // switch off std lib output
    gevSetErrorCallback(ModelInstance::errorCallback);

    mLogMessages << "GAMS System Dir: " + CommonPaths::systemDir();

    char msg[GMS_SSSIZE];
    if (!gevCreateD(&mGEV,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        mLogMessages << "ERROR: " + QString(msg);

    gmoSetExitIndicator(0); // switch of lib exit() call
    gmoSetScreenIndicator(0); // switch off std lib output
    gmoSetErrorCallback(ModelInstance::errorCallback);

    if (!gmoCreateD(&mGMO,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        mLogMessages << "ERROR: " + QString(msg);

    dctSetExitIndicator(0); // switch of lib exit() call
    dctSetScreenIndicator(0); // switch off std lib output
    dctSetErrorCallback(ModelInstance::errorCallback);

    if (!dctCreateD(&mDCT,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        mLogMessages << "ERROR: " + QString(msg);
}

int ModelInstance::errorCallback(int count, const char *message)
{
    Q_UNUSED(count)
    //emit newLogMessage("ERROR CALLBACK: " + QString(message));
    qDebug()<< message;
    return 0;
}

}
}
}
