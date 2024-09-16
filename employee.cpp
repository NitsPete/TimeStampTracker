#include "employee.h"

Employee::Employee(unsigned int uniqueId, bool allowed2CheckIn, QString name, QString totalTimeToday, QString totalTimeThisSeason)
{
    this->uniqueId = uniqueId;
    this->allowed2CheckIn = allowed2CheckIn;
    this->name = name;
    this->totalTimeToday = totalTimeToday;
    this->totalTimeThisSeason = totalTimeThisSeason;

    this->bossSetsMorningTime = false;
}

void Employee::addCheckInTime(QTime checkInTime)
{
    list_checkInToday.append(checkInTime.toString("hh:mm"));
}

void Employee::addCheckOutTime(QTime addCheckOutTime)
{
    list_checkOutToday.append(addCheckOutTime.toString("hh:mm"));
}

bool Employee::employeeIsCheckedIn()
{
    // case in the morning before work
    if(list_checkInToday.isEmpty() && list_checkOutToday.isEmpty())
    {
        if(allowed2CheckIn)
        {
            return false;
        }
        return true;
    }  
    if(list_checkInToday.isEmpty())
    {
        return false;
    }
    if(list_checkOutToday.isEmpty())
    {
        return true;
    }

    QTime latestCheckInTime = QTime::fromString(list_checkInToday.last(), "hh:mm");
    QTime latestCheckOutTime = QTime::fromString(list_checkOutToday.last(), "hh:mm");

    if(latestCheckInTime < latestCheckOutTime)
    {
        return false;
    }else if(latestCheckInTime > latestCheckOutTime)
    {
        return true;
    }else // latestCheckInTime == latestCheckOutTime
    {
        if(list_checkInToday.length() == list_checkOutToday.length())
        {
            if(!allowed2CheckIn && !bossSetsMorningTime)
            {
                return true;
            }else
            {
                return false;
            }
        }

        return list_checkInToday.length() >list_checkOutToday.length();
    }

}

QString Employee::getName()
{
    return this->name;
}

QString Employee::getButtonText()
{
    // don't delete ':' after uniqueId because of MainWindow::pushButton_employee_clicked
    return QString::number(uniqueId) + ": " + name;
}

unsigned int Employee::getUniqueId()
{
    return this->uniqueId;
}

void Employee::setBossSetsMorningTime(bool bossSetsMorningTime)
{
    this->bossSetsMorningTime = bossSetsMorningTime;
}
