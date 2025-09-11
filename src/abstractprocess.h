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
#ifndef ABSTRACTPROCESS_H
#define ABSTRACTPROCESS_H

#include <QProcess>
#include <QString>
#include <QMutex>

class AbstractProcess
        : public QObject
{
    Q_OBJECT

public:
    AbstractProcess(const QString &app, QObject *parent = nullptr);

    QString application() const;
    bool runable();

    QString directory() const;
    void setDirectory(const QString &dir);

    QStringList parameters() const;
    void setParameters(const QStringList &parameters);

    QString model() const;
    void setModel(const QString &model);

    virtual void execute() = 0;

    virtual void stop();

signals:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

    void newLogMessage(const QString &msg);

    void newStdChannelData(const QByteArray &data);

    void runCanceled();

protected:
    QString nativeAppPath();


private slots:
    void readStdOut();
    void readStdErr();

private:
    void readStdChannel(QProcess::ProcessChannel channel);

protected:
    QProcess mProcess;

private:
    QString mApplication;
    QString mDirectory;
    QStringList mParameters;
    QString mModel;
    QMutex mOutputMutex;
};

#endif // ABSTRACTPROCESS_H
