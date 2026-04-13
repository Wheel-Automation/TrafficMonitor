#ifndef PROXYTHREAD_H
#define PROXYTHREAD_H

#include <QObject>
#include <QReadWriteLock>
#include <QQueue>
#include <QString>
#include <QMap>
#include <utils.h>
#include "com/com.h"
#include <QMutex>

struct DET_RESULT{
    QString camIP;
    QString portName;
    int nWheel;
    int nType;
    DET_RESULT(){
        camIP = "0.0.0.0";
        portName = "COM3"    ;
        nWheel = 6;
        nType = 122222;
    }

    DET_RESULT(QString ip, QString port, int nW, int nT){
        camIP = ip;
        portName = port;
        nWheel = nW;
        nType = nT;
    }

};

class proxyThread: public QObject
{
    Q_OBJECT

public:
    proxyThread(QObject* parent = nullptr);
    ~proxyThread();


    void            openComport(QString portName);
    void            lockComport(QString portName);
    void            unlockComport();
    void            addData(QString camIP, QString portName, int nWheel, int ntype);
    void            stopThread();

signals:
    void            proxiedData(QString camIP, QString comPort, int nW, int nT, QString szBuffer);
    void            finished();
    void            comportClosed();



public slots:
    void process();

private:
    bool            popData(DET_RESULT& one);

    void            closeAllPorts();
    void            sendData(DET_RESULT one);
private:
    bool            m_bEnable;
    bool            m_bCloseComportrequest;
    QString         m_closedPortName;


    QMap<QString, Com*>     m_handleMap;
    QQueue<DET_RESULT>     m_tasks;

    QMutex                  m_mutex;
    QMutex                  m_mutexData;

};

#endif // PROXYTHREAD_H
