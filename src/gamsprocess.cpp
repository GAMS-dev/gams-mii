/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023-2026 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023-2026 GAMS Development Corp. <support@gams.com>
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
#include "gamsprocess.h"

#ifdef _WIN32
#include "windows.h"
#endif

GAMSProcess::GAMSProcess(QObject *parent)
    : AbstractProcess("gams", parent)
{

}

void GAMSProcess::execute()
{
    if (!runable()) {
        emit runCanceled();
        return;
    }
    mProcess.setWorkingDirectory(directory());
    QStringList args { "\"" + model() + "\"" };
    args << parameters();
#if defined(__unix__) || defined(__APPLE__)
    mProcess.start(nativeAppPath(), args);
#else
    mProcess.setNativeArguments(args.join(" "));
    mProcess.setProgram(nativeAppPath());
    mProcess.start();
#endif
}
