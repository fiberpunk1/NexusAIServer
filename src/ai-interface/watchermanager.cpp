#include <QDebug>
#include <QFileInfo>
#include "watchermanager.h"


QMutex WatcherManager::mutex;
QScopedPointer<WatcherManager> WatcherManager::instance;

WatcherManager::WatcherManager(QObject *parent) : QObject(parent)
{
    msg_thread = nullptr;
    fiber_interface = nullptr;
}

WatcherManager& WatcherManager::getInstance()
{
    if(instance.isNull())
    {
        mutex.lock();
        if(instance.isNull())
        {
            instance.reset(new WatcherManager());
        }
        mutex.unlock();
    }
    return *instance.data();
}

void WatcherManager::InitMsg()
{
    msg_thread = new CMessageQueueThread();
    fiber_interface = new FiberNotice();

    connect(msg_thread, &CMessageQueueThread::SignalQueueMsgData,
            this, &WatcherManager::SlotParseMessageData, Qt::BlockingQueuedConnection);

    //loading model
    fiber_interface->Initinfer();

    msg_thread->start();

}
void WatcherManager::StopMsgTask()
{
    if(!msg_thread || msg_thread->stopState())
        return;

    if(msg_thread)
        msg_thread->stop();
    m_waitCondition.notify_all();
    m_qQueueMsg.clear();
    msg_thread->deleteLater();
    msg_thread = nullptr;
    qDebug()<<"CMainControl::stop!!";
}
void WatcherManager::SlotParseMessageData(QString msg)
{
    qDebug()<<"hello, i get msg:"<<msg;
    QFileInfo info(msg);
    QString name = info.baseName();
    qDebug()<<info.baseName();
    qDebug()<<info.fileName();
    qDebug()<<info.filePath();
    //    qDebug()<<info.absoluteFilePath();
    cv::Mat infer_mat = cv::imread(info.absoluteFilePath().toStdString(), 1);
    if(infer_mat.empty())
    {
        qDebug()<<"Mat Load image failed!";
    }
    else
    {
        QJsonObject result = fiber_interface->InferResult(infer_mat);
        QString result_img = tr("./infer_result.jpg");
        cv::imwrite(result_img.toStdString(), infer_mat);
        //            emit SigInferResult(name, result_img, confidence);

    }

}

void WatcherManager::AddMsgData(QString msg)
{
    mutex.lock();
    qDebug()<<"add message";
    m_qQueueMsg.enqueue(msg);
    m_waitCondition.notify_all();
    mutex.unlock();
}
