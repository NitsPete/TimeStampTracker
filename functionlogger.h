#ifndef FUNCTIONLOGGER_H
#define FUNCTIONLOGGER_H

#include <QFileInfo>
#include <QDir>
#include <QString>

#include <QDebug>

// Enable/Disable logging
const bool ENABLE_LOGGING = true;

// Add this define to start of a function to log entry and exit
#define LOG_FUNCTION() FunctionLogger functionLogger(FunctionLogger::getLoggingPath(), Q_FUNC_INFO);

class FunctionLogger
{
public:
    explicit FunctionLogger(const QString path, const QString &functionName);
    static QString getPath();
    static QString getLoggingPath();
    static void writeLog(const QString path, const QString &msg);

    ~FunctionLogger();

private:
    QString path;
    QString functionName;
};

#endif // FUNCTIONLOGGER_H
