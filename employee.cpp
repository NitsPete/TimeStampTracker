#include "employee.h"

Employee::Employee(unsigned int uniqueId, bool allowed2CheckIn, QString name, QString totalTimeToday, QString totalTimeThisSeason)
{
    this->uniqueId = uniqueId;
    this->allowed2CheckIn = allowed2CheckIn;
    this->name = name;
    this->totalTimeToday = totalTimeToday;
    this->totalTimeThisSeason = totalTimeThisSeason;
}

QString Employee::getName()
{
    return this->name;
}

QString Employee::getButtonText()
{
    return QString::number(uniqueId) + ": " + name;
}
