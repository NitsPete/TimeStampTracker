#include "excelinterface.h"

ExcelInterface::ExcelInterface()
{

}

QPair<QString, QString> ExcelInterface::runPythonProcess(QStringList params)
{
    QProcess p;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.remove("LD_LIBRARY_PATH"); // Make sure to finde lib libreglo.so
    p.setProcessEnvironment(env);

    p.start("python3", params);
    p.waitForFinished(-1);

    QString output = p.readAll();
    QString errorOutput = p.readAllStandardError();

    return QPair<QString, QString>(output, errorOutput);
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

void ExcelInterface::addCheckInTime(Employee *employee, QTime checkInTime)
{
    employee->addCheckInTime(checkInTime);
    addTime(employee, eCheckTime::CHECKIN, checkInTime);
}

void ExcelInterface::addCheckOutTime(Employee *employee, QTime checkOutTime)
{
    employee->addCheckOutTime(checkOutTime);
    addTime(employee, eCheckTime::CHECKOUT, checkOutTime);
}

Employee ExcelInterface::getEmployee(unsigned int number)
{
    QStringList params;
    params << PATH_GET_ROW;
    params << PATH_LIBREOFFICE_FILE << QString::number(number);

    QPair<QString, QString> outputs = runPythonProcess(params);
    QString output = outputs.first;
    QString errorOutput = outputs.second;

    // allowed2CheckIn, name, time season time day, checkin1, checkout1, checkin2, checkout2, ...
    QStringList employeeData = output.split('\n');

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

void ExcelInterface::addTime(Employee *employee, eCheckTime checkTime, QTime time)
{
    QStringList params;
    params << PATH_WRITE_TIME
           << PATH_LIBREOFFICE_FILE
           << QString::number(employee->getUniqueId())
           << time.toString("hh:mm:ss")
           << QString::number(static_cast<int>(checkTime));

    QPair<QString, QString> outputs = runPythonProcess(params);
    QString output = outputs.first;
    QString errorOutput = outputs.second;
    if(!errorOutput.isEmpty())
    {
        qDebug() << errorOutput;
    }



    QString timeSeason = "";
    QString timeDay = "";

    QStringList times = output.split("\n");
    if(times.length() >= 2)
    {
        timeSeason = QLocale(QLocale::German).toString(times.at(0).toDouble(), 'f', 1) + " h";
        timeDay = times.at(1);

        int hour = 0;
        int minute = 0;
        int second = 0;
        QStringList timeStamps = timeDay.split(':');
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

    employee->setTotalTimeSeason(timeSeason);
    employee->setTotalTimeToday(timeDay);
}
