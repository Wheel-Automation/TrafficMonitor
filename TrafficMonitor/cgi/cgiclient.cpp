
#include <QUrl>
#include <QDebug>
#include <QNetworkProxy>
#include <QUrlQuery>
#include "utils.h"
#include "cgiclient.h"


CgiClient::CgiClient(QObject *parent)
: QObject(parent),
  manager(new QNetworkAccessManager(this))
{

}


void CgiClient::sendLoginRequest(const QString &ip, const QString &username, const QString &password) {
    // Build CGI URL

    QUrl qurl(QString("http://%1/cgi-bin/login.cgi?action=list&group=LOGIN").arg(ip));
    QNetworkRequest request(qurl);
    qDebug() << "Send CGI login request:" << qurl;

    // Required headers
    request.setRawHeader("User-Agent", "IPC_CGI");
    // request.setRawHeader("Host", QString("%1:80").arg(ip).toUtf8());

    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/html");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                             QNetworkRequest::NoLessSafeRedirectPolicy);



    // Basic Auth
    QByteArray auth = QString("%1:%2").arg(username, password).toUtf8().toBase64();
    request.setRawHeader("Authorization", "Basic " + auth);

    manager->setProxy(QNetworkProxy::NoProxy);


    // Send GET
    QNetworkReply* reply = manager->get(request);
    reply->setProperty("reqType", QVariant::fromValue(static_cast<int>(e_cgi_login)));

    connect(reply, &QNetworkReply::finished, this, &CgiClient::onReplyFinished);
}

void CgiClient::sendReadEthernetRequest(QString ip, QString username, QString password){
     qDebug() << "Read the ethernet configuration of camera " << ip;

     QUrl qurl(QString("http://%1/cgi-bin/param.cgi?action=list&group=ETH").arg(ip));
     QNetworkRequest request(qurl);
     qDebug() << "read ethernet url:" << qurl;

     // Required headers
     request.setRawHeader("User-Agent", "IPC_CGI");
     // request.setRawHeader("Host", QString("%1:80").arg(ip).toUtf8());
     request.setHeader(QNetworkRequest::ContentTypeHeader, "text/html");


     // Basic Auth
     QByteArray auth = QString("%1:%2").arg(username, password).toUtf8().toBase64();
     request.setRawHeader("Authorization", "Basic " + auth);

     // set request type
     QNetworkReply* reply = manager->get(request);
     reply->setProperty("reqType", QVariant::fromValue(static_cast<int>(e_cgi_get_eth)));

     // Send GET
     connect(reply, &QNetworkReply::finished, this, &CgiClient::onReplyFinished);

}

void CgiClient::sendModifyIprequest(QString ip, QString username, QString pwd, EthernetInfo eth){


    QUrl qurl(QString("http://%1/cgi-bin/param.cgi").arg(ip));

    QUrlQuery query;
    // fixed part for setup ip
    query.addQueryItem("action", "update");
    query.addQueryItem("group", "ETH");
    query.addQueryItem("ETH.no", "0");

    // Dynamic parameters
    query.addQueryItem("ETH.dhcp", eth.dhcp? "1": "0");
    query.addQueryItem("ETH.ipaddr", eth.ipaddr);
    query.addQueryItem("ETH.netmask", eth.netmask);
    query.addQueryItem("ETH.gateway", eth.gateway);
    query.addQueryItem("ETH.autoDns", eth.autoDns ? "1": "0");
    query.addQueryItem("ETH.dns1", eth.dns1);
    query.addQueryItem("ETH.dns2", eth.dns2);

    qurl.setQuery(query);

    QNetworkRequest request(qurl);

    qDebug() << "Send CGI request to modify IP address" << query.toString(QUrl::FullyEncoded);

    // Required headers
    request.setRawHeader("User-Agent", "IPC_CGI");
    request.setRawHeader("Host", QString("%1:80").arg(ip).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain, */*");

    // Basic Auth
    QByteArray auth = QString("%1:%2").arg(username, pwd).toUtf8().toBase64();
    request.setRawHeader("Authorization", "Basic " + auth);

    // set query params



    // set request type
    QNetworkReply* reply = manager->get(request);
    reply->setProperty("reqType", QVariant::fromValue(static_cast<int>(e_cgi_set_eth)));

    // Send GET
    connect(reply, &QNetworkReply::finished, this, &CgiClient::onReplyFinished);
}


void CgiClient::onReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply){
        return;
    }
    uint8_t reqType = reply->property("reqType").toInt();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        emit requestFinished(data, reqType);
    } else {
        emit requestError(reply->errorString(), reqType);
    }
    reply->deleteLater();

}
