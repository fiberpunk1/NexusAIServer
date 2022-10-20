#include <QDebug>
#include "cemailqueuethread.h"
#include "emailmanager.h"

CEmailQueueThread::CEmailQueueThread()
{
    b_stop = false;
}

CEmailQueueThread::~CEmailQueueThread()
{

}

void CEmailQueueThread::run()
{
    EmailManager* email_man = &EmailManager::getInstance();
    while(!b_stop)
    {
        email_man->mutex.lock();
        //队列为空，等待
        while(!b_stop && email_man->m_qQueueMsg.empty())
        {
            email_man->m_waitCondition.wait(&email_man->mutex);
        }

        if(b_stop)
        {
            email_man->mutex.unlock();
            break;
        }

        //取出消息，并进行处理
        QStringList msg = email_man->m_qQueueMsg.dequeue();
        qDebug()<<"email queue thread get:"<<msg;
//        email_man->SlotParseMessageData(msg);
        email_man->ProcessEmailData(msg);
        email_man->mutex.unlock();
    }
}

void CEmailQueueThread::stop()
{
    b_stop = true;
}

bool CEmailQueueThread::stopState()
{
    return b_stop;
}
