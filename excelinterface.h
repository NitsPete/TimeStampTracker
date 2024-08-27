#ifndef EXCELINTERFACE_H
#define EXCELINTERFACE_H

#include <QList>

#include "employee.h"

class ExcelInterface
{
public:
    ExcelInterface();

    static QList<Employee> getList_employee();

private:
    static Employee getEmployee(unsigned int number); // number=2 returns second employee on the list

};

#endif // EXCELINTERFACE_H
