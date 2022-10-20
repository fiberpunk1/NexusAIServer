#include <QString>
#include <QDebug>
#include <QObject>
#include <QImage>

#include <QFile>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "WebServer.h"

#include "opencv2/opencv.hpp"
#include "opencv2/dnn.hpp"


#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#endif
#ifdef __linux__
#define SLEEP(ms) usleep(ms*1000)
#endif

static const char *s_http_addr = "http://0.0.0.0:8002";    // HTTP port
WebServer* WebServer::instanceServe = NULL;
//static const char *s_root_dir = "C:\\Users\\admin\\Desktop\\ESP32-Wifi\\esp32-controler\\build-NexusAI-Desktop_Qt_5_9_7_MSVC2017_64bit-Release\\web";
static const char *s_root_dir = "web";
static FiberNotice* fiber_infer = new FiberNotice();
static struct mg_http_serve_opts opts;

WebServer::WebServer(QObject *parent):QObject(parent)
{
    _root = strdup(".");
    //    :_thrIsRun(false),_thr(NULL),
    _thrIsRun = false;
    _thr = NULL;
    _port = 8002;
    _dirList = true;

}

WebServer::~WebServer()
{
    this->stop();
}


WebServer* WebServer::getInstance()
{
    if(instanceServe == NULL){
        instanceServe = new WebServer;

    }
    return instanceServe;
}

void WebServer::init()
{
    fiber_infer = new FiberNotice();
    fiber_infer->Initinfer();
}

void WebServer::start()
{
    if(!_thrIsRun)
    {
        _thrIsRun = true;
        if(_thr != NULL)
        {
            delete _thr;
        }
        // 设置线程
        _thr = new std::thread([this](){this->doServerPoll();});
        _thr->detach();
    }
}

void WebServer::stop()
{
    if(_thrIsRun){
        _thrIsRun = false;
    }
}

void WebServer::reStart()
{
    this->stop();
    this->start();
}

void WebServer::ev_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        if(mg_http_match_uri(hm, "/upload"))
        {
            struct mg_http_part part;
            size_t ofs = 0;
            QByteArray img_data;
            while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0)
            {
                qDebug()<<tr("get body len:%1").arg(part.body.len);
                QByteArray t = QByteArray::fromRawData(part.body.ptr,part.body.len);
                img_data.append(t);
            }
            QImage img;
            bool t = img.loadFromData(img_data);
            if(!img.isNull() && t)
            {
                qDebug()<<"send finish!";
                img.save("upload_infer.jpg","JPEG",100);
                cv::Mat infer_input = cv::imread("upload_infer.jpg", 1);

                QJsonObject ret = fiber_infer->InferResult(infer_input);
                QJsonDocument doc(ret);
                QByteArray json = doc.toJson();
//                QJsonObject rets = fiber_infer
                mg_http_reply(c, 200, "", json.data());
            }
            else
            {
                mg_http_reply(c, 500, "image broke", "try again");
            }

        }
        else
        {

            opts.root_dir = s_root_dir;
            qDebug()<<"before set server dir";  //这一行去掉了，就无法返回网页。。
            mg_http_serve_dir(c, hm, &opts);
            qDebug()<<"set server dir";
        }
    }
//    (void) fn_data;
}


void WebServer::doServerPoll()
{
    puts("start======");
    mg_log_set("3");                              // Set to 3 to enable debug
    mg_mgr_init(&_mgr);                            // Initialise event manager
    mg_http_listen(&_mgr, s_http_addr, WebServer::ev_handler, NULL);  // Create HTTP listener

    // 进行轮询
    while(_thrIsRun)
    {
        //进行轮询,超时等待1000ms
        mg_mgr_poll(&_mgr,1000);
    }
    // 释放资源
    mg_mgr_free(&_mgr);
    puts("stop======");
}


