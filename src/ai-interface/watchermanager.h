#ifndef WATCHERMANAGER_H
#define WATCHERMANAGER_H

#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include <QQueue>
#include <QString>

#include "cmessagequeuethread.h"
#include "fibernotice.h"


class WatcherManager : public QObject
{
    Q_OBJECT
public:
    static WatcherManager& getInstance();
    void InitMsg();
    void StopMsgTask();
    void AddMsgData(QString);
    friend class CMessageQueueThread;
    QString cpu_info;
private:
    WatcherManager(QObject *parent = nullptr);
    WatcherManager& operator=(const WatcherManager &other);
    static QMutex mutex;
    static QScopedPointer<WatcherManager>instance;
    friend struct QScopedPointerDeleter<WatcherManager>;


    QWaitCondition m_waitCondition;
    QQueue<QString> m_qQueueMsg;
    CMessageQueueThread* msg_thread;
    FiberNotice* fiber_interface;

signals:
    void SigInferResult(QString,QString,float); //camera_name, file_name, confidence value
    void SigInferJson(QJsonObject);

public slots:
    void SlotParseMessageData(QString msg);
};

#endif // WATCHERMANAGER_H
