#include <string>
#include <QDebug>
#include <QDateTime>
#include "NetClient.h"
#include "NetLib.h"
#include "macro.h"
#include "device_calibrator.h"


device_calibrator::device_calibrator(std::string dev_ip, QObject* parent):
  QObject(parent),
  _dev_ip(dev_ip),
  _g_handle(0)
{
    _g_handle = 0;
}



void device_calibrator::run(){

     _g_handle = InitClientLib();
    if(_g_handle <= 0){
        qDebug() << "failed to initialize client library" << endl;
        emit(calibrated(false, _dev_ip));
        emit(finished());
        return;
    }

    bool result = false;
    for(int i = 0; i < 1; i ++){
        if(!login_device()){
            qDebug("failed to login device");
            break;
        }

        // NETCLIENT_API BOOL NETCLIENT_CALL GetDevTime(long lHandle, net_time_t *pDevTime);
        // NETCLIENT_API BOOL NETCLIENT_CALL SetDevTime(long lHandle, net_time_t *pDevTime);
        net_time_t dev_time;
        if(!GetDevTime(_g_handle, &dev_time)){
            qDebug()<< "failed to get device time";
            break;
        }

        if(is_correct_time(dev_time)) { // this device is already calibrated
            qDebug()<< "already calibrated";
            result = true;
            break;
        }
        if(SetDevTime(_g_handle, &dev_time)){
            qDebug()<< "success to set device time";
            result = true;
            break;
        }
    }
    logout_device();
    DeInitClientLib(_g_handle);
    emit(calibrated(result, _dev_ip));
    emit(finished());
    qDebug() << "rockchip device set time finished";
}

bool    device_calibrator:: is_correct_time(net_time_t& dev_time){
    QDateTime date = QDateTime::currentDateTime();
    int y, m, d, h, M, s;
    y = date.date().year();
    m= date.date().month();
    d = date.date().day();
    h = date.time().hour();
    M = date.time().minute();
    s = date.time().second();
    if(y == dev_time.year
       && m == dev_time.mon
       && d == dev_time.day
       && h == dev_time.hour
       && M == dev_time.minute
       ){                   // correct dev time. no need to caliba
        return true;
    }
    dev_time.year = y;
    dev_time.mon = m;
    dev_time.day = d;
    dev_time.hour = h;
    dev_time.minute = M;
    dev_time.sec = s;
    return false;
}


bool    device_calibrator:: login_device(){
    if(_g_handle <= 0){
        qDebug() << "invalid library handle";
        return false;
    }
    bool result = false;
    char szSvrIp[20] = {0,};
    std::string ss = _dev_ip;
    strcpy_s(szSvrIp, ss.c_str());

    int nPort = ROCKCHIP_PORT;
    char szUser[20] = "admin";
    char szPwd[20] = "123456";
    NetworkConf_t netInfo;
    memset(&netInfo, 0, sizeof(netInfo));
    int nRet = Login(_g_handle, szSvrIp, nPort, szUser, szPwd);
    result = (nRet >= 0)? true: false;
    return result;
}

void device_calibrator:: logout_device(){
    if(_g_handle <= 0)
    {
        qDebug() << "invalid library handle";
        return;
    }
    Logout(_g_handle);
}
