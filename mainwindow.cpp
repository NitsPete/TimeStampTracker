#include "mainwindow.h"
#include "ui_mainwindow.h"

// toDo List:
// Find a way to write Times to libreOfficeInterface faster
// Find a way for auto update with libreOfficeInterface
// Logout employee if there a 10 seconds no mouse input (Just remove displayed informations) -> Failed to catch mouse events on mac!
// Look at later:
// Time should saved with seconds in excel (also consider seconds in calculations)
// Min. 50 employee buttons should be displayed on the left side. Maybe make a scrollbar
// Scroll bar disapeard if there are to much check in/out times added to model

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initLibreOfficeServer();
    initLibreOfficeFile();
    initLibreOfficeSheet();

    lastCheckedDate = QDate::currentDate();
    timer_check4newDay = new QTimer(this);

    connect(ui->pushButton_come, &QPushButton::clicked, this, &MainWindow::pushButton_come_clicked);
    connect(ui->pushButton_go, &QPushButton::clicked, this, &MainWindow::pushButton_go_clicked);

    connect(timer_check4newDay, &QTimer::timeout, this, &MainWindow::check4newDay);

    timer_check4newDay->start(60000); // Check every 60 seconds

    initOutputLabel();

    ui->label_timeDay->clear();
    ui->label_timeSeason->clear();

    initDateTimeLabel();

    initEmployeeList();
}

MainWindow::~MainWindow()
{
    if (libreOfficeServer->state() != QProcess::NotRunning) {
        libreOfficeServer->kill();
        libreOfficeServer->waitForFinished();
    }

    delete ui;
}

void MainWindow::updateTextSize()
{
    setTextSize(ui->pushButton_come, 4);
    setTextSize(ui->pushButton_go, 4);

    setTextSize(ui->label_time, 4);
    setTextSize(ui->label_date, 4);

    setTextSize(ui->label_day, 1);
    setTextSize(ui->label_timeDay, 1);
    setTextSize(ui->label_season, 1);
    setTextSize(ui->label_timeSeason, 1);

    setTextSize(ui->label_output, 4);

    int iconSize = ui->label_time->font().pointSize();
    ui->label_timeIcon->setFixedSize(iconSize, iconSize);
    ui->label_dateIcon->setFixedSize(iconSize, iconSize);
}

void MainWindow::setTextSize(QWidget *widget, unsigned int scaleFactor)
{
    QFont font;
    font = widget->font();
    font.setPointSize(std::min(widget->width(), widget->height()) / scaleFactor);
    widget->setFont(font);
}

void MainWindow::initLibreOfficeServer()
{
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
    QStringList params;
    params << PATH_INIT_LIBRE_OFFICE_FILE
           << PATH_LIBREOFFICE_FILE;

    QPair<QString, QString> outputs = ExcelInterface::runPythonProcess(params);
    QString errorOutput = outputs.second;

    if(!errorOutput.isEmpty())
    {
        qDebug() << errorOutput;
    }
}

void MainWindow::initLibreOfficeSheet()
{
    QStringList params;
    params << PATH_INIT_LiBRE_OFFICE_SHEET
           << PATH_LIBREOFFICE_FILE;

    QPair<QString, QString> outputs = ExcelInterface::runPythonProcess(params);
    QString errorOutput = outputs.second;

    if(!errorOutput.isEmpty())
    {
        qDebug() << errorOutput;
    }
}

void MainWindow::initOutputLabel()
{
    ui->label_output->clear();

    timer_flashOutputLabel = new QTimer(this);
    connect(timer_flashOutputLabel, &QTimer::timeout, this, &MainWindow::updateOutputLabelFlash);
}

void MainWindow::initDateTimeLabel()
{
    timer_updateDateTime = new QTimer(this);
    connect(timer_updateDateTime, &QTimer::timeout, this, &MainWindow::updateDateTime);
    timer_updateDateTime->start(1000); // ms
}

void MainWindow::initEmployeeList()
{
    list_employee = ExcelInterface::getList_employee();

    while(QLayoutItem *item = ui->verticalLayout_mainLeft->takeAt(0))
    {
        if(QWidget *widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }

    for(Employee &employee : list_employee)
    {
        QPushButton *tmpButton = new QPushButton(employee.getButtonText());
        connect(tmpButton, &QPushButton::clicked, this, [this, tmpButton]()
        {pushButton_employee_clicked(tmpButton->text());});
        ui->verticalLayout_mainLeft->addWidget(tmpButton);

        setPushButtonEmployeeColor(&employee);
    }
}

void MainWindow::setPushButtonEmployeeColor(Employee *employee)
{
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
}

void MainWindow::pushButton_come_clicked()
{
    if(currentEmployee == nullptr)
    {
        qDebug() << "pushButton_come_clicked: nullptr exception!";
        return;
    }

    ExcelInterface::addCheckInTime(currentEmployee, QTime::currentTime());
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
    if(currentEmployee == nullptr)
    {
        qDebug() << "pushButton_go_clicked: nullptr exception!";
        return;
    }

    ExcelInterface::addCheckOutTime(currentEmployee, QTime::currentTime());
    setPushButtonEmployeeColor(currentEmployee);

    flashOutputLabel = false;
    QPalette palette = ui->label_output->palette();
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->label_output->setPalette(palette);
    ui->label_output->setText(currentEmployee->getName() + " GATA LUCRU!");
    timer_flashOutputLabel->start(FLASH_INTERVALL); // ms
    unloadEmployee();
}

void MainWindow::loadEmployee(Employee *employee)
{
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
    timer_flashOutputLabel->stop();
    currentFlashCounter = 0;
    ui->label_output->clear();
}

void MainWindow::updateOutputLabelFlash()
{
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
        lastCheckedDate = currentDate;
        initLibreOfficeSheet();
    }
}

void MainWindow::pushButton_employee_clicked(const QString &buttonText)
{
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

