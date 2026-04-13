#ifndef IPSEARCH_H
#define IPSEARCH_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QUdpSocket>
#include "utils.h"

// socket connection port to search device
#define DEVIDE_SEARCH_PORT 51089

class ipsearch : public QObject
{
    Q_OBJECT
public:
    ipsearch(quint16 server_port);


signals:
    void finished();
    // report search result
    void foundDeviceIP(QString ip, QString deviceName);

public slots:
    void stopSearch();
    void run();



private:
    bool _bEnable;

    QString _szCurrentIP;
    QString _queryData;
private:

    quint16             _server_port;
    //QUdpSocket*     _socket;
};

#endif // IPSEARCH_H
