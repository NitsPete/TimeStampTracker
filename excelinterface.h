#ifndef EXCELINTERFACE_H
#define EXCELINTERFACE_H

#include <QList>
#include <QProcess>
#include <QLocale>

#include <QDebug>

#include "employee.h"

static const QString PATH_LIBREOFFICE_FILE = "/home/admin/SharedFolder/Arbeitszeiten.ods";
static const QString PATH_GET_ROW = "/home/admin/Desktop/TimeStampTracker/pythonLibreOfficeAPI/readRow.py";

class ExcelInterface
{
public:
    ExcelInterface();

    static QList<Employee> getList_employee();

private:
    static Employee getEmployee(unsigned int number); // number=2 returns second employee on the list

};

#endif // EXCELINTERFACE_H
