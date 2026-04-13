#include <QtDebug>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "utils.h"
#include "proxyworker.h"


proxyWorker::proxyWorker(QObject* parent):
    QObject(parent)
{

}

// send socket data
void proxyWorker::run(){

    QTcpSocket* socket = new QTcpSocket(this);

#if 0
    connect(socket, &QTcpSocket::connected, [=](){}); // connected
    connect(socket, &QTcpSocket::disconnected, [=](){
        qDebug() << "disconnected...";
        socket->disconnectFromHost();
    });
    connect(socket, &QTcpSocket::bytesWritten, [=](qint64 bytes){
        qDebug() << bytes << " bytes written...";
    });

    connect(socket, &QTcpSocket::readyRead,[=](){});
#endif
    socket->connectToHost(m_serverIP, m_serverPort);
    // we need to wait...

    if(!socket->waitForConnected(2000))
    {
        qDebug()<<"Failed to conect to socket server";
        emit sentBuffer(m_szBuffer, m_serverIP, m_serverPort, false);
        return;
    }
#if 0
    QJsonObject jobj;
    jobj[report_type] = "0";
    jobj[camera_index] = "0";
    jobj[cross_type_key] = "1";
    jobj[wheels_key] = QString("%1").arg(m_nWheel);
    jobj[wheels_kinds] = m_szWheelType;
    // jobj[camera_ip_key] = m_clientIP;

    QJsonDocument doc(jobj);
    QString strJson(doc.toJson(QJsonDocument::Compact));

    qDebug()<<strJson;
    socket->write(strJson.toUtf8().constData());
#else
    socket->write(m_szBuffer.toUtf8().constData());
#endif
    // socket->flush();
    if(!socket->waitForBytesWritten()) {
        emit sentBuffer(m_szBuffer, m_serverIP, m_serverPort, false);
        return;
    }
    else{
        emit sentBuffer(m_szBuffer, m_serverIP, m_serverPort, true);
        qDebug() << "success to send data";
    }
}

