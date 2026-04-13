#include "longconnectserver.h"
#include "QMessageBox"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QDateTime>
#include <QSerialPort>
#include "macro.h"
#include "utils.h"
#include "proxyworker.h"
#include "com/com.h"
#include "proxyworker.h"
#include "device_calibrator.h"
#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

#define api_time_year               QString("device_set_time_year")
#define api_time_month              QString("device_set_time_month")
#define api_time_day                QString("device_set_time_day")
#define api_time_hour               QString("device_set_time_hour")
#define api_time_min                QString("device_set_time_min")
#define api_time_sec                QString("device_set_time_second")





ConnectSocket::ConnectSocket(QObject* parent)
{
    m_BlockSize = 0;

//    connect(this,SIGNAL(readyRead()),this, SLOT(readMessage()));
//    connect(this,SIGNAL(disconnected()),this ,SLOT(disconnectSlot()));
 //   qDebug() << this << " created";
}


ConnectSocket::~ConnectSocket()
{
    // qDebug() << this << " deleted";
}


void ConnectSocket:: SetDescriptor(QTcpSocket* socket)
{
    m_sck = socket;
    connect(socket,SIGNAL(readyRead()),this, SLOT(readMessage()));
    connect(socket,SIGNAL(disconnected()),this ,SLOT(disconnectSlot()));
    qDebug() << "socket created :" << socket->socketDescriptor();
    m_descriptor = socket->socketDescriptor();
}


void ConnectSocket::readMessage()
{
    QString client_ip = m_sck->peerAddress().toString();

    // read data from socket
    qDebug() << "received messaged from " << client_ip;
    QByteArray data = m_sck->readAll();
    QString param = QString(data);
    param = param.replace("\r\n","");
    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(param.toUtf8());

    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << "jetson response:" << strJson;
    try
    {
        if(doc.isNull() || !doc.isObject()){
            qDebug() << "Invalid JSON format";
        }
        else
        {
            obj = doc.object();
            //qDebug()<<obj<<endl;
            int type = obj[report_type].toInt();
            if(type == report_det_result){ // detection result
                int nCamIndex = obj[camera_index].toInt();
                QString cam_index = QString("%1").arg(nCamIndex);
                int cross_type = obj[cross_type_key].toInt();
                int nWheels = obj[wheels_key].toInt();
#if 0
                QString clss = QString("%1").arg(obj[wheels_kinds].toString());
                QString _clss = "";
                for (int i = 0; i < clss.length(); i ++){
                    QChar chr = clss.at(i);
                    if (chr == '1')
                        _clss += "单";
                    else
                        _clss += "双";
                }
#else
                QString _clss = QString("%1").arg(obj[wheels_kinds].toInt());
                if(QString::compare(_clss, "0") == 0)
                    _clss = "";

#endif
                emit signal_received_data( client_ip, cam_index,  cross_type, nWheels, _clss, strJson);
                qDebug("success to receiv data from jetson");
            }
            else if(type == report_cur_state)// report current state
            {
                QString szTimeCalibed = obj["time_calibed"].toString();
                if(szTimeCalibed == "false"){ // should calib time of camera
                    QJsonObject jobj;

                    int y, M, d, h,m,s;
                    QDateTime date = QDateTime::currentDateTime();
                    date.date().getDate(&y, &M, &d);
                    h = date.time().hour();
                    m = date.time().minute();
                    s = date.time().second();

                    jobj[api_time_year] = y;
                    jobj[api_time_month] = M;
                    jobj[api_time_day] = d;
                    jobj[api_time_hour] = date.time().hour();
                    jobj[api_time_min] = date.time().minute();
                    jobj[api_time_sec] = date.time().second();

                    QJsonDocument timedoc(jobj);
                    QString timeResponse(timedoc.toJson(QJsonDocument::Compact));
                    sendMessage(timeResponse);
                }

                int index = obj[camera_index].toInt();
                emit signal_recevied_status(index, strJson);
                qDebug("success to status data from jetson");

            }
        }
        m_sck->disconnectFromHost();
        if (m_sck->state() == QAbstractSocket::UnconnectedState
            || m_sck->waitForDisconnected(1000)) {
                qDebug("Disconnected!");
        }
    } catch (...) {
        m_sck->disconnectFromHost();
        if (m_sck->state() == QAbstractSocket::UnconnectedState
            || m_sck->waitForDisconnected(1000)) {
                qDebug("Disconnected!");
        }
        log_file("error occured in setFortune server thread");
    }
}

void ConnectSocket::sendMessage(QString message)
{
    if(m_sck->state() == QAbstractSocket::ConnectedState)
    {
        m_sck->write(message.toLocal8Bit().constData()); //write the data itself
        m_sck->flush();
        m_sck->waitForBytesWritten(3000);
        qDebug() << "client write to buffer";
        return;

    }
    else
        return;
}

void ConnectSocket::disconnectSlot()
{
#if 1
     if(m_sck->state() == QAbstractSocket::ConnectedState)
     {
         m_sck->waitForDisconnected(3000);

     }
     emit disconnectedSignal(int(m_descriptor));
#else
    m_sck->disconnectFromHost();
    if (m_sck->state() == QAbstractSocket::UnconnectedState
        || m_sck->waitForDisconnected(1000)) {
            qDebug("Disconnected!");
    }
#endif
    qDebug()<<"disconnect client connection" << m_descriptor;
}

Server::Server(QObject *parent)
: QTcpServer(parent)
{
    m_bEnableThread = true;
    device_ip = "0.0.0.0";
    m_bStarted = false;
    connect(this, SIGNAL(newConnection()),
                this, SLOT(onNewConnection()));
}

/*
void Server::incomingConnection(long long descriptor)
{
    ConnectSocket *socket = new ConnectSocket(this);
    socket->setSocketDescriptor(descriptor);
    socket->SetDescriptor(descriptor);

    connect(socket, SIGNAL(disconnectedSignal(int)),this, SLOT(clientDisconnected(int)));

    connect(socket, &ConnectSocket::signal_received_data, [=](QString camIP, QString cam_index, int crossType, int nWheels, QString clss, QString szBuffer){
        emit signal_received_data(camIP,cam_index,crossType, nWheels, clss, szBuffer);
    });

    connect(socket, &ConnectSocket::signal_recevied_status, [=](int camIndex, QString szBuffer){
        emit signal_recevied_status(camIndex, szBuffer);
    });

    m_ClientDesMap[descriptor] = socket;
    QString clientIP = socket->peerAddress().toString(); ;
    qDebug() << "new incomming conneciton has been added" << clientIP <<"at " << server_port;
}*/



void Server::onNewConnection()
{
    // need to grab the socket
    QTcpSocket *sck = this->nextPendingConnection();

    if (!sck->waitForConnected(1000)){
        qDebug() << "Failed to connect to host";
        return ;
    }
    if(!sck )
        return;

    ConnectSocket *sckConnector = new ConnectSocket(this);

    connect(sckConnector, SIGNAL(disconnectedSignal(int)), this, SLOT(clientDisconnected(int)));

    connect(sckConnector, &ConnectSocket::signal_received_data, [=](QString camIP, QString cam_index, int crossType, int nWheels, QString clss, QString szBuffer){
        emit signal_received_data(camIP,cam_index,crossType, nWheels, clss, szBuffer);
    });

    connect(sckConnector, &ConnectSocket::signal_recevied_status, [=](int camIndex, QString szBuffer){
        emit signal_recevied_status(camIndex, szBuffer);
    });

    m_ClientDesMap[sck->socketDescriptor()] = sckConnector;
    sckConnector->SetDescriptor(sck);
    QString clientIP = sck->peerAddress().toString();
    qDebug() << "new incomming conneciton has been added" << sck->socketDescriptor() << clientIP <<"at " << server_port;

    calibrate_time(clientIP);

}

void   Server::onReceiveDetectionResult(QString camIP, QString cam_index, int crossType, int nWheels, QString clss, QString szBuffer){
    // check enable proxy?
    emit signal_received_data(camIP,cam_index,crossType, nWheels, clss, szBuffer);
}

void Server::clientDisconnected(int descriptor)
{

    if(!m_ClientDesMap.contains(descriptor)){
        qDebug() << "Invalid socket descripttor:" << descriptor;
        return;
    }
    m_ClientDesMap[descriptor]->deleteLater();
    m_ClientDesMap.remove(descriptor);
    qDebug() << "removed socket thread from server map:" << descriptor;
}

void Server::onstart(){
    QString ipAddress = GetSelfAddress();
    QHostAddress clientAdd = QHostAddress(server_ip);

    qDebug() << "creating socket server thread" << server_ip;
    QString msg = QString("creating socket server thread at %1").arg(server_ip);
    log_file(msg);
    connect(this, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(chatServerError(QAbstractSocket::SocketError)));
    quint16 pp = (quint16)server_port;
    if (!listen(clientAdd, pp)) { //QHostAddress::Any
        qDebug()<<QString("Unable to start the server ip = %1, error string: %2").arg(server_ip).arg(errorString());
        emit(signal_stop());
        return;
    }
    // this->setMaxPendingConnections(1);
    qDebug() <<QString("success to create server thread: ip = %1(%2)").arg(server_ip).arg(server_port);
    m_bStarted = true;
}


void Server::chatServerError(QAbstractSocket::SocketError socketError){
    if (socketError == QTcpSocket::RemoteHostClosedError)
        return;
   //  qDebug()<<"socket server error: "<<clientConnection->errorString();
   // clientConnection->close();
}

void Server::stopThread(){
    // disconnect all accepted connection

    for(auto iter = m_ClientDesMap.begin(); iter != m_ClientDesMap.end(); iter ++){
        ConnectSocket* pCnt = iter.value();
        qDebug() << "disconnect the current connections";
        pCnt->disconnect();
        pCnt->deleteLater();
    }

    // close all pending connection
#if 1
    while (hasPendingConnections()) {
        qDebug() << "close the pending connections";
        QTcpSocket* sck =  nextPendingConnection();
        sck->close();
    }
#endif
    close();
    qDebug() << "stop TCP Server: " <<server_ip;
}


void Server::calibrate_time(QString devcie_ip){
    // time calibration
    int period = 24 * 60 * 60 / HEART_BIT_RATE;
    std::string std_ip = devcie_ip.toStdString();
    QDateTime date = QDateTime::currentDateTime();

    int h = date.time().hour();
    int m = date.time().minute();
    int s = date.time().second();
    bool calib = false;

#if 1
    if(h == 3 && m == 0 && s < 35)
    {
        calib = true;
    }

    if(!calib
       && m_devices_calibed.contains(std_ip)
       && m_devices_calibed[std_ip] > 0 ){// this device is already calibrated
        m_devices_calibed[std_ip] += 1;
        if(m_devices_calibed[std_ip] < period){ //
            return;
        }
        m_devices_calibed[std_ip] = 0;
    }
#endif


    QThread* thread = new QThread();
    device_calibrator* _srch_rv = new device_calibrator(std_ip);
    _srch_rv ->moveToThread(thread);
    connect(thread, SIGNAL(started()), _srch_rv , SLOT(run()));
    connect(_srch_rv , SIGNAL(finished()), thread, SLOT(quit()));
    connect(_srch_rv , &device_calibrator::calibrated, [=](bool bSuccess, std::string ip){
        _srch_rv ->deleteLater();
        if(bSuccess){
            m_devices_calibed[ip] = 1;
        }
        else{
            m_devices_calibed[ip] = 0;
        }
    });
    thread->start();
}
