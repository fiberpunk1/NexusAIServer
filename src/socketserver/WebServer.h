#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

extern "C"{
#include "mongoose.h"
}
#include <QObject>
#include <QByteArray>
#include <thread>
#include <atomic>

#include "../ai-interface/fibernotice.h"

// 这里为了将来不使用Qt的时候比较方便,所以没有使用QT的定时器来做轮询
// 而是采用了std的线程.
// solym@sohu.com

class WebServer: public QObject
{
    Q_OBJECT

    WebServer(QObject *parent = 0);
public:
    ~WebServer();
    // 获取单例对象
    static WebServer* getInstance();

    void init();
    // 启动服务器
    void start();
    // 停止服务器
    void stop();
    // 重启服务器
    void reStart();

private:
    // 服务器事件轮询
    void doServerPoll();

    // Mongoose事件处理函数
    static void ev_handler(struct mg_connection* nc,int ev,void* p, void* fn_data);

    // 单例对象指针
    static WebServer*       instanceServe;


    unsigned short          _port;  ///< 使用的端口号
    char*                   _root;  ///< Web根目录
    bool                    _dirList;

    struct mg_mgr           _mgr;   ///< Mongoose 事件管理对象
    struct mg_connection*   _pnc;   ///< Mongoose 连接器对象
    std::thread*            _thr;   ///< Mongoose 运行在的线程管理对象
    std::atomic_bool        _thrIsRun;  ///< 线程执行操控

signals:
    void SendCmd(QString);
};

#endif // WEBSERVER_HPP
