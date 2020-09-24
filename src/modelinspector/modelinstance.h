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
#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "gevmcc.h"
#include "gmomcc.h"
#include "dctmcc.h"

#include <QString>
#include <QStringList>

namespace gams {
namespace studio {
namespace modelinspector {

struct SymbolInfo
{
    int Index = -1;
    QString Name;
    int Dimension = -1;
    int Type = -1;

    //QString type()
    //{
    //    switch (Type) {
    //        case dctfuncSymType:
    //            return "dctfuncSymType";
    //        case dctsetSymType:
    //            return "dctsetSymType";
    //        case dctacrSymType:
    //            return "dctacrSymType";
    //        case dctparmSymType:
    //            return "dctparmSymType";
    //        case dctvarSymType:
    //            return "dctvarSymType";
    //        case dcteqnSymType:
    //            return "dcteqnSymType";
    //        case dctaliasSymType:
    //            return "dctaliasSymType";
    //        default: // dctunknownSymType
    //            return "dctunknownSymType";
    //    }
    //}
};

// TODO
// o do constant values like INF
// o use dtoaLoc library for value formatting
class ModelInstance
{
public:
    ModelInstance(const QString &workingDir);
    ModelInstance(const ModelInstance &modelInstance); // todo needed?
    ~ModelInstance();

    gevHandle_t gev() const {
        return mGEV;
    }

    gmoHandle_t gmo() const {
        return mGMO;
    }

    dctHandle_t dct() const {
        return mDCT;
    }

    QString modelName() const {
        char name[GMS_SSSIZE];
        gmoNameModel(mGMO, name);
        return name;
    }

    bool isEquation(int symType) const {
        return symType == dcteqnSymType ? true : false;
    }

    bool isVariable(int symType) const {
        return symType == dctvarSymType ? true : false;
    }

    QString scratchDir() const;
    void setScratchDir(const QString &scratchDir);


    int symbolCount() const {
        return dctNLSyms(mDCT);
    }

    QString logMessages() {
        auto messages = mLogMessages.join("\n");
        mLogMessages.clear();
        return messages;
    }

    QStringList symbolNames() const;

    SymbolInfo symbol(int index) const;

    void loadScratchData();

    ModelInstance& operator=(const ModelInstance &modelInstance);

private:
    void initialize();
    static int errorCallback(int count, const char *message);

private:
    QString mScratchDir;
    QString mWorkspace;

    gevHandle_t mGEV = nullptr;
    gmoHandle_t mGMO = nullptr;
    dctHandle_t mDCT = nullptr;

    QStringList mLogMessages;
};

}
}
}

#endif // MODELINSTANCE_H
