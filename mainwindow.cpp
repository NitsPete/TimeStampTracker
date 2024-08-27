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
        ui->verticalLayout_mainLeft->addWidget(new QPushButton(employee.getButtonText()));
    }
}


void MainWindow::updateDateTime()
{
    ui->label_time->setText(QTime::currentTime().toString("HH:mm:ss"));
    ui->label_date->setText(QDate::currentDate().toString("dd.MM.yyyy"));
}

