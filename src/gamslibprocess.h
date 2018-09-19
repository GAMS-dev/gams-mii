#ifndef GAMSLIBPROCESS_H
#define GAMSLIBPROCESS_H

#include <QObject>
#include <QProcess>
#include <QString>

class GAMSLibProcess
        : public QObject
{
    Q_OBJECT

public:
    GAMSLibProcess(QObject *parent = Q_NULLPTR);

    void setTargetDir(const QString &targetDir);
    QString targetDir() const;

    void setModelNumber(int modelNumber);
    int modelNumber() const;

    void setModelName(const QString &modelName);
    QString modelName() const;

    void execute();

    void setGlbFile(const QString &glbFile);

    void printOutputToDebug();

private:
    QString nativeAppPath();

private:
    QProcess mProcess;
    QString mAppName;
    QString mTargetDir;
    int mModelNumber = -1;
    QString mModelName;
    QString mGlbFile;
};

#endif // GAMSLIBPROCESS_H
