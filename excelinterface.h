#ifndef EXCELINTERFACE_H
#define EXCELINTERFACE_H

#include <QList>
#include <QProcess>
#include <QLocale>
#include <QPair>
#include <QDebug>

#include "employee.h"

static const QString PATH_LIBREOFFICE_FILE = "/home/admin/SharedFolder/NewTestFile.ods";

// toDo Make path dynamic
const QString PATH = "/home/admin/Desktop/TimeStampTracker/pythonLibreOfficeAPI/";
const QString PATH_GET_ROW = PATH + "readRow.py";
const QString PATH_WRITE_TIME = PATH + "writeTime.py";
const QString PATH_INIT_LIBRE_OFFICE_FILE = PATH + "createInitLibreOfficeFileIfNotExist.py";

typedef enum eCheckTime
{
    CHECKIN,
    CHECKOUT
}eCheckTime;

class ExcelInterface
{
public:
    ExcelInterface();

    static QPair<QString, QString> runPythonProcess(QStringList params); // QPair<ourput, errorOutput>
    static QList<Employee> getList_employee();
    static void addCheckInTime(Employee *employee, QTime checkInTime);
    static void addCheckOutTime(Employee *employee, QTime checkOutTime);


private:
    static Employee getEmployee(unsigned int number); // number=2 returns second employee on the list
    static void addTime(Employee *employee, eCheckTime checkTime, QTime time);
};

#endif // EXCELINTERFACE_H
