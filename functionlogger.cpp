#include "functionlogger.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>

FunctionLogger::FunctionLogger(const QString path, const QString &functionName)
    : path(path), functionName(functionName)
{
    writeLog(path, "Entering function: " + functionName);
}

FunctionLogger::~FunctionLogger()
{
    writeLog(path, "Exiting function: " + functionName);
}

void FunctionLogger::writeLog(const QString path, const QString &msg)
{
    if(!ENABLE_LOGGING)
    {
        return;
    }

    static QMutex mutex;
    QMutexLocker locker(&mutex);

    QFile logFile(path);
    if(logFile.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        out << "[" << timestamp << "] " << msg << "\n";
    }
}
