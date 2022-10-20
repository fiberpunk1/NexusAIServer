#ifndef EMAILSENDER_H
#define EMAILSENDER_H

#include <QObject>

class EmailSender : public QObject
{
    Q_OBJECT
public:
    explicit EmailSender(QObject *parent = nullptr);
    void SetLogin(QString us, QString p){user = us; pwd=p;}
    void SetSenderRecv(QString send, QString recv){e_sender = send; e_receivers=recv;}
    void SetMessage(QString file_path, QString message){e_file_path = file_path; e_message = message;}

    void SendEmail(QString smtp_server);

public slots:
    void SendEmailWithImg(QString img_path, QString msg);

signals:
    void SigMsg(QString);

private:

    QString user;
    QString pwd;
    QString e_sender;
    QString e_receivers;
    QString e_file_path;
    QString e_message;
};

#endif // EMAILSENDER_H
