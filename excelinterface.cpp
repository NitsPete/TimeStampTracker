#include "excelinterface.h"

ExcelInterface::ExcelInterface()
{

}

QList<Employee> ExcelInterface::getList_employee()
{
    QList<Employee> list_employee;

    unsigned int number = 0;
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
    QProcess p;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.remove("LD_LIBRARY_PATH"); // Make sure to finde lib libreglo.so
    p.setProcessEnvironment(env);

    QStringList params;
    params << PATH_GET_ROW;
    params << PATH_LIBREOFFICE_FILE << QString::number(number);
    p.start("python3", params);
    p.waitForFinished(-1);

    QString output = p.readAll();
    // allowed2CheckIn, name, time season time day, checkin1, checkout1, checkin2, checkout2, ...
    QStringList employeeData = output.split('\n');

    QString errorOutput = p.readAllStandardError();
    if(!errorOutput.isEmpty())
    {
        qDebug() << errorOutput;
    }

    Employee *pEmployee;
    bool bossSetsMorningTime = false;
    bool allowed2CheckIn = false;
    QString name = "";
    QString timeSeason = "";
    QString timeDay = "";

    if(employeeData.length() > 0)
    {
        if(employeeData.at(0) != "-1")
        {
            allowed2CheckIn = true;
        }
    }

    if(employeeData.length() > 1)
    {
        name = employeeData.at(1);
    }

    if(employeeData.length() > 2)
    {
        timeSeason = QLocale(QLocale::German).toString(employeeData.at(2).toDouble(), 'f', 1);
        timeSeason += " h";
    }

    if(employeeData.length() > 3)
    {
        int hour = 0;
        int minute = 0;
        int second = 0;
        QStringList timeStamps = employeeData.at(3).split(':');
        if(timeStamps.length() >= 3)
        {
            hour = timeStamps.at(0).toInt();
            minute = timeStamps.at(1).toInt();
            second = timeStamps.at(2).toInt();
        }
        double time = hour + (double)minute / 60 + (double)second / 3600;
        timeDay = QLocale(QLocale::German).toString(time, 'f', 1);
        timeDay += " h";
    }

    pEmployee = new Employee(number, allowed2CheckIn, name, timeDay, timeSeason);

    for(int i = 4; i < employeeData.length(); ++i)
    {
        if((i == 4) && !allowed2CheckIn && !employeeData.at(4).isEmpty())
        {
            bossSetsMorningTime = true;
        }

        if(!employeeData.at(i).isEmpty())
        {
            int hour = 0;
            int minute = 0;
            QStringList timeStamps = employeeData.at(i).split(':');
            if(timeStamps.length() >= 2)
            {
                hour = timeStamps.at(0).toInt();
                minute = timeStamps.at(1).toInt();
            }

            if(i%2 == 0)
            {
                pEmployee->addCheckInTime(QTime(hour, minute));
            }else
            {
                pEmployee->addCheckOutTime(QTime(hour, minute));
            }

        }
    }

    pEmployee->setBossSetsMorningTime(bossSetsMorningTime);

    return *pEmployee;

}
