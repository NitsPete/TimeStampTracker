#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    for(Employee employee : list_employee)
    {
        QPushButton *tmpButton = new QPushButton(employee.getButtonText());
        connect(tmpButton, &QPushButton::clicked, this, [this, tmpButton]()
        {pushButton_employee_clicked(tmpButton->text());});
        ui->verticalLayout_mainLeft->addWidget(tmpButton);
    }
}

// toDo hier weiter machen und die infos in die gui laden
void MainWindow::loadEmployee(Employee *employee)
{
    if(employee == nullptr)
    {
        qDebug() << "loadEmployee: nullptr exception!";
        return;
    }

    qDebug() << employee->getName();

    if(employee->employeeIsCheckedIn())
    {
        qDebug() << "Is checked  in!";
    }else
    {
        qDebug() << "Is not cheked in!";
    }

    // toDo diese funktion fertig programmieren anhand diesr können dann auch die button angepasst werden; vllt muss hier auch eher der button übergeben werden?
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

