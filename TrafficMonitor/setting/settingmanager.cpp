#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include "utils.h"
#include "macro.h"
#include "settingmanager.h"
#include "settingutils.h"


#define json_device_ip          "device_ip"
#define json_device_type        "device_type"
#define json_device_pwd         "device_pwd"
#define json_logSaveDay         "log_save"
#define json_heartRate          "heartRate"
#define json_proxy              "proxy"
#define json_enable_proxy       "enable_proxy"
#define json_proxy_server       "proxy_server"
#define json_proxy_port         "proxy_port"

SettingManager::SettingManager(QObject * parent):
    QObject(parent)
{
    // make a home directory
    QString userDir = QDir::homePath();
    m_homeDir = userDir + "/QTireDetector";
    if(!QDir(m_homeDir).exists()){
        QDir().mkdir(m_homeDir);
    }
    m_configFile = QString("%1//config.json").arg(m_homeDir);
    m_configProxyFile = QString("%1/configProxy.json").arg(m_homeDir);

}

void SettingManager::loadSetting(PANEL_PARM& setting){
    setting.serverPort = SERVER_PORT;
    setting.clientPort = SOCKET_PORT;

    QFileInfo fi(m_configFile);
    if(!fi.exists()){ // cannot find configuration file
        setDefaultValue(setting);
        return;
    }
    // read cofiguration file
    QFile file ;
    file.setFileName(m_configFile);
    if( file.open( QIODevice::ReadOnly ) )
    {
        QByteArray bytes = file.readAll();
        file.close();

        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson( bytes, &jsonError );
        if( jsonError.error != QJsonParseError::NoError )
        {
            setDefaultValue(setting);
            qDebug() << "fromJson failed: " << jsonError.errorString();
            return ;
        }
        if(!document.isObject() )
        {
            setDefaultValue(setting);
            return;
        }
        QJsonObject jsonObj = document.object();
        setting.jtip = jsonObj[json_device_ip].toString();
        if(jsonObj.contains(json_device_type)){
            QJsonValue val = jsonObj.value(json_device_type);
            // Check type of the value
            if (val.isDouble()) {
                setting.dev_type = val.toInt();
            }
        }
        else{
            setting.dev_name = DEVTYPE;
        }
        setting.jtpwd = jsonObj[json_device_pwd].toString();
        setting.heartRate = jsonObj[json_heartRate].toInt(HEART_BIT_RATE);
        setting.cam_index = 0;
        setting._det_mode = DETECT_ONE_DIRECT;
        setting.nLogSaveDay = qMax(jsonObj[json_logSaveDay].toInt(), 1);
#if 0
        QJsonObject proxObj = jsonObj[json_proxy].toObject();
        setting.proxy.enableProxy = proxObj[json_enable_proxy].toBool();
        setting.proxy.proxyIP = proxObj[json_proxy_server].toString();
        setting.proxy.proxyPort = proxObj[json_proxy_port].toInt();
#else
        setting.proxy.proxyMode = e_proxy_rs;
        setting.proxy.proxyPort = 3;
        setting.proxy.proxyIP = "127.0.0.1";
        setting.proxy.proxyPort = 51087;
#endif
     }
     else{
        setDefaultValue(setting);
        return;
     }

}


void SettingManager::saveSetting(PANEL_PARM setting){

    QJsonObject mainContent;
    mainContent[json_device_ip] = setting.jtip;
    mainContent[json_device_type] = setting.dev_type;
    mainContent[json_device_pwd] = setting.jtpwd;
    mainContent[json_logSaveDay] = setting.nLogSaveDay;
    mainContent[json_heartRate] = setting.heartRate;

#if 0
    QJsonObject proxy;
    proxy[json_enable_proxy] = setting.proxy.enableProxy;
    proxy[json_proxy_server] = setting.proxy.proxyIP;
    proxy[json_proxy_port] = setting.proxy.proxyPort;
    mainContent.insert(json_proxy, proxy);
#endif

    QJsonDocument document;
    document.setObject( mainContent );
    QByteArray bytes = document.toJson( QJsonDocument::Indented );
    QFile file(m_configFile);
    if( file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate ) )
    {
        QTextStream iStream( &file );
        iStream.setCodec( "utf-8" );
        iStream << bytes;
        file.close();
    }
    else
    {
        qDebug() << "file open failed: " << m_configFile;
    }

}

void    SettingManager::setDefaultValue(PANEL_PARM& setting){
    setting.jtip = "192.168.1.200";
    setting.dev_type = DEVTYPE;
    setting.jtpwd= "123456";
    setting.cam_index = 0;
    setting._det_mode = DETECT_ONE_DIRECT;
    setting.proxy.proxyIP = "127.0.0.1";
    setting.proxy.proxyPort = 51087;
    setting.proxy.proxyMode = e_proxy_rs;
    setting.nLogSaveDay = 7;
    setting.enable_led = false;
    setting.enable5Wheel = false;
    setting.enableTimeCalib = false;
    return;
}

// load proxy setting
void    SettingManager::loadProxySetting(QMap<QString, PROXY_SETTING>& proxyTable){
    proxyTable.clear();
    QFileInfo fi(m_configProxyFile);
    if(!fi.exists()){ // cannot find configuration file
        return;
    }
    // read cofiguration file
    QFile file ;
    file.setFileName(m_configProxyFile);
    if( file.open( QIODevice::ReadOnly ) )
    {
        QByteArray bytes = file.readAll();
        file.close();

        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson( bytes, &jsonError );
        if( jsonError.error != QJsonParseError::NoError )
        {
            qDebug() << "fromJson failed: " << jsonError.errorString();
            return ;
        }
        if(!document.isObject() )
        {
            qDebug() << "failed to convert json object";
            return;
        }
        QJsonObject jsonObj = document.object();
        foreach(const QString& key, jsonObj.keys()) {
            QJsonObject one = jsonObj[key].toObject();
            PROXY_SETTING setting;
            setting.password = one["password"].toString();
            setting.proxyMode = PROXY_MODE(one["proxyMode"].toInt());
            setting.proxyIP = one["proxyIP"].toString();
            setting.proxyPort = one["tcpPort"].toInt();
            setting.comPort = one["comPort"].toString();
            QString deviceIP = key;
            proxyTable[deviceIP] = setting;
        }
    }
    return;
}

void    SettingManager::saveProxySetting(QMap<QString, PROXY_SETTING> proxyTable){

    // make json object
    QJsonObject     jMainObj;
    QMap<QString, PROXY_SETTING>::iterator iter;
    for(iter = proxyTable.begin(); iter != proxyTable.end(); iter++ ){
        QString camIP = iter.key();
        PROXY_SETTING setting= iter.value();

        QJsonObject     jOne ;
        jOne["password"] = setting.password;
        jOne["proxyMode"] = int(setting.proxyMode);
        jOne["proxyIP"] = setting.proxyIP;
        jOne["tcpPort"] = setting.proxyPort;
        jOne["comPort"] = setting.comPort;
        jMainObj.insert(camIP, jOne);
    }
    // save json object
    QJsonDocument document;
    document.setObject(jMainObj);
    QByteArray bytes = document.toJson( QJsonDocument::Indented );
    QFile file(m_configProxyFile);
    if( file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate ) )
    {
        QTextStream iStream( &file );
        iStream.setCodec( "utf-8" );
        iStream << bytes;
        file.close();
    }
    qDebug() << "Success to save proxy configuration file";
}
