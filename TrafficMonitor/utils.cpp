#include "utils.h"
#include <QString>
#include <QPoint>
#include <QtMath>
#include <QJsonArray>
#include <QJsonValue>
#include <QPointF>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QTcpServer>
#include <QNetworkAddressEntry>
#include <QTime>

#include <QList>
#include <QStringList>

#include "macro.h"



void register_datatype(){
    qRegisterMetaType<PANEL_PARM>("PANEL_PARM");
    qRegisterMetaType<SETTING>("SETTING");
    qRegisterMetaType<IP_SET>("IP_SET");
    qRegisterMetaType<DETECT_MODE>("DETECT_MODE");

}

int distance(QPoint pt1, QPoint pt2){
    return (int) qSqrt(qPow(pt1.rx() - pt2.rx(), 2) + qPow(pt1.ry() - pt2.ry(), 2));
}


/**
 * @brief QJsontoQPointArray
 * @param jsonArray
 * @return : convert json array into float array
 */
QPointF* QJsontoQPointArray(QJsonArray jsonArray){

    int size = jsonArray.count();
    int nPTs = int(size / 2);
    QPointF* pts= new QPointF[nPTs];

    for (int i = 0; i < nPTs; i ++){
        double x = (float)jsonArray.at(2 * i).toDouble();
        double y = (float)jsonArray.at(2 * i + 1).toDouble();
        pts[i] = QPointF(x, y);
    }
    return pts;
}

void    promp_error_message(QWidget* parent, QString errormsg){
    QMessageBox qbox(QMessageBox::Question,QString("通知"), errormsg.toUtf8(), QMessageBox::Yes, parent);
    qbox.setButtonText(QMessageBox::Yes, QString("确定"));
//    qbox.setStyleSheet("QMessageBox {background-color: rgb(51, 51, 51);}");
//    qbox.setStyleSheet("QMessageBox QLabel {color: rgb(200, 200, 200);}}");

    qbox.exec();
}

QString GetSelfAddress(){
    QString ipAddress;
       QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
       // use the first non-localhost IPv4 address
       for (int i = 0; i < ipAddressesList.size(); ++i) {

           if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
               ! ipAddressesList.at(i).isLoopback() &&
               ipAddressesList.at(i).toIPv4Address()) {
               ipAddress = ipAddressesList.at(i).toString();
               break;
           }
       }
       // if we did not find one, use IPv4 localhost
       if (ipAddress.isEmpty())
           ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
       return ipAddress;
}

// return all ip addresses
QStringList     GetSelfMultiAddress(){
    QList<QString> ipAddresses;

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {

        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ! ipAddressesList.at(i).isLoopback() &&
            ipAddressesList.at(i).toIPv4Address()) {
            QString addres = ipAddressesList.at(i).toString();
            ipAddresses.append(addres);
            //break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddresses.size() <= 0){
        QString ip = QHostAddress(QHostAddress::LocalHost).toString();
        ipAddresses.append(ip);

    }
    return QStringList(ipAddresses);
}




bool GetGateWay(QString ip, QString& ipGateWay){
    ipGateWay = "";
    QStringList ipList = ip.split(".");
    if(ipList.length() != 4)
        return false;
    ipGateWay = QString("%1.%2.%3.254").arg(ipList[0],ipList[1],ipList[2]);
    return true;
}

void    log_file(QString message){
    if(!LOG_FILE)
        return ;

    QString ts = QTime::currentTime().toString();
    message = QString("%1:   %2\n").arg(ts).arg(message);

    QTime ct = QTime::currentTime();
    // write file
    if(ct.minute() == 59 && ct.second() == 59)
    {
        QFile f("log.txt");
        if (f.open(QIODevice::WriteOnly)) {
            QTextStream stream(&f);
            stream << message;
            f.close();
        }
    }
    else{
        QFile f("log.txt");
        if (f.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream stream(&f);
            stream << message;
            f.close();
        }
    }
}

QString Utils::getLogHomeDir(){
    QString userDir = QDir::homePath();
    QString logDir = userDir + "/QTireDetector/Log";
    if(!QDir(logDir).exists()){
        QDir().mkdir(logDir);
    }
    return logDir;
}

QStringList      Utils::findAllLogFiles(QString dirPath){
    QStringList files;
    QList<QString> nameFilter;
    nameFilter<<"*.csv";

    QDirIterator it(dirPath, nameFilter, QDir::Files);
    while (it.hasNext())
        files << it.next();
    files.sort();
    return files;
}

QStringList         Utils::findAllSubDirectories(QString dirPath){
    QDirIterator  it(dirPath,  QDir::Dirs |QDir::NoDotAndDotDot);

    QStringList subDirs;
    while(it.hasNext()) {
        subDirs << it.next();
    }
    subDirs.sort();
    return subDirs;
}

QString Utils::FilePathToName(QString fullPath){
    QFileInfo f(fullPath);
    if(!f.exists())
        return "";
    return f.fileName();
}


