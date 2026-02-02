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
#include "abstractprocess.h"
#include "commonpaths.h"

#include <QDir>
#include <QStandardPaths>

AbstractProcess::AbstractProcess(const QString &app, QObject *parent)
    : QObject(parent)
    , mProcess(this)
    , mApplication(app)
{
    connect(&mProcess, &QProcess::readyReadStandardOutput,
            this, &AbstractProcess::readStdOut);
    connect(&mProcess, &QProcess::readyReadStandardError,
            this, &AbstractProcess::readStdErr);
    connect(&mProcess, &QProcess::finished,
            this, &AbstractProcess::finished);
}

QString AbstractProcess::application() const
{
    return mApplication;
}

QString AbstractProcess::directory() const
{
    return mDirectory;
}

void AbstractProcess::setDirectory(const QString &dir)
{
    mDirectory = QDir::toNativeSeparators(dir);
}

QStringList AbstractProcess::parameters() const
{
    return mParameters;
}

void AbstractProcess::setParameters(const QStringList &parameters)
{
    mParameters = parameters;
}

QString AbstractProcess::model() const
{
    return mModel;
}

void AbstractProcess::setModel(const QString &model)
{
    mModel = model;
}

void AbstractProcess::stop()
{
    mProcess.kill();
}

QString AbstractProcess::nativeAppPath()
{
    const QString& systemDir = CommonPaths::systemDir();
    if (systemDir.isEmpty())
        return QString();
    auto appPath = QStandardPaths::findExecutable(application(), { systemDir });
    return QDir::toNativeSeparators(appPath);
}

bool AbstractProcess::runable()
{
    auto app = nativeAppPath();
    if (!app.isEmpty() && QFileInfo::exists(app))
        return true;
    QString msg = QString("ERROR: The %1 executable could not be found.").arg(application());
    emit newLogMessage(msg);
    return false;
}

void AbstractProcess::readStdOut()
{
    readStdChannel(QProcess::StandardOutput);
}

void AbstractProcess::readStdErr()
{
    readStdChannel(QProcess::StandardError);
}

void AbstractProcess::readStdChannel(QProcess::ProcessChannel channel)
{
    mOutputMutex.lock();
    mProcess.setReadChannel(channel);
    bool avail = mProcess.bytesAvailable();
    mOutputMutex.unlock();

    while (avail) {
        mOutputMutex.lock();
        mProcess.setReadChannel(channel);
        emit newStdChannelData(mProcess.readLine().constData());
        avail = mProcess.bytesAvailable();
        mOutputMutex.unlock();
    }
}
