#include "camViewerWorker.h"
#include "mainwindow.h"

#include <QDebug>
#include <QThread>
#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif
#include "macro.h"
#include "mainwindow.h"
#include "utils.h"

camViewerWorker::camViewerWorker(QObject *parent):
    QObject(parent),
    _buffer(nullptr)
{
    m_benable_thread = true;
    m_szRtspUrl = QString("");
    m_devType = DEVTYPE;
}

void camViewerWorker::process(){
    qDebug() << "start camera worker thread";
    std::string cam_url;

    cv::Mat frame, tempfrm;
    bool res = false;
    int frameW, frameH;
    bool bCameraOpend = true;


    // open new url
    cam_url = get_cam_url();
    QString curl = QString::fromStdString(cam_url);
    qDebug() <<"opening rtsp: "<<curl;
    cap = cv::VideoCapture(cam_url);
    if (!cap.isOpened()){
        qDebug("failed to open rtsp:");
        bCameraOpend = false;
        emit rtspConnectFailed();
        releaseResource();
        return;
    }
    emit connected(true);

    // allocate frame buffer
    if(_buffer != nullptr){
        delete [] _buffer;
        _buffer = nullptr;
    }
    frameW = int(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    frameH = int(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    getResizeScale(frameW, frameH, 480);
    _buffer = new uchar[frameW * frameH * 3];
    emit(start_display());
    bCameraOpend = true;

    while(true){
        try {
            if(!isEnabled()){
                qDebug("rtsp stream disabled, breaking...");
                break;
            }

            res = cap.read(frame);
            if(!res || frame.empty()){
                cap.release();
                bCameraOpend = false;
                emit rtspConnectFailed();
                break;
            }

            cv::resize(frame, tempfrm, cv::Size(frameW, frameH));
            cv::cvtColor(tempfrm, tempfrm, cv::COLOR_BGR2RGB);
            memcpy(_buffer, tempfrm.data, frameW * frameH * 3);
            emit signal_one_frame(_buffer, frameW, frameH, 3);
            QThread::msleep(3);

        } catch (...) {
            qDebug()<< "exception at rtsp error";
            break;
        }
    }
    releaseResource();
    qDebug() << "exiting rtsp camera thread";
}

void    camViewerWorker::getResizeScale(int& w, int & h, const int maxHeight){
    float scale = maxHeight / float(h);
    if (scale > 1.0f)
        return;
    w = int(w * scale);
    w = ((w + 3) / 4) * 4;
    h = int(h * scale);
    return;
}


void camViewerWorker::releaseResource(){
    if(_buffer)
        delete[] _buffer;
    _buffer = nullptr;
    if(cap.isOpened())
        cap.release();
    emit(finished());
}

//rtskp://admin:1q2w3e4r@192.168.101.50:554/Streaming/Channels/101
std::string  camViewerWorker::get_cam_url(){
    QString url;
    if(m_devType == e_dev_jetson)
        url = QString("rtsp://%1/fengqi.wheel.live.sdp").arg(m_camIP);
    else if(m_devType == e_dev_c308x){ // c308x
        url = QString("rtsp://%1/fengqi.wheel.live.sdp").arg(m_camIP);
    }
    else if(m_devType == e_dev_rv1126){ // rockchip(瑞思微)
        url = QString("rtsp://%1:554/stream/av0_0").arg(m_camIP);
    }
    else if(m_devType == e_dev_hisi3519){ // HiSi3519V500
        url = QString("rtsp://%1:8554/1").arg(m_camIP);
    }
    else{
        qDebug() << "Failed to open camera url. Unsupported device type";
    }
    std::string std_url = url.toLocal8Bit().constData();
    return std_url;
}

void camViewerWorker::stop_working()
{
    qDebug() << "kill camera thread by signal";
    m_lockEnable.lockForWrite();
    m_benable_thread = false;
    m_lockEnable.unlock();
}

void    camViewerWorker::pause(){
    m_lockEnable.lockForWrite();
    m_benable_thread = false;
    m_lockEnable.unlock();
}

void    camViewerWorker::setDeviceType(int deviceType)
{
    m_devType = deviceType;
}


void    camViewerWorker::setCameraIP(QString ip){
    m_camIP = ip;
}

bool    camViewerWorker::isEnabled(){
    return m_benable_thread;
}



