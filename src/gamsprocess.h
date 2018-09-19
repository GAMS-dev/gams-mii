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

    void setParameters(const QStringList &parameters);

    void printOutputToDebug();

private:
    QString nativeAppPath();

private:
    QProcess mProcess;
    QString mAppName;
    QStringList mParameters;
    QString mWorkingDir;
};

#endif // GAMSPROCESS_H
