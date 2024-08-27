#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QStringList>

class Employee
{
public:
    Employee(unsigned int uniqueId, bool allowed2CheckIn, QString name, QString totalTimeToday, QString totalTimeThisSeason);

    QString getName();
    QString getButtonText(); // <uniqueId>: <name>

private:
    unsigned int uniqueId;
    bool allowed2CheckIn; // only some employee are allowed to self check in the first time in the morning
    QString name; // <second name> <first name>
    QString totalTimeToday;
    QString totalTimeThisSeason;
    QStringList list_checkInToday;
    QStringList list_checkOutToday;

};

#endif // EMPLOYEE_H
