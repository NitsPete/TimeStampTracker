#include "excelinterface.h"

ExcelInterface::ExcelInterface()
{

}

PythonOutput ExcelInterface::runPythonProcess(QStringList params)
{
    QProcess p;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.remove("LD_LIBRARY_PATH"); // Make sure to finde lib libreglo.so
    p.setProcessEnvironment(env);

    p.start("python3", params);
    p.waitForFinished(-1);

    PythonOutput pythonOutput;
    pythonOutput.returnVal = p.exitCode();
    pythonOutput.processOutput = p.readAll();

    QString processErrorOutput = p.readAllStandardError();

    if(!pythonOutput.processOutput.isEmpty())
    {
        qDebug() << pythonOutput.processOutput;
    }

    if(!processErrorOutput.isEmpty())
    {
        qDebug() << "Error: " << processErrorOutput;
    }

    return pythonOutput;
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

BufferedTime ExcelInterface::addCheckInTime(Employee *employee, QTime checkInTime)
{
    employee->addCheckInTime(checkInTime);
    return addTime(employee, eCheckTime::CHECKIN, checkInTime);
}

BufferedTime ExcelInterface::addCheckOutTime(Employee *employee, QTime checkOutTime)
{
    employee->addCheckOutTime(checkOutTime);
    return addTime(employee, eCheckTime::CHECKOUT, checkOutTime);
}

Employee ExcelInterface::getEmployee(unsigned int number)
{
    QStringList params;
    params << PATH_GET_ROW;
    params << PATH_LIBREOFFICE_FILE << QString::number(number);

    PythonOutput outputs = runPythonProcess(params);
    QString output = outputs.processOutput;

    // allowed2CheckIn, name, time season time day, checkin1, checkout1, checkin2, checkout2, ...
    QStringList employeeData = output.split('\n');

    Employee *pEmployee;
    bool bossSetsMorningTime = false;
    bool allowed2CheckIn = false;
    QString name = "";
    double timeSeason = 0;
    double timeDay = 0;

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
        timeSeason = employeeData.at(2).toDouble();
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
        timeDay = hour + (double)minute / 60 + (double)second / 3600;
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

BufferedTime ExcelInterface::addTime(Employee *employee, eCheckTime checkTime, QTime time)
{
    BufferedTime bufferedTime;
    bufferedTime.uniqueId = QString::number(employee->getUniqueId());
    bufferedTime.time = time.toString("hh:mm:ss");
    bufferedTime.checkTime = QString::number(static_cast<int>(checkTime));


    double timeSeason = employee->getTotalTimeSeasonDoubleValue();
    double timeDay = employee->getTotalTimeTodayDoubleValue();

    // Update time season + today
    QStringList list_checkInTimes = employee->getList_checkInToday();
    if((checkTime == eCheckTime::CHECKOUT) && !list_checkInTimes.isEmpty())
    {
        QTime checkInTime = QTime::fromString(list_checkInTimes.last(), "hh:mm");
        double timeDiff = checkInTime.secsTo(time) / 3600.0;
        timeSeason += timeDiff;
        timeDay += timeDiff;
    }

    employee->setTotalTimeSeason(timeSeason);
    employee->setTotalTimeToday(timeDay);

    return bufferedTime;
}

void ExcelInterface::writeBufferedTimes2database(QList<BufferedTime> *list_bufferedTimes)
{
    if(list_bufferedTimes->isEmpty())
    {
        return;
    }

    QStringList params;
    params << PATH_WRITE_TIME
           << PATH_LIBREOFFICE_FILE;
    for(BufferedTime bufferedTime : *list_bufferedTimes)
    {
        params << bufferedTime.uniqueId
               << bufferedTime.time
               << bufferedTime.checkTime;
    }
    PythonOutput outputs = runPythonProcess(params);
    int returnVal = outputs.returnVal;

    if(returnVal == SUCCESS)
    {
        list_bufferedTimes->clear();
    }
}
