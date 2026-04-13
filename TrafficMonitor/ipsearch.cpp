#include "ipsearch.h"
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QUdpSocket>
#include <QThread>
#include <QTcpServer>
#include <QTextCodec>
#include <QNetworkAddressEntry>

ipsearch::ipsearch(quint16 server_port){
    _server_port = server_port;
    _bEnable = false;
    _queryData = "AML device Query";
}


void ipsearch::stopSearch(){
    _bEnable = false;
}

void ipsearch::run(){


    QString sendData = _queryData ;


    // _socket->bind(_server_port,  QUdpSocket::ShareAddress );
    QString selfIP = GetSelfAddress();
    qDebug() << "self ip address: "<< selfIP << ": " << "connection port" << _server_port;
    _bEnable = true;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    while(true){
        if(!_bEnable) // stop search
            break;



        for(int ipCnt = 0; ipCnt < ipAddressesList.size(); ipCnt ++){
            QString ip = ipAddressesList.at(ipCnt).toString();
            if (ip == "127.0.0.1")
                continue;
            QUdpSocket* _socket = new QUdpSocket(this);
            //bool bConnected = _socket->bind(_server_port,  QUdpSocket::ShareAddress );
            bool bConnected = _socket->bind(QHostAddress(ip), _server_port, QUdpSocket::ShareAddress );
            if(!bConnected){
                //qDebug() << "failed to bind udp socket";
                continue;
            }
            //qDebug() << "sucess to combine with " << ip;
            QByteArray dgram = _queryData.toStdString().c_str();
            //_socket->writeDatagram(buffer, nLen, QHostAddress::Broadcast,_server_port);
            //qint64 nWritten = _socket->writeDatagram(dgram.data(), dgram.size(), QHostAddress::Broadcast, _server_port);
            qint64 nWritten = _socket->writeDatagram(dgram.data(), dgram.size(), QHostAddress("255.255.255.255"), _server_port);
            QThread::msleep(100);

            if (nWritten < 0)
                continue;
            QHostAddress ip6Address;
            while(_socket->hasPendingDatagrams()) //拥有等待的数据报
            {
                if(!_bEnable)
                    break;
                QByteArray datagram; //拥于存放接收的数据报
                datagram.resize(int(_socket->pendingDatagramSize()));
                _socket->readDatagram(datagram.data(),datagram.size(), &ip6Address);

                QString data = QString::fromUtf8(datagram.data());

                bool conversionOK;
                QHostAddress ip4Address(ip6Address.toIPv4Address(&conversionOK));
                if(conversionOK){
                    QString ip4String = ip4Address.toString();

                    if(data != _queryData && ip4String != ip){ // get the device ip
                        // send report
                        qDebug() << ip4String  << data;
                        emit(foundDeviceIP(ip4String, data));
                    }
                }
            }
            _socket->close();
        }
    }
    emit(finished());
}



