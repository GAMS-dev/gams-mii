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
#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "gevmcc.h"
#include "gmomcc.h"
#include "dctmcc.h"

#include <QString>

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance
{
public:
    ModelInstance(const QString &workingDir);
    ~ModelInstance();

    void setScratchDir(const QString &scratchDir);

    void instantiate();

private:
    QString mScratchDir;
    QString mWorkingDir;

    gevHandle_t mGEV = nullptr;
    gmoHandle_t mGMO = nullptr;
};

}
}
}

#endif // MODELINSTANCE_H
