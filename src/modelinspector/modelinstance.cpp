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
#include <QVector>
#include <QVariant>

#include <cstdlib>
#include <vector>

using namespace std;

namespace gams {
namespace studio {
namespace modelinspector {

static gdxStrIndex_t domNames;
static gdxStrIndexPtrs_t domPtrs;

ModelInstance::ModelInstance(const QString &workingDir)
    : mScratchDir(""),
      mWorkingDir(workingDir)
{
    gevSetExitIndicator(0); // switch of lib exit() call
    gevSetScreenIndicator(0); // switch off std lib output
    gevSetErrorCallback(ModelInstance::errorCallback);

    char msg[GMS_SSSIZE];
    if (!gevCreateD(&mGEV,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        qDebug() << "ERROR: " << msg; // TODO(AF): execption/syslog

    gmoSetExitIndicator(0); // switch of lib exit() call
    gmoSetScreenIndicator(0); // switch off std lib output
    gmoSetErrorCallback(ModelInstance::errorCallback);

    if (!gmoCreateD(&mGMO,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        qDebug() << "ERROR: " << msg; // TODO(AF): execption/syslog

    dctSetExitIndicator(0); // switch of lib exit() call
    dctSetScreenIndicator(0); // switch off std lib output
    dctSetErrorCallback(ModelInstance::errorCallback);

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
    if (mDCT) dctFree(&mDCT);
}

QString ModelInstance::scratchDir() const
{
    return mScratchDir;
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
    if (gevInitEnvironmentLegacy(mGEV, ctrlFile.toStdString().c_str())) {
        qDebug() << "ERROR: " << "Could not initialize model instance"; // TODO(AF): execption/syslog
        return;
    }

    char msg[GMS_SSSIZE];
    gmoRegisterEnvironment(mGMO, mGEV, msg);
    if (gmoLoadDataLegacy(mGMO, msg)) {
        qDebug() << "ERROR: " << "Could not load model instance: " << QString(msg); // TODO(AF): execption/syslog
        return;
    }

    QString dictFile = mScratchDir + "/gamsdict.dat";
    if (dctLoadEx(mDCT, dictFile.toStdString().c_str(), msg, sizeof(msg))) {
        qDebug() << "ERROR: Could not load dictionary file. " << QString(msg); // TODO(AF): execption/syslog
        return;
    }

    qDebug() << "absolute scratch path >> " << mScratchDir;
}

ModelStatistic ModelInstance::statistic()
{
    ModelStatistic ms;
    ms.RowCount = dctNRows(mDCT);
    ms.ColumnCount = dctNCols(mDCT);
    ms.LargestDimension = dctLrgDim(mDCT);
    ms.UniqueElementCount = dctNUels(mDCT);
    ms.SymbolCount = dctNLSyms(mDCT);
    ms.UsedMemory = dctMemUsed(mDCT);

    // get symbol names
    for (int i=1; i<=ms.SymbolCount; ++i) {
        char name[GMS_SSSIZE];
        dctSymName(mDCT, i, name, GMS_SSSIZE);
        ms.SymbolNames << name;
    }

    for (int i=1; i<=ms.SymbolCount; ++i) {
        int symbolDimension = dctSymDim(mDCT, i);
        char symbolName[GMS_SSSIZE];
        dctSymName(mDCT, i, symbolName, GMS_SSSIZE);
        ms.SymbolDimensions << QString("%1: %2").arg(symbolName).arg(symbolDimension);
    }

    ms.SymbolNames << "\nSYMBOL TYPE";
    for (int i=1; i<=ms.SymbolCount; ++i) {
        char symbolName[GMS_SSSIZE];
        dctSymName(mDCT, i, symbolName, GMS_SSSIZE);
        int symbolType = dctSymType(mDCT, i);
        switch (symbolType) {
            case dctfuncSymType:
                ms.SymbolNames << QString("Symbol %1 Type: dctfuncSymType (%2)").arg(symbolName).arg(symbolType);
                break;
            case dctsetSymType:
                ms.SymbolNames << QString("Symbol %1 Type: dctsetSymType (%2)").arg(symbolName).arg(symbolType);
                break;
            case dctacrSymType:
                ms.SymbolNames << QString("Symbol %1 Type: dctacrSymType (%2)").arg(symbolName).arg(symbolType);
                break;
            case dctparmSymType:
                ms.SymbolNames << QString("Symbol %1 Type: dctparmSymType (%2)").arg(symbolName).arg(symbolType);
                break;
            case dctvarSymType:
                ms.SymbolNames << QString("Symbol %1 Type: dctvarSymType (%2)").arg(symbolName).arg(symbolType);
                break;
            case dcteqnSymType:
                ms.SymbolNames << QString("Symbol %1 Type: dcteqnSymType (%2)").arg(symbolName).arg(symbolType);
                break;
            case dctaliasSymType:
                ms.SymbolNames << QString("Symbol %1 Type: dctaliasSymType (%2)").arg(symbolName).arg(symbolType);
                break;
            default: // dctunknownSymType
                ms.SymbolNames << QString("Symbol %1 Type: dctunknownSymType (%2)").arg(symbolName).arg(symbolType);
                break;
        }
    }

    // TODO dctSymUserInfo
    for (int i=1; i<=ms.SymbolCount; ++i) {
        auto value = dctSymOffset(mDCT, i);
        char symbolName[GMS_SSSIZE];
        dctSymName(mDCT, i, symbolName, GMS_SSSIZE);
        qDebug() << "Symbol: " << symbolName << "Value: " << value;
    }
    auto value = dctSymOffset(mDCT, 8);
    qDebug() << "Symbol: ---" << "Value: " << value;

    ms.SymbolNames << "\nSYMBOL DOMAIN INDEX";
    auto domainNameCount = dctDomNameCount(mDCT);
    ms.SymbolDomainNames << QString("Domain Name Count: %1").arg(domainNameCount);
    int idx[domainNameCount];
    for (int i=1, dimension; i<=ms.SymbolCount; ++i) {
        char symbolName[GMS_SSSIZE];
        dctSymName(mDCT, i, symbolName, GMS_SSSIZE);
        dctSymDomIdx(mDCT, i, idx, &dimension);
        QStringList idxstr;
        for (auto value : vector<int>(idx, idx+dimension)) {
            idxstr << QString::number(value);
        }
        ms.SymbolNames << QString("symbol: %1 idx: %2 length: %3")
                          .arg(symbolName)
                          .arg(idxstr.join(", "))
                          .arg(dimension);
    }


    GDXSTRINDEXPTRS_INIT (domNames, domPtrs);
    for (auto symbolName : ms.SymbolNames) {
        int symbolIndex = dctSymIndex(mDCT, symbolName.toStdString().c_str());
        if (symbolIndex <= 0) continue;
        int  symDomNamesCnt;
        dctSymDomNames(mDCT, symbolIndex, domPtrs, &symDomNamesCnt);
        QStringList domains;
        for  (int i=0; i<symDomNamesCnt; ++i) {
            domains << domPtrs[i];
        }
        if (symDomNamesCnt)
            ms.SymbolDomainNames << QString("%1: %2").arg(symbolName).arg(domains.join(", "));
        else
            ms.SymbolDomainNames << QString("%1: -").arg(symbolName);
    }

    // get UEL labels
    for (int i=1; i<=ms.UniqueElementCount; ++i) {
        char name[GMS_SSSIZE];
        char ansiChar[GMS_SSSIZE];
        dctUelLabel(mDCT, i, ansiChar, name, GMS_SSSIZE);
        ms.UniqueIdentifiers << name;
    }

//    dctColIndex
//    dctRowIndex
//    dctColUels
//    dctRowUels
//    dctFindFirstRowCol
//    dctFindNextRowCol
//    dctFindClose

//    ms.SymbolNames << "\nCOLUMN UELs";
//    for (int i=1; i<=ms.SymbolCount; ++i) {
//        int symbolDimension = dctSymDim(mDCT, i);
//        int uelIndices[symbolDimension];
//        dctColIndex(i, uelIndices);
//    }

    return ms;
}

int ModelInstance::errorCallback(int count, const char *message)
{
    Q_UNUSED(count);
    qDebug() << message;
    return 0;
}

}
}
}
