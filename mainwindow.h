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
    QTimer *timer_flashOutputLabel;
    Employee *currentEmployee = nullptr;
    QList<Employee> list_employee;

    const QString BUTTON_COLOR_RED = "QPushButton { background-color: red;}";
    const QString BUTTON_COLOR_GREEN = "QPushButton { background-color: green;}";

    const unsigned int FLASH_INTERVALL = 750; // ms
    const unsigned int MAX_FLASH_TIMES = 13; // How long should output text be visible = MAX_FLASH_TIMES * FLASH_INTERVALL
    unsigned int currentFlashCounter = 0;

    void initOutputLabel();
    void initDateTimeLabel();
    void initEmployeeList();
    void setPushButtonEmployeeColor(Employee *employee);

    void loadEmployee(Employee *employee);
    void loadTableView(Employee *employee);

    void unloadEmployee();

    void stopTimer_flashOutputLabel();

private slots:
    void pushButton_come_clicked();
    void pushButton_go_clicked();

    void updateOutputLabelFlash();
    void updateDateTime();
    void pushButton_employee_clicked(const QString &buttonText);
};
#endif // MAINWINDOW_H
