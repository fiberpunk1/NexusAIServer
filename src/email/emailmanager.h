#ifndef EMAILMANAGER_H
#define EMAILMANAGER_H

#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include <QQueue>
#include <QString>
#include <QObject>

#include "cemailqueuethread.h"
#include "emailsender.h"

class EmailManager : public QObject
{
    Q_OBJECT
public:

    static EmailManager& getInstance();
    void InitEmailManager();
    void StopEmailTask();
    void AddEmailData(QStringList email_list);
    void ProcessEmailData(QStringList);
    friend class CEmailQueueThread;


signals:
    void SigStartSendEmail(QString,QString);

public slots:

private:
     EmailManager(QObject *parent = nullptr);
     EmailManager& operator=(const EmailManager &other);
     static QMutex mutex;
     static QScopedPointer<EmailManager>instance;
     friend struct QScopedPointerDeleter<EmailManager>;

     QWaitCondition m_waitCondition;
     QQueue<QStringList> m_qQueueMsg;
     CEmailQueueThread* email_thread;
     EmailSender *email_sender_t;
};

#endif // EMAILMANAGER_H
