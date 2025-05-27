#include "mainwindow.h"
#include "ui_mainwindow.h"

// toDo List:
// Autostart hinzufügen vom System
// Daten in LibreOffice synchronisieren alle 60 Sekunden
// Tim neue Version geben
// Checken ob ein neuer Tag ist alle 100ms
// Programm friert ein bei neuen Tag (Uhrzeit war 00:00:44)
// Write errors per mail -> maybe better use a unsafe mail without 2 factor
// Daily send a backup per mail
// Mitarbeiter solllen im nachhinein geordnet werden können. Dies soll passieren durch eine Zahl vor jedem Namen. Dann anch Weißen und nicht weißen hintergrund bei erstellung eines sheets sortieren. Liste der Mitarbeiter soll nach ihrer uID sortiert werden.
// Wenn keine zeiten in der Zeiten liste sind dann nach einer Minute auch die Daten von LibreOffice ziehen
// check4newDay() -> list_bufferedTimes should be empty or a email should be send!
// Regex hinzufügen in libreoffice file
// Zeiten sollen maximal 24:00:00 enthalten können. Wenn Uhrzeiten ohne drücken von "Command" nach unten gezogen werden wird immer +24 addiert. Dieser Fehler kann so abgefangen werden!
// Schreibschutz hinzufügen in libreoffice file
// void MainWindow::noMouseMovement() -> If data where write to database GUI is frozen!
// Scroll bar disapears if there are to much check in/out times added to model

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    LOG_FUNCTION();

    ui->setupUi(this);

    qApp->installEventFilter(this); // Need filter to catch mouse movement

    initLibreOfficeServer();
    initLibreOfficeFile();
    initLibreOfficeSheet();

    lastCheckedDate = QDate::currentDate();
    timer_check4newDay = new QTimer(this);
    timer_noMouseMovement = new QTimer(this);

    connect(ui->pushButton_come, &QPushButton::clicked, this, &MainWindow::pushButton_come_clicked);
    connect(ui->pushButton_go, &QPushButton::clicked, this, &MainWindow::pushButton_go_clicked);

    connect(timer_check4newDay, &QTimer::timeout, this, &MainWindow::check4newDay);
    connect(timer_noMouseMovement, &QTimer::timeout, this, &MainWindow::noMouseMovement);

    timer_check4newDay->start(CHECK4NEW_DAY_INTERVALL);
    timer_noMouseMovement->start(NO_MOUSE_MOVEMENT_INTERVALL);

    initOutputLabel();

    ui->label_timeDay->clear();
    ui->label_timeSeason->clear();

    initDateTimeLabel();

    initEmployeeList();
}

MainWindow::~MainWindow()
{
    LOG_FUNCTION();

    if (libreOfficeServer->state() != QProcess::NotRunning)
    {
        libreOfficeServer->kill();
        libreOfficeServer->waitForFinished();
    }

    delete ui;
}

void MainWindow::updateTextSize()
{
    LOG_FUNCTION();

    setTextSize(ui->pushButton_come, 4);
    setTextSize(ui->pushButton_go, 4);

    setTextSize(ui->label_time, 4);
    setTextSize(ui->label_date, 4);

    setTextSize(ui->label_day, 2);
    setTextSize(ui->label_timeDay, 1);
    setTextSize(ui->label_season, 2);
    setTextSize(ui->label_timeSeason, 1);

    setTextSize(ui->label_output, 4);

    int iconSizeSavedData = 2 * ui->label_output->font().pointSize();
    ui->label_savedDataIcon->setFixedSize(iconSizeSavedData, iconSizeSavedData);

    int iconSizeDateTime = ui->label_time->font().pointSize();
    ui->label_timeIcon->setFixedSize(iconSizeDateTime, iconSizeDateTime);
    ui->label_dateIcon->setFixedSize(iconSizeDateTime, iconSizeDateTime);
}

void MainWindow::setTextSize(QWidget *widget, unsigned int scaleFactor)
{
    LOG_FUNCTION();

    QFont font;
    font = widget->font();
    font.setPointSize(std::min(widget->width(), widget->height()) / scaleFactor);
    widget->setFont(font);
}

void MainWindow::initLibreOfficeServer()
{
    LOG_FUNCTION();

    QString libreOfficePath = "libreoffice";
    QStringList args;
    args << "--calc" << "--accept=socket,host=localhost,port=2002;urp;" << "--nologo" << "--headless" << "--invisible";
    libreOfficeServer = new QProcess();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.remove("LD_LIBRARY_PATH"); // Make sure to finde lib libreglo.so
    libreOfficeServer->setProcessEnvironment(env);
    libreOfficeServer->start(libreOfficePath, args);

    if(!libreOfficeServer->waitForStarted())
    {
        QMessageBox::critical(this, "Process start failed", "Unable to start LibreOffice-Server\nDatabase may not work!");
    }

    // Makes sure the server is online
    while(!check4LibreOfficeServer());
}

bool MainWindow::check4LibreOfficeServer()
{
    LOG_FUNCTION();

    static unsigned int count = 1;
    QProcess process;
    process.start("bash", QStringList() << "-c" << "telnet 127.0.0.1 2002");
    process.waitForFinished(1000); // If connect to servr this will raise: "QProcess: Destroyed while process ("bash") is still running."

    QString output = process.readAllStandardOutput();

    if (output.contains("Connected"))
    {
        qDebug() << "LibreOffice-Server is online!";
        count = 1;
        return true;
    } else {
        qDebug() << QString::number(count) << ": Wait for LibreOffice-Server!";
        ++count;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Wait 1 second
    }
    return false;
}

void MainWindow::initLibreOfficeFile()
{
    LOG_FUNCTION();

    QStringList params;
    params << PATH_INIT_LIBRE_OFFICE_FILE
           << PATH_LIBREOFFICE_FILE;

    PythonOutput outputs = ExcelInterface::runPythonProcess(params);
    int returnVal = outputs.returnVal;

    if(returnVal == FAILED_TO_SAVE_FILE)
    {
        QMessageBox::critical(nullptr, "Unabel to save file", "Please ask admin to check premission of folder!");
    }
}

void MainWindow::initLibreOfficeSheet()
{
    LOG_FUNCTION();

    QStringList params;
    params << PATH_INIT_LiBRE_OFFICE_SHEET
           << PATH_LIBREOFFICE_FILE;

    PythonOutput outputs = ExcelInterface::runPythonProcess(params);
    int returnVal = outputs.returnVal;

    if(returnVal == FAILED_TO_SAVE_FILE)
    {
        QMessageBox::critical(nullptr, "Unabel to save file", "Please ask admin to close LibreOffice file!");
        initLibreOfficeSheet();
    }
}

void MainWindow::initOutputLabel()
{
    LOG_FUNCTION();

    ui->label_output->clear();

    timer_flashOutputLabel = new QTimer(this);
    connect(timer_flashOutputLabel, &QTimer::timeout, this, &MainWindow::updateOutputLabelFlash);
}

void MainWindow::initDateTimeLabel()
{
    LOG_FUNCTION();

    timer_updateDateTime = new QTimer(this);
    connect(timer_updateDateTime, &QTimer::timeout, this, &MainWindow::updateDateTime);
    timer_updateDateTime->start(CHECK4NEW_TIME); // ms
}

void MainWindow::initEmployeeList()
{
    LOG_FUNCTION();

    QList<Employee> tmpList = ExcelInterface::getList_employee();

    // If databank is saved from outside while trying to init employee list
    if(tmpList.isEmpty())
    {
        return;
    }

    list_employee = tmpList;

    while(QLayoutItem *item = ui->verticalLayout_mainLeft->takeAt(0))
    {
        if(QWidget *widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }
    while(QLayoutItem *item = ui->verticalLayout_mainRight->takeAt(0))
    {
        if(QWidget *widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }

    int i = 0;
    for(Employee &employee : list_employee)
    {
        QPushButton *tmpButton = new QPushButton(employee.getButtonText());
        connect(tmpButton, &QPushButton::clicked, this, [this, tmpButton]()
        {pushButton_employee_clicked(tmpButton->text());});

        if(i % 2 == 0)
        {
            ui->verticalLayout_mainLeft->addWidget(tmpButton);
        }else
        {
            ui->verticalLayout_mainRight->addWidget(tmpButton);
        }

        setPushButtonEmployeeColor(&employee);
        ++i;
    }
}

void MainWindow::setPushButtonEmployeeColor(Employee *employee)
{
    // No logging because to much spam
    //LOG_FUNCTION();

    for(int i = 0; i < ui->verticalLayout_mainLeft->count(); ++i)
    {
        QWidget *widget = ui->verticalLayout_mainLeft->itemAt(i)->widget();

        if (QPushButton *button = qobject_cast<QPushButton*>(widget))
        {
            if(button->text().startsWith(QString::number(employee->getUniqueId()) + ":"))
            {
                if(employee->employeeIsCheckedIn())
                {
                    button->setStyleSheet(BUTTON_COLOR_RED);
                    return;
                }else
                {
                    button->setStyleSheet(BUTTON_COLOR_GREEN);
                    return;
                }
            }
        }
    }

    for(int i = 0; i < ui->verticalLayout_mainRight->count(); ++i)
    {
        QWidget *widget = ui->verticalLayout_mainRight->itemAt(i)->widget();

        if (QPushButton *button = qobject_cast<QPushButton*>(widget))
        {
            if(button->text().startsWith(QString::number(employee->getUniqueId()) + ":"))
            {
                if(employee->employeeIsCheckedIn())
                {
                    button->setStyleSheet(BUTTON_COLOR_RED);
                    return;
                }else
                {
                    button->setStyleSheet(BUTTON_COLOR_GREEN);
                    return;
                }
            }
        }
    }
}

void MainWindow::pushButton_come_clicked()
{
    LOG_FUNCTION();

    if(currentEmployee == nullptr)
    {
        qDebug() << "pushButton_come_clicked: nullptr exception!";
        return;
    }

    list_bufferedTimes.append(ExcelInterface::addCheckInTime(currentEmployee, QTime::currentTime()));
    ui->label_savedDataIcon->setPixmap(QPixmap(":/images/dataUnsaved.png"));
    setPushButtonEmployeeColor(currentEmployee);

    flashOutputLabel = false;
    QPalette palette = ui->label_output->palette();
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->label_output->setPalette(palette);
    ui->label_output->setText(currentEmployee->getName() + " START LUCRU!");
    timer_flashOutputLabel->start(FLASH_INTERVALL); // ms
    unloadEmployee();
}

void MainWindow::pushButton_go_clicked()
{
    LOG_FUNCTION();

    if(currentEmployee == nullptr)
    {
        qDebug() << "pushButton_go_clicked: nullptr exception!";
        return;
    }

    list_bufferedTimes.append(ExcelInterface::addCheckOutTime(currentEmployee, QTime::currentTime()));
    ui->label_savedDataIcon->setPixmap(QPixmap(":/images/dataUnsaved.png"));
    setPushButtonEmployeeColor(currentEmployee);

    flashOutputLabel = false;
    QPalette palette = ui->label_output->palette();
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->label_output->setPalette(palette);
    ui->label_output->setText(currentEmployee->getName() + " GATA LUCRU!");
    timer_flashOutputLabel->start(FLASH_INTERVALL); // ms
    unloadEmployee();
}

void MainWindow::noMouseMovement()
{
    ++noMouseMovementCounter;

    if((noMouseMovementCounter * NO_MOUSE_MOVEMENT_INTERVALL) == INACTIVE_USER_TIMEOUT_TIME)
    {
        FunctionLogger::writeLog(LOGGING_PATH, "    INACTIVE_USER_TIMEOUT_TIME");

        ui->label_output->setText("");
        unloadEmployee();
    }

    if(((noMouseMovementCounter * NO_MOUSE_MOVEMENT_INTERVALL) % UPLOAD_AND_REINIT_INTERVALL) == 0)
    {
        FunctionLogger::writeLog(LOGGING_PATH, "    UPLOAD_AND_REINIT_INTERVALL");

        if(!list_bufferedTimes.isEmpty())
        {
            FunctionLogger::writeLog(LOGGING_PATH, "    !list_bufferedTimes.isEmpty()");

            ExcelInterface::writeBufferedTimes2database(&list_bufferedTimes);
        }

        if(list_bufferedTimes.isEmpty())
        {
            FunctionLogger::writeLog(LOGGING_PATH, "    list_bufferedTimes.isEmpty()");

            ui->label_savedDataIcon->setPixmap(QPixmap(":/images/dataSaved.png"));

            initEmployeeList();
        }
    }
}

void MainWindow::loadEmployee(Employee *employee)
{
    LOG_FUNCTION();

    if(employee == nullptr)
    {
        qDebug() << "loadEmployee: nullptr exception!";
        return;
    }

    ui->label_output->clear();
    QPalette palette = ui->label_output->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_output->setPalette(palette);
    ui->label_output->setText("CORESPUNDE: " + employee->getName());

    currentEmployee = employee;

    if(employee->employeeIsCheckedIn())
    {
        ui->pushButton_come->setEnabled(false);
        ui->pushButton_go->setEnabled(true);
    }else
    {
        ui->pushButton_come->setEnabled(true);
        ui->pushButton_go->setEnabled(false);
    }

    ui->label_timeDay->setText(employee->getTotalTimeToday());
    ui->label_timeDay->setStyleSheet("");
    ui->label_timeSeason->setText(employee->getTotalTimeSeason());

    loadTableView(employee);
}

void MainWindow::loadTableView(Employee *employee)
{
    LOG_FUNCTION();

    unsigned int extraSpace;
    (!employee->getAllowed2CheckIn() && !employee->getBossSetsMorningTime()) ? extraSpace = 1 : extraSpace = 0;
    unsigned int columns = qMax((unsigned int)(employee->getList_checkInToday().length()+extraSpace), (unsigned int)employee->getList_checkOutToday().length());
    QStandardItemModel *model = new QStandardItemModel(2, columns, ui->tableView_timeStamps);

    model->setHeaderData(0, Qt::Vertical, QObject::tr("START: "));
    model->setHeaderData(1, Qt::Vertical, QObject::tr("STOP: "));

    for(unsigned int i = 0; i < columns; ++i)
    {
        model->setHeaderData(i, Qt::Horizontal, QObject::tr(QString::number(i+1).toStdString().c_str()));
    }

    // Insert start times
    for(int i = 0; i < employee->getList_checkInToday().length(); ++i)
    {
        if(!employee->getAllowed2CheckIn() && !employee->getBossSetsMorningTime())
        {
            model->setData(model->index(0, i+1), employee->getList_checkInToday().at(i));
        }else
        {
            model->setData(model->index(0, i), employee->getList_checkInToday().at(i));
        }
    }

    // Insert end times
    for(int i = 0; i < employee->getList_checkOutToday().length(); ++i)
    {
        model->setData(model->index(1, i), employee->getList_checkOutToday().at(i));
    }

    // If boss didn't add check in time, mark this time and day time yellow
    // But there always must be the item(0, 0)
    if(!model->item(0, 0))
    {
        model->setItem(0, 0, new QStandardItem(""));
    }
    QStandardItem *firstItem = model->item(0, 0);
    if(!employee->getAllowed2CheckIn() && !employee->getBossSetsMorningTime() && firstItem)
    {
        firstItem->setBackground(QBrush(Qt::yellow));
        ui->label_timeDay->setStyleSheet("QLabel { background-color: yellow; }");
    }

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int column = 0; column < model->columnCount(); ++column) {
            QStandardItem *item = model->item(row, column);
            if (item) {
                QFont font = item->font();
                font.setPointSize(std::min(this->width(), this->height()) / 50);
                font.setBold(false);
                item->setFont(font);
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

    QFont headerFont = ui->tableView_timeStamps->horizontalHeader()->font();
    headerFont.setPointSize(std::min(this->width(), this->height()) / 40);
    headerFont.setBold(true);
    ui->tableView_timeStamps->setFont(headerFont);
    ui->tableView_timeStamps->setModel(model);
    ui->tableView_timeStamps->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_timeStamps->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_timeStamps->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_timeStamps->show();
}

void MainWindow::unloadEmployee()
{
    LOG_FUNCTION();

    ui->pushButton_come->setEnabled(false);
    ui->pushButton_go->setEnabled(false);

    ui->label_timeDay->clear();
    ui->label_timeDay->setStyleSheet("");
    ui->label_timeSeason->clear();

    ui->tableView_timeStamps->setModel(nullptr);

    currentEmployee = nullptr;
}

void MainWindow::stopTimer_flashOutputLabel()
{
    LOG_FUNCTION();

    timer_flashOutputLabel->stop();
    currentFlashCounter = 0;
    ui->label_output->clear();
}

void MainWindow::updateOutputLabelFlash()
{
    LOG_FUNCTION();

    if(currentFlashCounter >= MAX_FLASH_TIMES)
    {
        stopTimer_flashOutputLabel();
        return;
    }else{
        ++currentFlashCounter;
    }

    flashOutputLabel = !flashOutputLabel;

    QPalette palette = ui->label_output->palette();


    QColor color;

    flashOutputLabel ? color = Qt::darkGray : color = Qt::black;

    palette.setColor(QPalette::WindowText, color);

    ui->label_output->setPalette(palette);
}

void MainWindow::updateDateTime()
{
    ui->label_time->setText(QTime::currentTime().toString("HH:mm:ss"));
    ui->label_date->setText(QDate::currentDate().toString("dd.MM.yyyy"));
}

void MainWindow::check4newDay()
{
    QDate currentDate = QDate::currentDate();
    if(lastCheckedDate != currentDate)
    {
        LOG_FUNCTION();

        lastCheckedDate = currentDate;
        initLibreOfficeSheet();
        initEmployeeList();
    }
}

void MainWindow::pushButton_employee_clicked(const QString &buttonText)
{
    LOG_FUNCTION();

    stopTimer_flashOutputLabel();

    unsigned int uniqueId = buttonText.split(":").first().toUInt();

    Employee *clickedEmployee = nullptr;
    for(Employee &employee : list_employee)
    {
        if(uniqueId == employee.getUniqueId())
        {
            clickedEmployee = &employee;
        }
    }
    loadEmployee(clickedEmployee);
}
