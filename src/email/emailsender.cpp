#include <QThread>
#include <QSettings>
#include <QDebug>
#include "email/SmtpMime"
#include "emailsender.h"
#include "mimemessage.h"
#include "smtpexports.h"

EmailSender::EmailSender(QObject *parent) : QObject(parent)
{

}

void EmailSender::SendEmail(QString smtp_server)
{
    int port = 465;
    if((smtp_server=="smtp-mail.outlook.com") || (smtp_server=="smtp.office365.com"))
        port = 587;
    SmtpClient smtp(smtp_server, port, SmtpClient::SslConnection);

    if((smtp_server=="smtp-mail.outlook.com") || (smtp_server=="smtp.office365.com"))
    {
        smtp.setAuthMethod(SmtpClient::AuthLogin);
    }
    smtp.setUser(user);
    smtp.setPassword(pwd);

    MimeMessage message;
    message.setSender(new EmailAddress(e_sender));
    message.addRecipient(new EmailAddress(e_receivers, "Printer"));
    message.setSubject("From your own pc");

    MimeHtml html;
    html.setHtml(e_message);

    MimeInlineFile image1 (new QFile(e_file_path));

    message.addPart(&html);
    if(QFile::exists(e_file_path))
    {
        // Create a MimeInlineFile object for each imag
        // An unique content id must be setted
        image1.setContentId("image1");
        image1.setContentType("image/jpg");
        message.addPart(&image1);
    }

    emit SigMsg("Now connecting email server host...");
    if(smtp.connectToHost())
    {
        qDebug()<<"Connect email server succefful!";
        emit SigMsg("Connect email server succefful!");
        emit SigMsg("Now loging email server...");
        if(smtp.login())
        {
            qDebug()<<"login succefful!";
            emit SigMsg("Login email server succefful!");
            emit SigMsg("Now sending email...");
            if(smtp.sendMail(message))
            {
                qDebug()<<"send succefful!";
                emit SigMsg("Send succefful!\r\n");
            }
            else
            {
                emit SigMsg("Send failed!");
            }
        }
        else
        {
            emit SigMsg("Login failed!Please check your email's user name or password.\r\n");
            qDebug()<<"login failed!\n\n";
        }
    }
    else
    {
        emit SigMsg("Connect email server failed!Please check your network.\r\n");
    }

    smtp.quit();
}

void EmailSender::SendEmailWithImg(QString img_path, QString msg)
{
    QSettings settings("./Config.ini",QSettings::IniFormat);
    settings.beginGroup("email");
    QString smtp_type = (settings.value("smtp_type").toString());
    QString email_sender = (settings.value("sender_email").toString());
    QString email_psd = (QString(QByteArray::fromBase64(settings.value("sender_psd").toString().toLatin1())));
    QString email_receiver = (settings.value("receiver_email").toString());
    QString email_subject = settings.value("send_subject").toString();

    settings.endGroup();

    int port = 465;
    SmtpClient::ConnectionType ts=SmtpClient::SslConnection;
    if((smtp_type=="smtp-mail.outlook.com") || (smtp_type=="smtp.office365.com"))
    {
        port = 587;
        ts = SmtpClient::TlsConnection;
    }


    SmtpClient smtp(smtp_type, port, ts);

    if((smtp_type=="smtp-mail.outlook.com") || (smtp_type=="smtp.office365.com"))
    {
        smtp.setAuthMethod(SmtpClient::AuthLogin);
    }

    smtp.setUser(email_sender);
    smtp.setPassword(email_psd);

    MimeMessage message;
    message.setSender(new EmailAddress(email_sender));
    message.addRecipient(new EmailAddress(email_receiver, "Printer"));
    message.setSubject(email_subject);

    qDebug()<<"in emailsender.cpp SendEmailWithImg ";

    MimeHtml html;
    html.setHtml(msg);

    MimeInlineFile image1 (new QFile(img_path));
    message.addPart(&html);
    if(QFile(img_path).exists())
    {
        // Create a MimeInlineFile object for each image
        // An unique content id must be setted
        image1.setContentId("image1");
        image1.setContentType("image/jpg");
        message.addPart(&image1);
    }
    emit SigMsg("Now connecting email server host...");
    if(smtp.connectToHost())
    {
//        qDebug()<<"Connect email server succefful!";
        emit SigMsg("Connect email server succefful!");
        emit SigMsg("Now loging email server...");
        if(smtp.login())
        {
//            qDebug()<<"login succefful!";
            emit SigMsg("Login email server succefful!");
            emit SigMsg("Now sending email...");
            if(smtp.sendMail(message))
            {
//                qDebug()<<"send succefful!";
                emit SigMsg("Send succefful!\r\n");
            }
            else
            {
                emit SigMsg("Send failed!");
            }
        }
        else
        {
            emit SigMsg("Login failed!Please check your email's user name or password.");
//            qDebug()<<"login failed!";
        }
    }
    else
    {
        emit SigMsg("Connect email server failed!Please check your network.");
    }

    smtp.quit();
}

