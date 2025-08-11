#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QList>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QProcess>
#include <QMessageBox>
#include <QMouseEvent>
#include <QElapsedTimer>
#include <QThread>
#include <QTcpSocket>
#include <thread>
#include <chrono>
#include <QDebug>

#include "excelinterface.h"
#include "functionlogger.h"


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

    bool eventFilter(QObject *obj, QEvent *event) override
    {
        Q_UNUSED(obj);

        if(event->type() == QEvent::MouseMove)
        {
            noMouseMovementCounter = 0;
        }

        return false;
    }

private:
    Ui::MainWindow *ui;

    QProcess *libreOfficeServer;
    QTimer *timer_noMouseMovement;
    QTimer *timer_updateDateTime;
    QTimer *timer_flashOutputLabel;
    Employee *currentEmployee = nullptr;
    QList<Employee> list_employee;
    QList<BufferedTime> list_bufferedTimes; // Contains times which aren't synchronize with database

    const QString BUTTON_COLOR_RED = "QPushButton { color: white; background-color: red; border: none; }";
    const QString BUTTON_COLOR_GREEN = "QPushButton { color: white; background-color: green; border: none; }";

    const unsigned int NO_MOUSE_MOVEMENT_INTERVALL = 5 * 1000; // [ms] Value must be less than below values
    const unsigned int INACTIVE_USER_TIMEOUT_TIME = 2 * NO_MOUSE_MOVEMENT_INTERVALL; // [ms]
    const unsigned int UPLOAD_AND_REINIT_INTERVALL = 12 * NO_MOUSE_MOVEMENT_INTERVALL; // [ms]

    const unsigned int CHECK4NEW_DAY_INTERVALL = 100; // [ms]
    const unsigned int CHECK4NEW_TIME = 1000; // [ms]
    const unsigned int FLASH_INTERVALL = 750; // [ms]
    const unsigned int MAX_FLASH_TIMES = 13; // How long should output text be visible = MAX_FLASH_TIMES * FLASH_INTERVALL
    unsigned int noMouseMovementCounter = 0;
    unsigned int currentFlashCounter = 0;
    bool flashOutputLabel = false;

    QTimer *timer_check4newDay;
    QDate lastCheckedDate;

    void updateTextSize();
    void setTextSize(QWidget *widget, unsigned int scaleFactor);

    void initLibreOfficeServer();
    void check4LibreOfficeServer();
    void restartLibreOfficeServer();
    void initLibreOfficeFile();
    void initLibreOfficeSheet();
    void initOutputLabel();
    void initDateTimeLabel();
    void initSavedDataLabel();
    void initEmployeeList();
    void setPushButtonEmployeeColor(Employee *employee);

    void loadEmployee(Employee *employee);
    void loadTableView(Employee *employee);
    void unloadEmployee();

    void stopTimer_flashOutputLabel();

private slots:
    void pushButton_come_clicked();
    void pushButton_go_clicked();

    void noMouseMovement();
    void updateOutputLabelFlash();
    void updateDateTime();
    void check4newDay();
    void pushButton_employee_clicked(const QString &buttonText);
};
#endif // MAINWINDOW_H
