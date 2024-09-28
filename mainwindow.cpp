#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton_come, &QPushButton::clicked, this, &MainWindow::pushButton_come_clicked);
    connect(ui->pushButton_go, &QPushButton::clicked, this, &MainWindow::pushButton_go_clicked);

    ui->label_timeDay->clear();
    ui->label_timeWeek->clear();
    ui->label_timeSeason->clear();

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

    currentEmployee->addCheckInTime(QTime::currentTime());
    setPushButtonEmployeeColor(currentEmployee);

    unloadEmployee();
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

    unloadEmployee();
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

    ui->label_timeDay->setText(employee->getTotalTimeToday());
    ui->label_timeWeek->setText(employee->getTotalTimeWeek());
    ui->label_timeSeason->setText(employee->getTotalTimeSeason());

    loadTableView(employee);

    // toDo Überlegen was in das output window angezeigt werden soll + hier noch die woche und saison zeit laden
}

void MainWindow::loadTableView(Employee *employee)
{
    unsigned int columns = qMax(employee->getList_checkInToday().length(), employee->getList_checkOutToday().length());
    QStandardItemModel *model = new QStandardItemModel(2, columns, ui->tableView_timeStamps);

    model->setHeaderData(0, Qt::Vertical, QObject::tr("Anfang"));
    model->setHeaderData(1, Qt::Vertical, QObject::tr("Ende"));

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

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int column = 0; column < model->columnCount(); ++column) {
            QStandardItem *item = model->item(row, column);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

    //toDo der scrollbar verschwindet manchmal
    //toDo die tabelle muss bei jeden neuen Zeiteintrag geupdatet werden (schauen wie am besten (signal in der employye classe bei add time?

    ui->tableView_timeStamps->setModel(model);
    ui->tableView_timeStamps->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_timeStamps->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_timeStamps->show();
}

void MainWindow::unloadEmployee()
{
    ui->pushButton_come->setEnabled(false);
    ui->pushButton_go->setEnabled(false);

    ui->label_timeDay->clear();
    ui->label_timeWeek->clear();
    ui->label_timeSeason->clear();

    ui->tableView_timeStamps->setModel(nullptr);

    currentEmployee = nullptr;
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

