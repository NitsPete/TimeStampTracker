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
    QStringList params;
    params << PATH_GET_DATA;
    params << PATH_LIBREOFFICE_FILE;

    PythonOutput outputs = runPythonProcess(params);
    QString output = outputs.processOutput;

    QStringList list_employeeDataStr = output.split("\r\n");
    list_employeeDataStr.removeAll("");

    QList<Employee> list_employee;
    for(QString employeeDataStr : list_employeeDataStr)
    {
        list_employee.append(getEmployee(employeeDataStr));
    }

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

Employee ExcelInterface::getEmployee(QString output)
{
    // uniqueId, allowed2CheckIn, name, time season, time day, checkin1, checkout1, checkin2, checkout2, ...
    QStringList employeeData = output.split('\n');

    Employee *pEmployee;
    unsigned int number = 0;
    bool bossSetsMorningTime = false;
    bool allowed2CheckIn = false;
    QString name = "";
    double timeSeason = 0;
    double timeDay = 0;

    if(employeeData.length() > EmployeeData::RowNumber)
    {
        number = employeeData.at(EmployeeData::RowNumber).toUInt();
    }

    if(employeeData.length() > EmployeeData::Allowed2CheckIn)
    {
        if(employeeData.at(EmployeeData::Allowed2CheckIn) != "-1")
        {
            allowed2CheckIn = true;
        }
    }

    if(employeeData.length() > EmployeeData::Name)
    {
        name = employeeData.at(EmployeeData::Name);
    }

    if(employeeData.length() > EmployeeData::TimeSeason)
    {
        timeSeason = employeeData.at(EmployeeData::TimeSeason).toDouble();
    }

    if(employeeData.length() > EmployeeData::TimeDay)
    {
        int hour = 0;
        int minute = 0;
        int second = 0;
        QStringList timeStamps = employeeData.at(EmployeeData::TimeDay).split(':');
        if(timeStamps.length() >= 3)
        {
            hour = timeStamps.at(0).toInt();
            minute = timeStamps.at(1).toInt();
            second = timeStamps.at(2).toInt();
        }
        timeDay = hour + (double)minute / 60 + (double)second / 3600;
    }

    pEmployee = new Employee(number, allowed2CheckIn, name, timeDay, timeSeason);

    for(int i = EmployeeData::CheckInOutTimes; i < employeeData.length(); ++i)
    {
        if((i == EmployeeData::CheckInOutTimes) && !allowed2CheckIn && !employeeData.at(EmployeeData::CheckInOutTimes).isEmpty())
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

            if((i-EmployeeData::CheckInOutTimes)%2 == 0)
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
