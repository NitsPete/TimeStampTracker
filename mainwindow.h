#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QList>

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
    QList<Employee> list_employee;

    void initDateTimeLabel();
    void initEmployeeList();
    void loadEmployee(Employee *employee);

private slots:
    void updateDateTime();
    void pushButton_employee_clicked(const QString &buttonText);
};
#endif // MAINWINDOW_H
