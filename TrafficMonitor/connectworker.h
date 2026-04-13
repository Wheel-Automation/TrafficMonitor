#ifndef CONNECTWORKER_H
#define CONNECTWORKER_H

#include <QObject>
#include <QString>
#include <QMap>
#include "utils.h"
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QReadWriteLock>

class connectWorker: public QObject
{
    Q_OBJECT
public:
    explicit connectWorker(PANEL_PARM param, SETTING setting, int connect_type, QObject* parent = nullptr);
    void stop_connect(){_enable = false; }
    void setProxyMap(QMap<QString, PROXY_SETTING> proxyMap){m_proxyMap = proxyMap;}
signals:
    void finished();
    void report(int, int, PANEL_PARM, SETTING); //

public slots:
    void connected();
    void process();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();


private:
    void process_connect();
    void doConnect();

private:
    bool _enable;
    QHostAddress _szip;
    QString _szpwd;
    PANEL_PARM _panel;
    QMap<QString, PROXY_SETTING>    m_proxyMap;
    SETTING     _setting;
    int         _connect_type;
    QTcpSocket  *_socket;

};

#endif // CONNECTWORKER_H
