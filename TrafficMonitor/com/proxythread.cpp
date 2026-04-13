#include "proxythread.h"
#include <QDebug>
#include <QThread>
proxyThread::proxyThread(QObject* parent):
    QObject(parent)
{
    m_bEnable = true;
    m_bCloseComportrequest = false;
}

proxyThread::~proxyThread(){

}


void            proxyThread::openComport(QString portName){
    qDebug() << "port Name " << portName;

    if(m_handleMap.contains(portName)){
         m_handleMap[portName]->open(portName);
    }
    else{
        Com* handle = new Com();
        handle->open(portName);
        m_mutex.lock();
        m_handleMap[portName] = handle;
        m_mutex.unlock();
    }

}

void    proxyThread::lockComport(QString portName){
    m_mutex.lock();
    m_closedPortName = portName;
    m_bCloseComportrequest = true;
    m_mutex.unlock();
}

void    proxyThread::unlockComport(){
    m_mutex.lock();
    m_closedPortName = "";
    m_mutex.unlock();
}

void            proxyThread::addData(QString camIP, QString portName, int nWheel, int ntype){
    m_mutexData.lock();
    DET_RESULT one(camIP, portName, nWheel, ntype);
    m_tasks.enqueue(one);
    m_mutexData.unlock();
}


bool  proxyThread:: popData(DET_RESULT& one){
    bool bSuccess = false;
    m_mutexData.lock();
    if(m_tasks.size() > 0){
        one = m_tasks.dequeue();
        bSuccess = true;
    }
    m_mutexData.unlock();
    return bSuccess;
}


void            proxyThread::stopThread(){
     m_mutex.lock();
     m_bEnable = false;
     m_mutex.unlock();
}

void      proxyThread:: closeAllPorts(){
    QMap<QString, Com*>::iterator iter;

    for(iter = m_handleMap.begin(); iter != m_handleMap.end(); iter ++){
        Com* handle = iter.value();
        handle->close();
        handle->deleteLater();
    }
    m_handleMap.clear();
}

void      proxyThread::sendData(DET_RESULT one){
    QString portName = one.portName;
    if(portName == m_closedPortName)
        return;
    int nW = one.nWheel;
    int nT = one.nType;
    QString szBuffer;
    //m_lockSeral.lockForRead();
    if(!m_handleMap.contains(portName)){
        openComport(portName);
    }
    if(!m_handleMap[portName]->isOpened()){
        m_handleMap[portName]->open(portName);
    }
    szBuffer = m_handleMap[portName]->sendWheelData(nW, nT);
    qDebug() << "emit proxed result "<< portName << szBuffer;
    emit proxiedData(one.camIP, portName, nW, nT, szBuffer);
}

void proxyThread::process(){
    qDebug()<< "start proxy thread";
    DET_RESULT one;
    while(true){
        if(!m_bEnable)
        {
            qDebug()<< "stop proxy thread";
            break;
        }
        if(m_bCloseComportrequest){
            if(m_handleMap.contains(m_closedPortName)){
                m_handleMap[m_closedPortName]->close();
            }
            emit comportClosed();
            m_bCloseComportrequest = false;
        }
        if(!popData(one))
        {
            QThread::msleep(100);
            continue;
        }
        sendData(one);
        QThread::msleep(100);
        continue;
    }

    closeAllPorts();
    emit finished();
}
