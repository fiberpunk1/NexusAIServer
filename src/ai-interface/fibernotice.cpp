#include <iostream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <QRegExp>
#include <QDebug>
#include <QFile>

#include <QSettings>

#include <QtMath>
#include <QSettings>
#include <QFileDialog>

#include "fibernotice.h"
#include "email/emailmanager.h"

using namespace cv;
using namespace cv::dnn;

FiberNotice::FiberNotice(QObject *parent) : QObject(parent)
{

}

void FiberNotice::Initinfer()
{
    QString yolo_tiny_model = "fiberpunk_detector_failed.weight";
    QString yolo_tiny_cfg = "fiberpunk-detector.cfg";

    net = readNetFromDarknet(yolo_tiny_cfg.toLatin1().data(), yolo_tiny_model.toLatin1().data());
    QSettings settings("./Config.ini",QSettings::IniFormat);
    settings.beginGroup("infer");
    QString infer_type = settings.value("infer_type").toString();
    settings.endGroup();
    if(infer_type.startsWith("Openvino"))
    {
        net.setPreferableBackend(DNN_BACKEND_INFERENCE_ENGINE);
    }
    else
    {
        net.setPreferableBackend(DNN_BACKEND_OPENCV);
    }

    net.setPreferableTarget(DNN_TARGET_CPU);
    outNames = net.getUnconnectedOutLayersNames();
    for (int i = 0; i < outNames.size(); i++)
    {
        qDebug()<<"out put layer name:"<<QString(outNames[i].c_str());
    }
    QString model_type = QFileInfo(yolo_tiny_cfg).baseName();

    ifstream classNamesFile("./3d.txt");
//    ifstream classNamesFile("./yolov4.txt");
    if (classNamesFile.is_open())
    {
        string className = "";
        while (std::getline(classNamesFile, className))
            classNamesVec.push_back(className);
    }
    qDebug()<<"Infer engine init finish!!";
}

QJsonObject FiberNotice::InferResult(Mat &frame)
{

    Mat inputBlob = blobFromImage(frame, 1 / 255.F, Size(608, 608), Scalar(), true, false);
    net.setInput(inputBlob);
    std::vector<Mat> outs;
    qDebug()<<"before forward calculate...";
    net.forward(outs, outNames);

    vector<double> layersTimings;
    double freq = getTickFrequency() / 1000;
//    double time = net.getPerfProfile(layersTimings) / freq;
//    ostringstream ss;
//    ss << " detection time: " << time << " ms";
//    putText(frame, ss.str(), Point(50, 50), 0, 1.5, Scalar(0, 0, 255),2);
    vector<Rect> boxes;
    vector<int> classIds;
    vector<float> confidences;
    vector<float> confidences_normaled;
    qDebug()<<"out size:"<<outs.size();
    for (size_t i = 0; i<outs.size(); ++i)
    {
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > 0.35)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
    }
    qDebug()<<"all result here:";
    vector<int> indices;
    double alpha = 0.5;
    NMSBoxes(boxes, confidences, 0.35, 0.2, indices);
    QJsonObject result_obj;
    int ind_count = indices.size();
    result_obj.insert("result_count",ind_count);

    qDebug()<<"the indices counts: "<<ind_count;
    QJsonArray result_arry;
    for (size_t i = 0; i < indices.size(); ++i)
    {
        QJsonObject result_obj;
        int idx = indices[i];
        Rect box = boxes[idx];
        float cfdence = confidences[idx];
        String className = classNamesVec[classIds[idx]];

        result_obj.insert("label","failed");
        result_obj.insert("confidence",cfdence);

        qDebug()<<"the idx:"<<idx<<",i="<<i;
        QJsonArray result_box;
        result_box.insert(0,box.x);
        result_box.insert(1,box.y);
        result_box.insert(2,box.width);
        result_box.insert(3,box.height);

        result_obj.insert("box",result_box);

        result_arry.insert(i, result_obj);

        rectangle(frame, box, Scalar(0, 0, 255), 1, LINE_AA, 0);

        int bls = 0;
        int blst = 0;
        int text_y = box.tl().y-8;
        int rect_x = (int)box.tl().x+70;
        if(rect_x>frame.cols)
            rect_x = frame.cols - 2;

        if(text_y<0)
            text_y = 0;

        if(box.tl().y<26)
            bls = box.tl().y;
        else
            bls = box.tl().y-25;

        if(box.tl().y<4)
            blst = box.tl().y;
        else
            blst = box.tl().y-3;

        if((box.tl().x>=0) && (box.tl().y>=0) && ((bls+25)<frame.rows) &&((box.tl().x+70)<frame.cols))
        {
            Mat roi = frame(Rect(box.tl().x, bls,70,25));
            cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 0));
            addWeighted(color,alpha,roi, 1.0-alpha,0.0, roi);
        }
        putText(frame, className.c_str(), Point(box.tl().x, text_y), FONT_HERSHEY_COMPLEX , 0.6, Scalar(255, 255, 255), 1, LINE_AA);
    }
    result_obj.insert("result",result_arry);
    QString result_img = tr("./web/final_result.jpg");

    // [0.35,1] map to [0.8,0.98]
    for(vector<float>::iterator it=confidences.begin(); it != confidences.end(); it++)
    {
        float tmp_cfd = *it;
        if(tmp_cfd>=0.35)
        {
            float new_value = 0.277*tmp_cfd + 0.703;
            confidences_normaled.push_back(new_value);
        }
    }

    if(confidences_normaled.size()>0)
    {
        std::vector<float>::iterator biggest_amsk = std::max_element(confidences_normaled.begin(),confidences_normaled.end());
        qDebug()<<"max confidence:"<<*biggest_amsk;
        float ret = *biggest_amsk;
        ostringstream ss;
        ss << "max confidence:"<<ret;

        Mat roi = frame(Rect(Point(45, 23),Point(500,60)));
        cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 0));
        addWeighted(color,alpha,roi, 1.0-alpha,0.0, roi);
        putText(frame, ss.str(), Point(50, 50), FONT_HERSHEY_COMPLEX , 1.0, Scalar(255, 255, 255),2,LINE_AA);
        QFile::remove(result_img);

        QSettings settings("./Config.ini",QSettings::IniFormat);
        settings.beginGroup("email");
        float threshold = (settings.value("confidence_threshold").toFloat());
        qDebug()<<"threshold value:"<<threshold;
        settings.endGroup();

        if(ret>threshold)
        {
            QStringList send_mail;
            send_mail<<result_img;
            send_mail<<"Nexus AI found some issue may cause print failed!";
            qDebug()<<"ret value:"<<ret;
            QDateTime time1 = QDateTime::currentDateTime();
            qint64 inter_min = last_send_time.secsTo(time1);
            qDebug()<<"last time:"<<inter_min;

            //检测失败的邮件，至少要3min发送一封，防止太过频繁
            if((inter_min>60))
            {
                EmailManager::getInstance().AddEmailData(send_mail);
                last_send_time = QDateTime::currentDateTime();
            }

        }
    }

    cv::imwrite(result_img.toStdString(), frame);
    return result_obj;
}
