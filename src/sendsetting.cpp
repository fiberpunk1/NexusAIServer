#include <windows.h>
#include <dbt.h>

#include <QTimer>
#include <QDebug>
#include <QRegExp>
#include <QProcess>
#include <QThread>
#include <QEventLoop>
#include <QFileDialog>
#include <QStorageInfo>
#include <QMessageBox>
#include <QSettings>
#include <QNetworkInterface>
#include <QFileInfo>
#include <QNetworkConfigurationManager>

#include "sendsetting.h"
#include "ui_sendsetting.h"


SendSetting::SendSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendSetting)
{
    ui->setupUi(this);
    this->setWindowTitle("Setting");
    sender = new EmailSender();
    img_path = "";

    QSettings settings("./Config.ini",QSettings::IniFormat);

    settings.beginGroup("email");

    ui->comboBoxSmtpType->setCurrentText(settings.value("smtp_type").toString());
    ui->lineEditSender->setText(settings.value("sender_email").toString());
    ui->lineEditPwd->setText(QString(QByteArray::fromBase64(settings.value("sender_psd").toString().toLatin1())));
    ui->lineEditReceive->setText(settings.value("receiver_email").toString());
    ui->lineEditSubject1->setText(settings.value("send_subject").toString());
    ui->lineEditConfidenceThreshold->setText(settings.value("confidence_threshold").toString());
    settings.endGroup();

    settings.beginGroup("infer");
    QString infer_type = settings.value("infer_type").toString();
    QString detect_interval = settings.value("detect_interval_time").toString();
    QString record_interval = settings.value("image_save_interval_time").toString();
    settings.endGroup();


}

SendSetting::~SendSetting()
{
    delete ui;

}

void SendSetting::Delay(int mesc)
{
    QEventLoop eventloop;
    QTimer::singleShot(mesc,&eventloop,SLOT(quit()));
    eventloop.exec();
}


void SendSetting::SlotSendEmail(QString t_img_path,QString msg)
{
    QThread* chread = new QThread();
    sender = new EmailSender();
    connect(chread, SIGNAL(finished()), chread, SLOT(deleteLater()));
    sender->moveToThread(chread);
    chread->start(QThread::HighPriority);

    sender->SetLogin(ui->lineEditSender->text(), ui->lineEditPwd->text());
    sender->SetSenderRecv(ui->lineEditSender->text(), ui->lineEditReceive->text());
    sender->SetMessage(t_img_path, msg);
    sender->SendEmail(ui->comboBoxSmtpType->currentText());

}


void SendSetting::SlotMsg(QString msg)
{
    ui->labelSendingEmail->setText(msg);
//    emit SigLogStr(msg);
}

//send mail
void SendSetting::on_btnSend_clicked()
{
    if(sender != nullptr)
    {
        delete sender;
        sender = nullptr;
    }
    sender = new EmailSender();
    QThread* chread = new QThread();
    connect(chread, SIGNAL(finished()), chread, SLOT(deleteLater()));
    QObject::connect(sender, SIGNAL(SigMsg(QString)),this,SLOT(SlotMsg(QString)));
    connect(this, SIGNAL(SigStartSendEmail(QString,QString)),sender,SLOT(SendEmailWithImg(QString,QString)));
    sender->moveToThread(chread);
    chread->start();
    emit SigStartSendEmail(img_path, tr("from your Fiberpunk NexusAI, test sending email!"));
}

//保存所有邮箱相关设置
void SendSetting::on_btnSaveEmailSetting_clicked()
{
    QSettings settings("./Config.ini",QSettings::IniFormat);
    settings.beginGroup("email");
    settings.setValue("smtp_type",ui->comboBoxSmtpType->currentText());
    settings.setValue("sender_email",ui->lineEditSender->text());
    settings.setValue("sender_psd",ui->lineEditPwd->text().toUtf8().toBase64());
    settings.setValue("receiver_email",ui->lineEditReceive->text());
    settings.setValue("send_subject",ui->lineEditSubject1->text());
    settings.setValue("confidence_threshold", ui->lineEditConfidenceThreshold->text());
    settings.endGroup();
    QMessageBox::information(this, tr("Notice"),"Configs Saved!");
}
//send image
void SendSetting::on_btnLoadImg_clicked()
{
    img_path = QFileDialog::getOpenFileName(this, "Image", ".", "IMG(*jpg *png)");
}
