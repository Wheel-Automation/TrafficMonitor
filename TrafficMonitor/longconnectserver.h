//server.h
#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QtGui>
#include <QtNetwork>
#include <QtCore>
#include <QObject>
#include <QThreadPool>
#include <utils.h>
#include <QReadWriteLock>
#include <QMap>
#include <QMutex>

class ConnectSocket :  public QObject
{
    Q_OBJECT
public:
    ConnectSocket(QObject* parent=0);
    ~ConnectSocket();
    void SetDescriptor(QTcpSocket* sck) ;

signals:
    void    disconnectedSignal(int);
    void    signal_received_data(QString camIP, QString camIndex, int crossType, int nWheels, QString clss, QString szBuffer);
    void    signal_recevied_status(int cam_index, QString szBuffer);


private slots:
    void readMessage();
    void sendMessage(QString message);
    void disconnectSlot();

private:
    //网络断开后，描述符会立即变成－1，所以要先保存一下
    QTcpSocket* m_sck;
    qintptr         m_descriptor ;
    QString m_String;
    quint16 m_BlockSize;
};

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject *parent = nullptr);

    void            stopThread();
    QString         GetServerIP(){ return server_ip; }
    qint16          GetServerPort(){ return server_port; }
    void            SetServerPort(uint16_t nPort){server_port = nPort; }
    void            SetServerIP(QString ip){server_ip = ip;}
    bool            isStarted(){return m_bStarted;}
    void            setSocket(QTcpSocket*  sck);
protected:
    void            calibrate_time(QString device_ip);

private:
    QMap<qintptr, ConnectSocket*> m_ClientDesMap;


signals:
    void            singal_report(QString cam_ip, QString time_stamp, int Crosstype, int nWheels);
    void            signal_stop();    // server thread stoped
    void            signal_received_data(QString camIP, QString camIndex,  int crossType, int nWheels, QString clss, QString szBuffer);
    void            signal_recevied_status(int cam_index, QString szBuffer);

public slots:
    void            clientDisconnected(int);
    void            onstart();          // thread started
    void            chatServerError(QAbstractSocket::SocketError socketError);
    void            onReceiveDetectionResult(QString camIP, QString cam_index, int crossType, int nWheels, QString clss, QString szBuffer);
    void            onNewConnection();
private:
    QTcpSocket*     m_sck;
    QString         server_ip;
    QString         device_ip;
    uint16_t          server_port;
    bool            m_bEnableThread;
    bool            m_bStarted;
    QMap<std::string, int>    m_devices_calibed;
    QMutex          _mutex;
};

#endif // CHATSERVER_H


