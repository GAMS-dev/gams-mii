#include "gamslibprocess.h"
#include "commonpaths.h"

#include <QDebug>
#include <QDir>

GAMSLibProcess::GAMSLibProcess(QObject *parent)
    : QObject(parent),
      mAppName("gamslib")
{

}

void GAMSLibProcess::setTargetDir(const QString &targetDir)
{
    mTargetDir = targetDir;
}

QString GAMSLibProcess::targetDir() const
{
    return mTargetDir;
}

void GAMSLibProcess::setModelNumber(int modelNumber)
{
    mModelNumber = modelNumber;
}

int GAMSLibProcess::modelNumber() const
{
    return mModelNumber;
}

void GAMSLibProcess::setModelName(const QString &modelName)
{
    mModelName = modelName;
}

QString GAMSLibProcess::modelName() const
{
    return mModelName;
}

void GAMSLibProcess::execute()
{
    QStringList args;
    args << "-lib";
    args << QDir::toNativeSeparators(mGlbFile);
    args << (mModelName.isEmpty() ? QString::number(mModelNumber) : mModelName);
    args << QDir::toNativeSeparators(mTargetDir);
    qDebug() << "model name >> " << mModelName;
    qDebug() << "model dir  >> " << mTargetDir;
    mProcess.start(nativeAppPath(), args);
}

void GAMSLibProcess::setGlbFile(const QString &glbFile)
{
    mGlbFile = glbFile;
}

void GAMSLibProcess::printOutputToDebug()
{
    if (mProcess.waitForFinished()) {
        qDebug() << mProcess.readAllStandardOutput();
    }
}

QString GAMSLibProcess::nativeAppPath()
{
    QString systemDir = CommonPaths::systemDir();
    if (systemDir.isEmpty())
        return QString();
    auto appPath = QDir(systemDir).filePath(mAppName);
    return QDir::toNativeSeparators(appPath);
}
