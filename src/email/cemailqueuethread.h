#ifndef CEMAILQUEUETHREAD_H
#define CEMAILQUEUETHREAD_H

#include <QThread>
#include <QObject>

class CEmailQueueThread : public QThread
{
    Q_OBJECT
public:
    CEmailQueueThread();
    ~CEmailQueueThread();

    void run();
    void stop();
    bool stopState();
private:
    bool b_stop;
signals:
    void SigQueueEmailData(const QStringList & email);
};

#endif // CEMAILQUEUETHREAD_H
