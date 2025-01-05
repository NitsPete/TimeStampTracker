#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QList>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QMouseEvent>
#include <QProcess>
#include <QMessageBox>

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

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QMainWindow::resizeEvent(event);
        updateTextSize();
    }

private:
    Ui::MainWindow *ui;
    QProcess *libreOfficeServer;
    QTimer *timer_updateDateTime;
    QTimer *timer_flashOutputLabel;
    Employee *currentEmployee = nullptr;
    QList<Employee> list_employee;

    const QString BUTTON_COLOR_RED = "QPushButton { color: white; background-color: red; border: none; }";
    const QString BUTTON_COLOR_GREEN = "QPushButton { color: white; background-color: green; border: none; }";

    const unsigned int FLASH_INTERVALL = 750; // ms
    const unsigned int MAX_FLASH_TIMES = 13; // How long should output text be visible = MAX_FLASH_TIMES * FLASH_INTERVALL
    unsigned int currentFlashCounter = 0;
    bool flashOutputLabel = false;

    void updateTextSize();
    void setTextSize(QWidget *widget, unsigned int scaleFactor);

    void initLibreOfficeServer();
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
