#include <QDebug>
#include "device_serach_worker.h"
#include "NetClient.h"
#include "NetLib.h"
device_serach_worker::device_serach_worker(int timeout, QObject* parent): 
	_searchTimeout(timeout), 
	QObject(parent)
{

}

void  device_serach_worker::stopSearch(){
    _bEnable = false;
}


void device_serach_worker::run(){
    long handle = InitClientLib();
    if(handle <= 0){
        qDebug() << "failed to initialize client library" << endl;
        emit(finished());
        return;
    }

    local_info_t m_devInfoList[100] = {0}; //Save List;
    local_info_t searchList[100] = {0};
    bool bSame = false;
    int nAllDevCnt = 0;
	
	int nDevNum = SearchDeviceInfo(searchList, 100, _searchTimeout, NULL);
    // printf("Search Online Device Count：%d!\n", nDevNum);
    for (int i = 0; i < nDevNum; i++)
    {
        //打印设备信息;
        QString msg = QString("index:[%1],Ip:%2, port:%3, mac:%4, serial:%5,devType:%6,uth_flag:%7,"
            "led_type:%8, cam_count:%9!!!1\n")
                .arg(i)
                .arg(searchList[i].local_ip)
                .arg(searchList[i].local_port)
                .arg(searchList[i].mac_addr)
                .arg(searchList[i].serial_info)
                .arg(searchList [i].device_type)
                .arg(searchList [i].auth_flag)
                .arg(searchList [i].led_type)
                .arg(searchList [i].cam_count);
        //保存设备信息，过滤掉已保存（或重复相应）的设备IP,加入保存列表；
        QString ip = QString::fromUtf8(searchList[i].local_ip);
        QString devname = QString::fromUtf8(searchList[i].serial_info);
		qDebug() << ip << devname; 
         emit(foundDeviceIP(ip, devname));
    }
    emit(finished());
    DeInitClientLib(handle);
    qDebug() << "rockchip device search finished";
}


