#include "gamsprocess.h"
#include "commonpaths.h"

#include <QDir>

#ifdef _WIN32
#include "windows.h"
#endif

GAMSProcess::GAMSProcess(QObject *parent)
    : QObject(parent),
      mProcess(this),
      mAppName("gams")
{

}

void GAMSProcess::execute()
{
    mProcess.setWorkingDirectory(mWorkingDir);
#if defined(__unix__) || defined(__APPLE__)
    mProcess.start(nativeAppPath(), mParameters);
#else
    mProcess.setNativeArguments(mParameters.join(" "));
    mProcess.setProgram(nativeAppPath());
    mProcess.start();
#endif
}

void GAMSProcess::setWorkingDir(const QString &workingDir)
{
    mWorkingDir = workingDir;
}

QString GAMSProcess::workingDir() const
{
    return mWorkingDir;
}

void GAMSProcess::interrupt()
{
    QString pid = QString::number(mProcess.processId());
#ifdef _WIN32
    //IntPtr receiver;
    COPYDATASTRUCT cds;
    const char* msgText = "GAMS Message Interrupt";

    QString windowName("___GAMSMSGWINDOW___");
    windowName += pid;
    HWND receiver = FindWindowA(nullptr, windowName.toUtf8().constData());

    cds.dwData = (ULONG_PTR) 1;
    cds.lpData = (PVOID) msgText;
    cds.cbData = (DWORD) (strlen(msgText) + 1);

    SendMessageA(receiver, WM_COPYDATA, 0, (LPARAM)(LPVOID)&cds);
#else // Linux and Mac OS X
    QProcess proc;
    proc.setProgram("/bin/bash");
    QStringList args { "-c", "kill -2 " + pid};
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished(-1);
#endif
}

void GAMSProcess::stop()
{
    mProcess.kill();
}

void GAMSProcess::setParameters(const QStringList &parameters)
{
    mParameters = parameters;
}

QString GAMSProcess::nativeAppPath()
{
    QString systemDir = CommonPaths::systemDir();
    if (systemDir.isEmpty())
        return QString();
    auto appPath = QDir(systemDir).filePath(mAppName);
    return QDir::toNativeSeparators(appPath);
}
