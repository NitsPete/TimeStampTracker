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
const QString PATH_INIT_LiBRE_OFFICE_SHEET = PATH + "createNewSheet.py";

const int FAILED_TO_SAVE_FILE = -1;

typedef enum eCheckTime
{
    CHECKIN,
    CHECKOUT
}eCheckTime;

struct PythonOutput
{
    int returnVal;
    QString processOutput;
};

class ExcelInterface
{
public:
    ExcelInterface();

    static PythonOutput runPythonProcess(QStringList params); // QPair<ourput, errorOutput>
    static QList<Employee> getList_employee();
    static void addCheckInTime(Employee *employee, QTime checkInTime);
    static void addCheckOutTime(Employee *employee, QTime checkOutTime);


private:
    static Employee getEmployee(unsigned int number); // number=2 returns second employee on the list
    static void addTime(Employee *employee, eCheckTime checkTime, QTime time);
};

#endif // EXCELINTERFACE_H
