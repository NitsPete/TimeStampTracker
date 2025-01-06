#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QStringList>
#include <QTime>

class Employee
{
public:
    Employee(unsigned int uniqueId, bool allowed2CheckIn, QString name, QString totalTimeToday, QString totalTimeSeason);

    void addCheckInTime(QTime checkInTime); // If you want to also update the excel interface uses ExcelInterface::addCheckInTime instead
    void addCheckOutTime(QTime addCheckOutTime); // If you want to also update the excel interface uses ExcelInterface::addCheckOutTime instead

    bool employeeIsCheckedIn();

    bool getAllowed2CheckIn();
    bool getBossSetsMorningTime();
    QString getName();
    QString getButtonText(); // <uniqueId>: <name>
    QString getTotalTimeToday();
    QString getTotalTimeSeason();
    QStringList getList_checkInToday();
    QStringList getList_checkOutToday();
    unsigned int getUniqueId();

    void setBossSetsMorningTime(bool bossSetsMorningTime);
    void setTotalTimeToday(QString totalTimeToday);
    void setTotalTimeSeason(QString totalTimeSeason);

private:
    unsigned int uniqueId;
    bool allowed2CheckIn; // Only some employee are allowed to self check in the first time in the morning
    bool bossSetsMorningTime; // If chef have already set morning time for none allowed2CheckIn workers, for allowed2CheckIn this value must always be false
    QString name; // <second name> <first name>
    QString totalTimeToday;
    QString totalTimeSeason;
    QStringList list_checkInToday; // Morning time set by boss is always the first value of the list
    QStringList list_checkOutToday;

};

#endif // EMPLOYEE_H
