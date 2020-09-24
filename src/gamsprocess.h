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
#ifndef GAMSPROCESS_H
#define GAMSPROCESS_H

#include <QMutex>
#include <QObject>
#include <QProcess>

class GAMSProcess
        : public QObject
{
    Q_OBJECT

public:
    GAMSProcess(QObject *parent = nullptr);

    void setWorkingDir(const QString &workingDir);
    QString workingDir() const;

    void execute();
    void interrupt();
    void stop();

    void setModel(const QString &model);
    void setParameters(const QStringList &parameters);

    QProcess* process();

signals:
    void newStdChannelData(const QByteArray &data);

private:
    QString nativeAppPath();

    void readStdChannel(QProcess::ProcessChannel channel);
    void readStdOut();
    void readStdErr();

private:
    QProcess mProcess;
    QString mAppName;
    QString mModel;
    QStringList mParameters;
    QString mWorkingDir;

    QMutex mOutputMutex;
};

#endif // GAMSPROCESS_H
