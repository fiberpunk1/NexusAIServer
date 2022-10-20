#include <QDebug>
#include "cmessagequeuethread.h"
#include "watchermanager.h"
CMessageQueueThread::CMessageQueueThread():
    b_stop(false)
{

}

CMessageQueueThread::~CMessageQueueThread()
{

}

void CMessageQueueThread::run()
{
    WatcherManager* watcher = &WatcherManager::getInstance();
    while(!b_stop)
    {
        watcher->mutex.lock();
        //队列为空，等待
        while(!b_stop && watcher->m_qQueueMsg.empty())
        {
            watcher->m_waitCondition.wait(&watcher->mutex);
        }

        if(b_stop)
        {
            watcher->mutex.unlock();
            break;
        }

        //取出消息，并进行处理
        QString msg = watcher->m_qQueueMsg.dequeue();
        //fibernotice process msg
        qDebug()<<"run get msg:"<<msg;
//        watcher->SlotParseMessageData(msg);
        emit SignalQueueMsgData(msg);
        watcher->mutex.unlock();
//

    }

}

void CMessageQueueThread::stop()
{
    b_stop = true;
}
bool CMessageQueueThread::stopState()
{
    return b_stop;
}

