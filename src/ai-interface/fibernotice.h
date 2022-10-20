#ifndef FIBERNOTICE_H
#define FIBERNOTICE_H

#include <QObject>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>

#include "opencv2/opencv.hpp"
#include "opencv2/dnn.hpp"

using namespace std;


class FiberNotice : public QObject
{
    Q_OBJECT
public:
    explicit FiberNotice(QObject *parent = nullptr);
    void Initinfer();
    QJsonObject InferResult(cv::Mat &img);
private:
    std::vector<cv::String> outNames;
    cv::dnn::Net net;
    vector<string> classNamesVec;
    QDateTime last_send_time;

public slots:
};

#endif // FIBERNOTICE_H
