#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QList>
#include <QStandardItemModel>
#include <QScrollBar>

#include <QDebug>

#include "excelinterface.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *timer_updateDateTime;
    Employee *currentEmployee = nullptr;
    QList<Employee> list_employee;

    const QString BUTTON_COLOR_RED = "QPushButton { background-color: red;}";
    const QString BUTTON_COLOR_GREEN = "QPushButton { background-color: green;}";

    void initDateTimeLabel();
    void initEmployeeList();
    void setPushButtonEmployeeColor(Employee *employee);
    void loadEmployee(Employee *employee);
    void loadTableView(Employee *employee);

private slots:
    void pushButton_come_clicked();
    void pushButton_go_clicked();

    void updateDateTime();
    void pushButton_employee_clicked(const QString &buttonText);
};
#endif // MAINWINDOW_H
