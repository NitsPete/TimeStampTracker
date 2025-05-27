#ifndef FUNCTIONLOGGER_H
#define FUNCTIONLOGGER_H

#include <QString>

// Enable/Disable logging
const bool ENABLE_LOGGING = true;

const QString LOGGING_PATH = "/home/admin/SharedFolder//logCpp.txt";

// Add this define to start of a function to log entry and exit
#define LOG_FUNCTION() FunctionLogger functionLogger(LOGGING_PATH, Q_FUNC_INFO);

class FunctionLogger
{
public:
    explicit FunctionLogger(const QString path, const QString &functionName);
    static void writeLog(const QString path, const QString &msg);

    ~FunctionLogger();

private:
    QString path;
    QString functionName;
};

#endif // FUNCTIONLOGGER_H
