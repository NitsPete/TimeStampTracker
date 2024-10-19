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

    list_employee.append(Employee(1, true, "Alin Tega", "3,5 h", "543,4 h"));
    list_employee.append(Employee(2, true, "Adrian Tega", "13,5 h", "1543,4 h"));
    list_employee.append(Employee(3, false, "Liviu Tega", "23,5 h", "2543,4 h"));
    list_employee.append(Employee(4, false, "Nicolai Tega", "33,5 h", "3543,4 h"));
    list_employee.append(Employee(5, true, "Piotr Kutcinski", "43,5 h", "4543,4 h"));
    list_employee.append(Employee(6, true, "Piotr Gryglac", "53,5 h", "5543,4 h"));
    list_employee.append(Employee(7, true, "Waldemar Ciolek", "63,5 h", "6543,4 h"));
    list_employee.append(Employee(8, false, "GMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(9, true, "XMustermann Max", "73,5 h", "7543,4 h"));

    //*

    list_employee.append(Employee(10, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(11, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(12, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(13, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(14, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(15, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(16, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(17, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(18, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(19, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(20, true, "XMustermann Max", "73,5 h", "7543,4 h"));
/*
    list_employee.append(Employee(21, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(22, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(23, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(24, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(25, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(26, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(27, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(28, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(29, true, "XMustermann Max", "73,5 h", "7543,4 h"));

    list_employee.append(Employee(30, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(31, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(32, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(33, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(34, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(35, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(36, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(37, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(38, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(39, true, "XMustermann Max", "73,5 h", "7543,4 h"));

    list_employee.append(Employee(41, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(42, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(43, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(44, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(45, true, "XMustermann Max", "73,5 h", "7543,4 h"));

    list_employee.append(Employee(46, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(47, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(48, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    list_employee.append(Employee(49, true, "XMustermann Max", "73,5 h", "7543,4 h"));
    */
    //*/

    list_employee.append(Employee(99, true, "", "63,5 h", "6543,4 h")); // Zeigt Ende an weil der name leer ist ->  bedeutet dass die excel zelle leer ist
/*
    // Ist nicht eingecheckt -> checked out
    list_employee[0].addCheckInTime(QTime(0, 30));
    list_employee[0].addCheckInTime(QTime(2, 30));
    list_employee[0].addCheckInTime(QTime(4, 30));
    list_employee[0].addCheckInTime(QTime(6, 30));
    list_employee[0].addCheckInTime(QTime(8, 30));
    list_employee[0].addCheckInTime(QTime(10, 30));
    list_employee[0].addCheckInTime(QTime(12, 30));

    list_employee[0].addCheckOutTime(QTime(1, 30));
    list_employee[0].addCheckOutTime(QTime(3, 17));
    list_employee[0].addCheckOutTime(QTime(5, 23));
    list_employee[0].addCheckOutTime(QTime(7, 30));
    list_employee[0].addCheckOutTime(QTime(9, 17));
    list_employee[0].addCheckOutTime(QTime(11, 23));
    list_employee[0].addCheckOutTime(QTime(13, 30));

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
*/
    // User checked out bevor waldis check in zeit -> keine zeit rechnung, user ist check in -> allowed to check out -> alle zeiten vor dem ersten check in sollten verworfen werden
    list_employee[7].setBossSetsMorningTime(true);
    list_employee[7].addCheckInTime(QTime(3, 50));
    list_employee[7].addCheckOutTime(QTime(3, 30));

    return list_employee[number-1];
}
