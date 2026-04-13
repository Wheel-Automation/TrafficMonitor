// CgiClient.h
#ifndef CGICLIENT_H
#define CGICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QByteArray>

struct EthernetInfo {
    QString ipaddr;
    QString netmask;
    QString gateway;
    bool dhcp;
    bool autoDns;
    QString dns1;
    QString dns2;
    QString mac;
    QString errDescription;
};

class CgiClient : public QObject {
    Q_OBJECT
public:
    explicit CgiClient(QObject *parent = nullptr);


    // Sends the CGI GET request
    void sendLoginRequest(const QString &ip, const QString &username, const QString &password);


    void sendReadEthernetRequest(QString ip, QString username, QString password);

    void sendModifyIprequest(QString ip, QString username, QString pwd, EthernetInfo ethernet);

signals:
    void requestFinished(const QByteArray &data, int requestType);
    void requestError(const QString &err, int requestType);


private slots:
    void onReplyFinished();


private:
    QNetworkAccessManager *manager;

};


#endif // CGICLIENT_H
