#ifndef DEVICE_CALIBRATOR_H
#define DEVICE_CALIBRATOR_H


#include <QObject>
#include <QString>
#include "NetClient.h"
#include "NetLib.h"

class device_calibrator: public QObject
{
    Q_OBJECT
public:
    explicit device_calibrator(std::string dev_ip, QObject*  parent = nullptr);
signals:
    void finished();
    void calibrated( bool bsuccess, std::string device_ip = "");

public slots:
    void run();

private:
    bool                login_device();
    void                logout_device();
    bool                is_correct_time(net_time_t& dev_time);

private:
    std::string         _dev_ip;
    long                _g_handle;


};


#endif // DEVICE_CALIBRATOR_H
