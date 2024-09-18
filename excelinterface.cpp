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
    list_employee.append(Employee(6, false, "EMustermann Max", "53,5 h", "5543,4 h"));
    list_employee.append(Employee(7, false, "FMustermann Max", "63,5 h", "6543,4 h"));
    list_employee.append(Employee(8, false, "GMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(8, true, "", "63,5 h", "6543,4 h")); // Zeigt Ende an weil der name leer ist ->  bedeutet dass die excel zelle leer ist

    // Ist nicht eingecheckt -> checked out
    list_employee[0].addCheckInTime(QTime(0, 30));
    list_employee[0].addCheckInTime(QTime(2, 30));
    list_employee[0].addCheckInTime(QTime(4, 30));
    list_employee[0].addCheckInTime(QTime(6, 30));

    list_employee[0].addCheckOutTime(QTime(1, 30));
    list_employee[0].addCheckOutTime(QTime(3, 17));
    list_employee[0].addCheckOutTime(QTime(5, 23));
    list_employee[0].addCheckOutTime(QTime(7, 30));

    // Ist nicht eingecheckt darf aber selber einchecken -> checked out, allowed to check in
    // empty list by employee[1]

    // Ist eingecheckt -> checked in
    list_employee[2].addCheckInTime(QTime(3, 30));

    // Waldi hat noch keine zeit eingtragen ist somit eingecheckt in der früh -> checked in allowed to check out
    // empty list by emplyee[3]

    // Er hat ausgecheckt und waldi hat noch nicht angecheckt -> checkout out until 24 Uhr allowed to check in
    list_employee[4].addCheckInTime(QTime(13, 30));

    list_employee[4].addCheckOutTime(QTime(12, 30));
    list_employee[4].addCheckOutTime(QTime(20, 30));

    // Waldi hat schon eingecheckt in der früh -> chechedk in allwoed to check out
    list_employee[5].setBossSetsMorningTime(true);
    list_employee[5].addCheckInTime(QTime(13, 30));


    // Er hat schon ausgecheckt aber waldi noch nicht rein -> checked out allowed to check in
    list_employee[6].addCheckOutTime(QTime(20, 30));

    // User checked out bevor waldis check in zeit -> keine zeit rechnung, user ist check in -> allowed to check out -> alle zeiten vor dem ersten check in sollten verworfen werden
    list_employee[7].setBossSetsMorningTime(true);
    list_employee[7].addCheckInTime(QTime(3, 50));
    list_employee[7].addCheckOutTime(QTime(3, 30));

    return list_employee[number-1];
}
