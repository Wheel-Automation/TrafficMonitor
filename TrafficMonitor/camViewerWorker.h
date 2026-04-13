#ifndef CAMVIEWERWORKER_H
#define CAMVIEWERWORKER_H

#include <QObject>
#include <QString>
#include <QWaitCondition>
#include "utils.h"
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class camViewerWorker : public QObject
{
    Q_OBJECT
public:
    explicit camViewerWorker(QObject *parent = nullptr);
    void    setCameraIP(std::string ip);

signals:
    void    signal_one_frame(uchar* frame, int W, int H, int chn);
    void    finished();  // process finished
    void    connected(bool);
    void    start_display();
    void    rtspConnectFailed();   // failed to connect rtsp stream;

public slots:    
    void    process();

public:
    // set the camera device type, used to determine rtsp url
    void    setDeviceType(int deviceType);

    void    setCameraIP(QString ip);  // change camera url

    bool    is_running(){return m_benable_thread; }
    void    stop_working();
    void    pause();

private:
    void    releaseResource();
    std::string     get_cam_url();
    void    getResizeScale(int& w, int & h, const int maxHeight = 480);
    uchar*  _buffer;
    bool    isEnabled();

private:
    QReadWriteLock m_lockEnable;
    QString     m_szRtspUrl;
    bool        m_bUrlChanged;
    bool        m_benable_thread;
    cv::VideoCapture cap;
    QString         m_camIP;
    int             m_devType;
};

#endif // CAMVIEWERWORKER_H
