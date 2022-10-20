
#include <QUrl>
#include <QSettings>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QNetworkInterface>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "email/emailmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    svr = WebServer::getInstance();
    svr->init();
    b_started = false;
    email_setting = new SendSetting();
    EmailManager::getInstance().InitEmailManager();
    this->setWindowTitle("NexusAI-0.1.0");

    foreach(QNetworkInterface intf, QNetworkInterface::allInterfaces())
    {
        if(intf.flags().testFlag(QNetworkInterface::IsUp) && !intf.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            foreach (QNetworkAddressEntry entry, intf.addressEntries())
            {
                if(intf.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains(".") && !intf.humanReadableName().contains("VM"))
                {
                    local_ip = entry.ip().toString();
                    qDebug()<<"local ip:"<<local_ip;

                }
            }
        }

    }
    ui->lineEditAddressIP->setText(local_ip);

    QSettings settings("./Config.ini",QSettings::IniFormat);
    settings.beginGroup("infer");
    QString infer_type = settings.value("infer_type").toString();
    settings.endGroup();
    if(infer_type.startsWith("Openvino"))
    {
        ui->radioButtonCV->setChecked(false);
        ui->radioButtonVino->setChecked(true);
    }
    else
    {
        ui->radioButtonCV->setChecked(true);
        ui->radioButtonVino->setChecked(false);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnStart_clicked()
{
    if(!b_started)
    {
        b_started = true;
        svr->start();
        ui->labelStatus->setStyleSheet(QString("QLabel{ border-radius:16px; background-color: rgb(137, 244, 97); }"));
    }

}


void MainWindow::on_btnStop_clicked()
{
    if(b_started)
    {
        b_started = false;
        svr->stop();
        ui->labelStatus->setStyleSheet(QString("QLabel{ border-radius:16px; background-color: rgb(255, 46, 49); }"));
    }
}

//帮助
void MainWindow::on_actionhelp_triggered()
{
    QString url_node = "http://fiber-punk.com";
    QDesktopServices::openUrl(QUrl(url_node));
}

//关于
void MainWindow::on_actionAbout_triggered()
{
    QString url_node = "https://fiber-punk.com/pages/about";
    QDesktopServices::openUrl(QUrl(url_node));
}

//opencv cnn choose
void MainWindow::on_radioButtonCV_clicked(bool checked)
{
    if(checked)
    {
        QSettings settings("./Config.ini",QSettings::IniFormat);
        settings.beginGroup("infer");
        settings.setValue("infer_type","CPU");
        settings.endGroup();
    }
    QMessageBox::information(this, "Notice", "Pease restart NexusAI");
}

//openvino cnn choose
void MainWindow::on_radioButtonVino_clicked(bool checked)
{
    if(checked)
    {
        QSettings settings("./Config.ini",QSettings::IniFormat);
        settings.beginGroup("infer");
        settings.setValue("infer_type","Openvino");
        settings.endGroup();
    }
    QMessageBox::information(this, "Notice", "Pease restart NexusAI");
}

void MainWindow::on_btnEmail_clicked()
{
    email_setting->show();
}
