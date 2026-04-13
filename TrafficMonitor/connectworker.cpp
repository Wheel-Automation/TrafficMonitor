#include <QTcpSocket>
#include <QDateTime>
#include "connectworker.h"
#include "utils.h"
#include "macro.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#define api_device_name_key         QString("device_name")      // device name set by user
#define api_device_type_key         QString("device_type")      // device type key, defined by SUPT_DEV_TYPE(utils.h)
#define api_login_password_key      QString("login password") // device login password
#define api_header_key              QString("api_key")          // type of api requests
#define api_selected_camera_key     QString("selected camera")  // selected camera index
#define api_double_det_mode_key     QString("double detect mode")   // one/two camera mode
#define api_camera_user_nm_key      QString("camera user name")
#define api_camera_ip_key           QString("camera_ip")
#define api_camera_pwd_key          QString("camera_password")
#define api_det_direction_key       QString("detect_detection")
#define api_setting_roi_key         QString("detection_roi")
#define api_setting_ref_key         QString("detection_ref_ratio")
#define api_server_ip_key           QString("server ip address")
#define api_server_port_key         QString("server port address")
#define api_result_key              QString("acknowledge_result")
#define api_device_new_ip           QString("device_new_ip")
#define api_device_new_ip_mask      QString("device_new_ip_mask")
#define api_device_new_ip_gateway   QString("device_new_ip_gateway")
#define api_device_new_ip_dns       QString("device_new_ip_dns_server")
#define api_device_enable_led       QString("device_enable_led")
#define api_led_turn_on_time        QString("device_led_turn_on_clock")     // clock to turn on LED device
#define api_led_turn_off_time       QString("device_led_turn_off_clock")     // clock to turn off LED device
#define api_result_output_speed     QString("device_result_output_speed")   // threshold value of timeout before output detection result
#define api_enable_5wheel           QString("device_enable_five_wheel")     // enable 5 wheel result
#define api_enable_time_calib       QString("device_enable_time_calibration")   // enable time calibration

#define api_time_year               QString("device_set_time_year")
#define api_time_month              QString("device_set_time_month")
#define api_time_day                QString("device_set_time_day")
#define api_time_hour               QString("device_set_time_hour")
#define api_time_min                QString("device_set_time_min")
#define api_time_sec                QString("device_set_time_second")

typedef enum _API_REQUEST{
    API_UNKNOWN = 9999,
    API_GET_PARAM = 10001,  // get the parameter settings from server
    API_SET_PARAM = 10002,  // set the parameters into server
    API_RESET_PWD = 10003,  // reset the device password
    API_RESET_IP = 10004,    // reset the device ip
    API_RESTART_DEV = 10005 // restart the device
}API_REQUEST;

connectWorker::connectWorker(PANEL_PARM param, SETTING setting, int connect_type, QObject* parent):
    QObject(parent),
    _socket(nullptr)
{
    _panel = param;
    _setting = setting;
    _connect_type = connect_type;
    _enable = true;
}

// start processing
void connectWorker::process(){

    if(_connect_type != e_calib_time){
        doConnect();
        if(!_socket->waitForConnected(5000))
        {
            qDebug()<<"Failed to conect to socket server";
            log_file("Failed to conect to socket server");
            emit report(false, _connect_type, _panel, _setting);
            return;
        }

        qDebug() << "connected to jetson device...";
        log_file("connected to jetson device...");
        QJsonObject jobj;

        jobj[api_login_password_key] = _panel.jtpwd;
        jobj[api_selected_camera_key] = 0;
        if(_connect_type == e_get_param){ // get parameter from jetson
            jobj[api_header_key] = QString("%1").arg(API_GET_PARAM);
            int y, M, d, h,m,s;
            QDateTime date = QDateTime::currentDateTime();
            date.date().getDate(&y, &M, &d);
            h = date.time().hour();
            m = date.time().minute();
            s = date.time().second();

#if 1       // set time information
            jobj[api_time_year] = y;
            jobj[api_time_month] = M;
            jobj[api_time_day] = d;
            jobj[api_time_hour] = date.time().hour();
            jobj[api_time_min] = date.time().minute();
            jobj[api_time_sec] = date.time().second();
#endif
        }
        else if(_connect_type == e_set_param)// set param into jetson
        {
            QJsonArray corner_array;
            for(int i = 0; i < 6; i ++){
                float x = _setting.ptRegion[i].rx();
                float y = _setting.ptRegion[i].ry();
                corner_array.append(QJsonValue(double(x)));
                corner_array.append(QJsonValue(double(y)));
            }
            qDebug()<<"set device name: "<<_panel.dev_name <<"\n";

            jobj[api_device_name_key] = _panel.dev_name;
            jobj[api_header_key] = QString("%1").arg(API_SET_PARAM);
            jobj[api_camera_ip_key] = _panel.camip;
            //jobj[api_camera_pwd_key] = _panel.campwd;
            //jobj[api_camera_user_nm_key] = _panel.camname;
            jobj[api_det_direction_key] = QString("%1").arg(_panel.e_direct);
            jobj[api_setting_roi_key] = corner_array;
            jobj[api_setting_ref_key] = QString::number(_setting.ptRef);
            jobj[api_server_ip_key] = _panel.serverip;
            jobj[api_server_port_key] = _panel.serverPort;

            jobj[api_device_enable_led] = (_panel.enable_led)?1:0;
            jobj[api_led_turn_on_time] = _panel.ledOnTime;
            jobj[api_led_turn_off_time] = _panel.ledOffTime;
            jobj[api_result_output_speed] = _panel.timeOut;
            jobj[api_enable_5wheel] = (_panel.enable5Wheel)?1:0;
            jobj[api_enable_time_calib] = (_panel.enableTimeCalib)? 1:0;
        }
        else if(_connect_type == e_reset_dev_pwd){
            jobj[api_header_key] = QString("%1").arg(API_RESET_PWD);
        }
        else if(_connect_type == e_reset_ip){
            jobj[api_header_key] = QString("%1").arg(API_RESET_IP);
            jobj[api_device_new_ip] = _panel.jt_new_ip;
            jobj[api_device_new_ip_mask] = _panel.jt_new_mask;
            jobj[api_device_new_ip_gateway] = _panel.jt_new_gateway;
            jobj[api_device_new_ip_dns] = _panel.jt_new_dns;
        }
        else if(_connect_type == e_restart_device ){
            jobj[api_header_key] = QString("%1").arg(API_RESTART_DEV);
        }
        QJsonDocument doc(jobj);
        QString strJson(doc.toJson(QJsonDocument::Compact));

        qDebug()<<strJson;
        log_file(strJson);
        _socket->write(strJson.toUtf8().constData());
        _socket->flush();
        //emit finished();
    }
    else{ // send time calibration information

        QMap<QString, PROXY_SETTING>::iterator iter;
        for(iter = m_proxyMap.begin(); iter != m_proxyMap.end(); iter ++){ // send request for each device
            qDebug() << "send time calibration request";
            QString devIP = iter.key();
            PROXY_SETTING proxySet = iter.value();
            QString password = proxySet.password;

            QTcpSocket  *socket = new QTcpSocket(this);
            connect(socket, SIGNAL(connected()),this, SLOT(connected()));
            connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
            connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
            _szip = QHostAddress(devIP);
            socket->connectToHost(_szip, _panel.clientPort);

            // we need to wait...
            if(!socket->waitForConnected(5000))
            {
                qDebug()<<"Failed to conect to socket server";
                continue;
            }

            qDebug() << "connected to jetson device...";
            QJsonObject jobj;
            jobj[api_login_password_key] = password;
            jobj[api_selected_camera_key] = 0;
            jobj[api_double_det_mode_key] = DETECT_ONE_DIRECT;

            jobj[api_header_key] = QString("%1").arg(API_GET_PARAM);

            int y, M, d, h,m,s;
            QDateTime date = QDateTime::currentDateTime();
            date.date().getDate(&y, &M, &d);
            h = date.time().hour();
            m = date.time().minute();
            s = date.time().second();

            // set time information
            jobj[api_time_year] = y;
            jobj[api_time_month] = M;
            jobj[api_time_day] = d;
            jobj[api_time_hour] = date.time().hour();
            jobj[api_time_min] = date.time().minute();
            jobj[api_time_sec] = date.time().second();

            QJsonDocument doc(jobj);
            QString strJson(doc.toJson(QJsonDocument::Compact));

            qDebug()<<strJson;
            socket->write(strJson.toUtf8().constData());
            socket->flush();
        }
        //emit finished();
    }// end time calib request
}

void connectWorker::doConnect()
{
    _socket = new QTcpSocket(this);
    connect(_socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(_socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(_socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(_socket, SIGNAL(readyRead()),this, SLOT(readyRead()));

    _szip = QHostAddress(_panel.jtip);

    qDebug() << "connecting...:"<<_szip<<":"<<_panel.clientPort;
    QString msg = QString("connect to device server: %1(%2)").arg(_panel.jtip).arg(_panel.clientPort);
    log_file(msg);
    _socket->connectToHost(_szip, _panel.clientPort);
    // we need to wait...

}

void connectWorker::connected()
{

}

void connectWorker::disconnected()
{
    if(_socket == nullptr)
        return;
    qDebug() << "disconnected from host";
    _socket->disconnectFromHost();
}

void connectWorker::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void connectWorker::readyRead()
{
    qDebug() << "reading...";
    QByteArray data = _socket->readAll();
    int nRes = 0;
    try {
        QString param = QString(data);
        QJsonObject obj;
        QJsonDocument doc = QJsonDocument::fromJson(param.toUtf8());
        if(doc.isNull() || !doc.isObject()){
            qDebug() << "Invalid JSON...\n" << endl;
            log_file("response is invalid json format");
        }
        else
        {
           log_file("received string after setting parameters: ");

            qDebug()<<doc;

#if LOG_FILE
            QString strJson(doc.toJson(QJsonDocument::Compact));
            log_file(strJson);
#endif

            obj = doc.object();
            int key = obj[api_header_key].toInt();
            nRes = obj[api_result_key].toInt();

            if(key == API_UNKNOWN || nRes == 0){ // unkown error
                nRes = 0;
            }
            else if (_connect_type == e_get_param && key == API_GET_PARAM){ // get config result
                qDebug() << "read device name: "<<obj[api_device_name_key].toString()<<"\n";
                _panel.dev_name = obj[api_device_name_key].toString();
                // read device type
                if(obj.contains(api_device_type_key)){
                    QJsonValue val = obj.value(api_device_type_key);
                    _panel.dev_type = val.toInt(val.toInt(DEVTYPE));

                }
                else{
                    _panel.dev_type = DEVTYPE;
                }

                _panel.camip = obj[api_camera_ip_key].toString();
                _panel.serverip = obj[api_server_ip_key].toString();
                _panel.enable_led = obj[api_device_enable_led].toInt();
                _panel.ledOnTime = obj[api_led_turn_on_time].toInt();
                _panel.ledOffTime = obj[api_led_turn_off_time].toInt();
                _panel.timeOut = obj[api_result_output_speed].toInt();
                _panel.enable5Wheel = obj[api_enable_5wheel].toInt();
                _panel.enableTimeCalib = obj[api_enable_time_calib].toInt();
                int direction = obj[api_det_direction_key].toInt();
                qDebug() << "current direction" << direction << endl;
                if (direction == 0)
                    _panel.e_direct = DIRECTION_RL;
                else
                    _panel.e_direct = DIRECTION_LR;

                _setting.ptRef = float(obj[api_setting_ref_key].toDouble());

                // get the roi
                QJsonArray roi = obj[api_setting_roi_key].toArray();
                QPointF* pts = QJsontoQPointArray(roi);
                memcpy_s(_setting.ptRegion,sizeof(QPointF) * 6, pts, sizeof(QPointF) * 6);
                delete pts;
                log_file("2");

            }
            else if(_connect_type == e_set_param && key == API_SET_PARAM){ // set param result
                    qDebug() << "success to receive response after setting parameters, result =" << nRes;
            }
            else if(_connect_type == e_reset_dev_pwd && key == API_RESET_PWD){ // reset param result
                //nRes = 1;
            }
            else if(_connect_type == e_reset_ip && key == API_RESET_IP){ // reset device ip address
                //nRes = 1;
            }
            else if(_connect_type == e_restart_device){
                qDebug("receve data from restart_device event");
            }
        }

    } catch (...) {
        log_file("Received Invalid Json format");
        nRes = 1;
    }
    emit report(nRes, _connect_type, _panel, _setting);
    return;
}

// send requests to get a config file
void connectWorker::process_connect(){

}
