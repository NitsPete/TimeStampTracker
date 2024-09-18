#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton_come, &QPushButton::clicked, this, &MainWindow::pushButton_come_clicked);
    connect(ui->pushButton_go, &QPushButton::clicked, this, &MainWindow::pushButton_go_clicked);

    initDateTimeLabel();

    initEmployeeList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initDateTimeLabel()
{
    timer_updateDateTime = new QTimer(this);
    connect(timer_updateDateTime, &QTimer::timeout, this, &MainWindow::updateDateTime);
    timer_updateDateTime->start(1000); // 1 second
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
                    ui->pushButton_come->setEnabled(false);
                    ui->pushButton_go->setEnabled(true);
                    return;
                }else
                {
                    button->setStyleSheet(BUTTON_COLOR_GREEN);
                    ui->pushButton_come->setEnabled(true);
                    ui->pushButton_go->setEnabled(false);
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

    currentEmployee->addCheckInTime(QTime::currentTime());
    setPushButtonEmployeeColor(currentEmployee);
}

void MainWindow::pushButton_go_clicked()
{
    if(currentEmployee == nullptr)
    {
        qDebug() << "pushButton_go_clicked: nullptr exception!";
        return;
    }

    currentEmployee->addCheckOutTime(QTime::currentTime());
    setPushButtonEmployeeColor(currentEmployee);
}

// toDo hier weiter machen und die infos in die gui laden
void MainWindow::loadEmployee(Employee *employee)
{
    if(employee == nullptr)
    {
        qDebug() << "loadEmployee: nullptr exception!";
        return;
    }

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

    // toDo come and go button sollen ne zeit schreiben und die farbe ändern sowohl enabled tauschen


    // toDo hier noch die woche und saison zeit laden + die tabelle
    // toDo outlog out von einen user machen?

}

void MainWindow::updateDateTime()
{
    ui->label_time->setText(QTime::currentTime().toString("HH:mm:ss"));
    ui->label_date->setText(QDate::currentDate().toString("dd.MM.yyyy"));
}

void MainWindow::pushButton_employee_clicked(const QString &buttonText)
{
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

