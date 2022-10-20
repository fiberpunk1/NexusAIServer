#include <QDebug>
#include "emailmanager.h"

QMutex EmailManager::mutex;
QScopedPointer<EmailManager> EmailManager::instance;

EmailManager::EmailManager(QObject *parent) : QObject(parent)
{

}
EmailManager& EmailManager::getInstance()
{
    if(instance.isNull())
    {
        mutex.lock();
        if(instance.isNull())
        {
            instance.reset(new EmailManager());
        }
        mutex.unlock();
    }
    return *instance.data();
}

void EmailManager::InitEmailManager()
{
    email_thread = new CEmailQueueThread();
    email_sender_t = new EmailSender();
    email_thread->start();
}

void EmailManager::StopEmailTask()
{

}

void EmailManager::AddEmailData(QStringList email_list)
{
    mutex.lock();
    qDebug()<<"add message";
    m_qQueueMsg.enqueue(email_list);
    m_waitCondition.notify_all();
    mutex.unlock();
}

void EmailManager::ProcessEmailData(QStringList email_content)
{
    if(email_content.size()==2)
    {
        qDebug()<<"Now sending email in emailmanager.cpp real";
        if(email_sender_t != nullptr)
        {
            delete email_sender_t;
            email_sender_t = nullptr;
        }

        email_sender_t = new EmailSender();
        connect(&EmailManager::getInstance(), SIGNAL(SigStartSendEmail(QString,QString)),email_sender_t,SLOT(SendEmailWithImg(QString,QString)));
        QThread* chread = new QThread();
        connect(chread, SIGNAL(finished()), chread, SLOT(deleteLater()));
        email_sender_t->moveToThread(chread);
        chread->start();
        emit EmailManager::getInstance().SigStartSendEmail(email_content.at(0), email_content.at(1));
    }
}
