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

private:
    QString nativeAppPath();

private:
    QProcess mProcess;
    //QMutex mOutputMutex;
    QString mAppName;
    //QString mInputFile;
    QStringList mParameters;
    QString mWorkingDir;
};

#endif // GAMSPROCESS_H