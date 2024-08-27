#include "excelinterface.h"

ExcelInterface::ExcelInterface()
{

}

QList<Employee> ExcelInterface::getList_employee()
{
    QList<Employee> list_employee;

    unsigned int number = 1;
    bool allEmployeeFound = false;
    do{
       Employee employee = getEmployee(number);

       if(!employee.getName().isEmpty())
       {
           list_employee.append(employee);
       }else
       {
           allEmployeeFound = true;
       }

       ++number;
    }while(!allEmployeeFound);

    return list_employee;
}

Employee ExcelInterface::getEmployee(unsigned int number)
{
    // ToDo: Only contains test code at the moment
    QList<Employee> list_employee;

    list_employee.append(Employee(1, true, "Mustermann Max", "3,5 h", "543,4 h"));
    list_employee.append(Employee(2, true, "AMustermann Max", "13,5 h", "1543,4 h"));
    list_employee.append(Employee(3, true, "BMustermann Max", "23,5 h", "2543,4 h"));
    list_employee.append(Employee(4, false, "CMustermann Max", "33,5 h", "3543,4 h"));
    list_employee.append(Employee(5, false, "DMustermann Max", "43,5 h", "4543,4 h"));
    list_employee.append(Employee(6, true, "EMustermann Max", "53,5 h", "5543,4 h"));
    list_employee.append(Employee(7, false, "FMustermann Max", "63,5 h", "6543,4 h"));
    list_employee.append(Employee(7, true, "", "63,5 h", "6543,4 h"));

    return list_employee[number-1];
}
