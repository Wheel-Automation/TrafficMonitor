#ifndef PROXYWORKER_H
#define PROXYWORKER_H
#include <QThread>
#include <QObject>
#include <QRunnable>


class proxyWorker: public QObject, public QRunnable
{

    Q_OBJECT
public:
    explicit proxyWorker(QObject* parent = nullptr);
    void setParameter(QString serverIP, int nPort, QString szBuffer){
        m_serverIP = serverIP;
        m_serverPort = nPort;
        m_szBuffer = szBuffer;
    }

signals:
    void sentBuffer(QString szBuffer, QString serverIP, int serverPort, bool bSuccess);

protected:
    void run() override;

private:
    QString     m_serverIP;
    int         m_serverPort;
    QString     m_szBuffer;
};

#endif // PROXYWORKER_H
