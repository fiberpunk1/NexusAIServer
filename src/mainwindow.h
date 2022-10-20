#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "sendsetting.h"
#include "socketserver/WebServer.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnStart_clicked();

    void on_btnStop_clicked();

    void on_actionhelp_triggered();

    void on_actionAbout_triggered();

    void on_radioButtonCV_clicked(bool checked);

    void on_radioButtonVino_clicked(bool checked);

    void on_btnEmail_clicked();

private:
    Ui::MainWindow *ui;
    QString local_ip;
    WebServer* svr;
    SendSetting* email_setting;

    bool b_started;
};

#endif // MAINWINDOW_H
