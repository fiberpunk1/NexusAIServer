#ifndef SENDSETTING_H
#define SENDSETTING_H

#include <QWidget>

#include "email/SmtpMime"
#include "email/emailsender.h"

namespace Ui {
class SendSetting;
}

class SendSetting : public QWidget
{
    Q_OBJECT

public:
    explicit SendSetting(QWidget *parent = 0);
    ~SendSetting();

private slots:
    void SlotMsg(QString msg);
    void SlotSendEmail(QString img_path,QString msg);
    void on_btnSend_clicked();
    void on_btnSaveEmailSetting_clicked();
    void on_btnLoadImg_clicked();

private:
    Ui::SendSetting *ui;
    EmailSender* sender;
    QString img_path;
    void Delay(int mesc);

signals:
    void SigStartSendEmail(QString, QString);



};

#endif // SENDSETTING_H
