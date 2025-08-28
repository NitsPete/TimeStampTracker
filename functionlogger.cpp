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

QString FunctionLogger::getPath()
{
    QString path = "/home/admin/SharedFolder/";
    path += QString::number(QDate::currentDate().year()) + "/";

    return path;
}

QString FunctionLogger::getLoggingPath()
{
    return getPath() + "logCpp.txt";
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

    QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();
    if(!dir.exists())
    {
        if(dir.mkpath("."))
        {
            qDebug() << "Create folder: " << dir.absolutePath();
        }
        else
        {
            qDebug() << "Failed to create folder: " <<dir.absolutePath();
        }
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
