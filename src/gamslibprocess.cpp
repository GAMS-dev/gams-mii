/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023-2025 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023-2025 GAMS Development Corp. <support@gams.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "gamslibprocess.h"

GAMSLibProcess::GAMSLibProcess(QObject *parent)
    : AbstractProcess("gamslib", parent)
{

}

int GAMSLibProcess::modelNumber() const
{
    return mModelNumber;
}

void GAMSLibProcess::setModelNumber(int modelNumber)
{
    mModelNumber = modelNumber;
}

void GAMSLibProcess::execute()
{
    if (!isAppAvailable()) {
        emit runCanceled();
        return;
    }
    // TODO check for additional params
    QStringList args;
    args << (model().isEmpty() ? QString::number(mModelNumber) : model());
    args << directory();
    mProcess.start(nativeAppPath(), args);
    mProcess.waitForFinished(-1);
}
