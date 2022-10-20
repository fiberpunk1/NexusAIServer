#ifndef CMESSAGEQUEUETHREAD_H
#define CMESSAGEQUEUETHREAD_H

#include <QThread>
#include <QObject>

class CMessageQueueThread : public QThread
{
    Q_OBJECT
public:
    CMessageQueueThread();
    ~CMessageQueueThread();

    void run();
    void stop();
    bool stopState();

private:
    bool b_stop;

signals:
    void SignalQueueMsgData(const QString & msg);
};

#endif // CMESSAGEQUEUETHREAD_H
