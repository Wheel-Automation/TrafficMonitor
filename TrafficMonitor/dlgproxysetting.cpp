#include <QDebug>
#include <QDateTime>
#include <QTime>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSerialPort>
#include "macro.h"
#include "dlgproxysetting.h"
#include "ui_dlgproxysetting.h"
#include "proxyworker.h"
#include "utils.h"
#include "com/com.h"

dlgProxySetting::dlgProxySetting(PROXY_SETTING setting, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgProxySetting),
    _setting(setting),
    m_bInitComport(false)
{
    ui->setupUi(this);
    m_nWheelID = 1;

    iconConnect = "border-image:url(:/asset/connected.png)";
    iconDisconnect = "border-image:url(:/asset/disconnect.png)";

    initializeUI();


    QObject::connect(&m_Com, &Com::errorOccured, this, &dlgProxySetting::onComportError);
    initComPort();
    m_selfIP = GetSelfAddress();
    m_proxyTask.setMaxThreadCount(1);
    setFixedSize(width(), height());
}


dlgProxySetting::~dlgProxySetting()
{
    m_Com.close();
    delete ui;
}

void dlgProxySetting:: onComportError(QString erroMessag){
    qDebug() << "Error occured in comport: " << erroMessag;
}

void dlgProxySetting::initializeUI(){
    ui->proxyPort->setText(QString("%1").arg(_setting.proxyPort));

    int cnt = 0;
    for(int i = 2; i <= 6; i ++){
        ui->combWheel->addItem(QString("%1").arg(i));
        ui->combWheel->setItemData(cnt, QBrush(Qt::white), Qt::TextColorRole);
        cnt ++;
    }
    ui->combWheel->setCurrentIndex(4);
    m_testWheel = 6;

    ui->proxyPort->setValidator( new QIntValidator(1, 65535, this));

    ui->combProxyMode->addItem("禁止转发");
    ui->combProxyMode->addItem("TCP转发");
    ui->combProxyMode->addItem("COM口转发");
    for(int i = 0; i < 5; i ++){
        ui->combWheel->setItemData(i, QBrush(Qt::white), Qt::TextColorRole);
    }
    int nPorxyMode = int(_setting.proxyMode);
    ui->combProxyMode->setCurrentIndex(nPorxyMode);
    changeItemControl(PROXY_MODE(nPorxyMode));
    ui->btnLed->setVisible(false);
    setWindowTitle("设置转发客户端");
    setFixedSize(width(), height());
}

void    dlgProxySetting::initComPort(){
    int nPorts = 0;
    QStringList names = Com::findAllComPorts();
    names.sort();
    qDebug() << "all comports of this computer: " << names;
    int nSelectedIndex = -1;
    m_bInitComport = true;
    for(auto portName: names) {
        ui->combComPort->addItem(portName);
        ui->combWheel->setItemData(nPorts, QBrush(Qt::white), Qt::TextColorRole);
        if(portName == _setting.comPort){
            nSelectedIndex = nPorts;
        }
        nPorts ++;
    }

    if(nSelectedIndex >= 0){
        ui->combComPort->setCurrentIndex(nSelectedIndex);
    }
    else if(nPorts > 0){
        ui->combComPort->setCurrentIndex(0);
    }
}

void    dlgProxySetting::turnOnLed(bool bOn){
        ui->btnLed->setVisible(true);
        if(bOn)
            ui->btnLed->setStyleSheet(iconConnect);
        else
            ui->btnLed->setStyleSheet(iconDisconnect);
}



void    dlgProxySetting:: changeItemControl(PROXY_MODE proxyMode){

    if(proxyMode == e_proxy_disable){
        ui->lable_Port->setVisible(false);
        ui->label_WheelCnt->setVisible(false);
        ui->label_Log->setVisible(false);
        ui->combComPort->setVisible(false);
        ui->btnLed->setVisible(false);
        ui->combWheel->setVisible(false);
        ui->proxyPort->setVisible(false);
        ui->btnSendData->setVisible(false);
        ui->textSendBuffer->setVisible(false);
    }
    else if(proxyMode == e_proxy_tcp){
        ui->label_Log->setVisible(true);
        ui->lable_Port->setVisible(true);
        ui->lable_Port->setWindowTitle("客户端端口");
        ui->label_WheelCnt->setVisible(true);
        ui->btnLed->setVisible(false);
        ui->combComPort->setVisible(false);
        ui->combWheel->setVisible(true);
        ui->proxyPort->setVisible(true);
        ui->btnSendData->setVisible(true);
        ui->textSendBuffer->setVisible(true);
    }
    else if(proxyMode == e_proxy_rs){
        ui->label_Log->setVisible(true);
        ui->lable_Port->setVisible(true);
        ui->lable_Port->setWindowTitle("COM端口");
        ui->label_WheelCnt->setVisible(true);
        ui->btnLed->setVisible(true);
        ui->combComPort->setVisible(true);
        ui->combWheel->setVisible(true);
        ui->proxyPort->setVisible(false);
        ui->btnSendData->setVisible(true);
        ui->textSendBuffer->setVisible(true);
    }

}

void dlgProxySetting::on_btnOk_clicked()
{
    PROXY_MODE pMode = PROXY_MODE(ui->combProxyMode->currentIndex());

    if(pMode == e_proxy_tcp){ // enable proxy, but invalid parameters.
        if(!checkProxyServerPort()){
            return;
        }
    }
    else if(pMode == e_proxy_rs){
        QString port = ui->combComPort->currentText();
        if(port.length() <= 0){ // invalid port
            return;
        }
    }

    // read parameters
    _setting.proxyMode= pMode;
    _setting.proxyIP = "127.0.0.1";
    _setting.proxyPort = ui->proxyPort->text().toInt();
    if(pMode == e_proxy_rs)
        _setting.comPort = ui->combComPort->currentText();
    else
        _setting.comPort = "COM3";

    m_proxyTask.clear();
    m_Com.close();
    emit modifyProxy(_setting);
    close();
}

void dlgProxySetting::on_btnCancel_clicked()
{
    m_proxyTask.clear();
    m_Com.close();
    emit unmodifyProxy();
    close();
}


bool    dlgProxySetting::checkProxyServerPort(){

    QString text = ui->proxyPort->text();
    if(text.length() <= 0){
        QMessageBox qbox(QMessageBox::Question,tr("通知"),
                          tr("无效端口,该在1~65535范围内."), QMessageBox::Yes, this);
        qbox.setButtonText(QMessageBox::Yes, "确定");
        qbox.exec();
        return false;
    }

    int nPort = text.toInt();
    if (nPort == SOCKET_PORT || nPort == SERVER_PORT){
        QMessageBox qbox(QMessageBox::Question,tr("通知"),
                          tr("此端口已被占用，无法使用"), QMessageBox::Yes, this);
        qbox.setButtonText(QMessageBox::Yes, "确定");
        qbox.exec();
        return false;
    }

    if(nPort >= 65536){
        QMessageBox qbox(QMessageBox::Question,tr("通知"),
                          tr("无效端口,该在1~65535范围内."), QMessageBox::Yes, this);
        qbox.setButtonText(QMessageBox::Yes, "确定");
        qbox.exec();
        return false;
    }
    return true;
}

bool    dlgProxySetting:: checkProxyComPort(){
      int comPort = ui->combComPort->currentIndex();
      if(comPort < 0){
          QMessageBox qbox(QMessageBox::Question,tr("通知"),
                            tr("选择有效COM端口"), QMessageBox::Yes, this);
          qbox.setButtonText(QMessageBox::Yes, "确定");
          qbox.exec();
          return false;
      }
      return true;
}



// send test data to proxy server
void dlgProxySetting::on_btnSendData_clicked()
{
    PROXY_MODE pMode = PROXY_MODE(ui->combProxyMode->currentIndex());
    if(pMode == e_proxy_disable)
        return;

    int nType=12;
    switch (m_testWheel) {
        case 2:
            nType = 12;
            break;
        case 3:
            nType = 112;
            break;
        case 4:
            nType = 1222;
            break;
        case 5:
            nType = 12222;
            break;
        case 6:
            nType = 122222;
            break;
        default:
            break;
    }

    if(pMode == e_proxy_tcp){   // tcp mode
        if( !checkProxyServerPort())
            return;

        // determine wheel type
        QString serverIP = "127.0.0.1";
        int     serverPort = ui->proxyPort->text().toInt();
        proxyWorker* task = new proxyWorker();

        QString szData;
        szData = QString("{\"camera\":1,\"camera_ip\":\"%1\",\"cross_type\":1,\"id\":%2,\"kinds\":%3,\"report_type\":0,\"wheels\":%4}").
                arg(m_selfIP).arg(m_nWheelID).arg(nType).arg(m_testWheel);
        m_nWheelID ++;
        task->setParameter( serverIP, serverPort, szData);
        connect(task, &proxyWorker::sentBuffer, this, &dlgProxySetting::onSendBuffer);
        // task->start();
        m_proxyTask.start(task);
    }
    else if(pMode == e_proxy_rs){ // rs-232 mode
        if( !checkProxyComPort())
            return;
        QString portName = ui->combComPort->currentText();
        if(!m_Com.isOpened()){
            m_Com.open(portName);
        }
        if(m_Com.isOpened()){
            QString buffer = m_Com.sendWheelData(m_testWheel, nType);
            qDebug()<< "proxy send data result: " << buffer;
            bool bSendSuccess = buffer.length() > 0;
             if(!bSendSuccess)
                 buffer = "无法打开COM串口";

             QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
             buffer = timestamp  + " : " + buffer;
#if COMPORT_SEND_WHLTYPE
             if(buffer.length() > 0){
                 QString prefix = QString(" : (%1胎车,%2型)").arg(m_testWheel).arg(nType);
                 buffer = buffer + prefix;
             }
#endif
             ui->textSendBuffer->appendPlainText(buffer);
             ui->textSendBuffer->moveCursor (QTextCursor::End);
        }
    }
}

void dlgProxySetting:: onSendBuffer(QString szBuffer, QString serverIP, int serverPort, bool bSuccess){
    qDebug()<< "proxy send data result: " << bSuccess;
     if(!bSuccess){
         szBuffer = QString("无法服务器连接");
     }
     QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
     szBuffer = timestamp  + " : " + szBuffer;
     ui->textSendBuffer->appendPlainText(szBuffer);
     ui->textSendBuffer->moveCursor (QTextCursor::End);
}

void dlgProxySetting::on_combWheel_currentIndexChanged(const QString &szContent)
{
    m_testWheel = szContent.toInt();
}

void dlgProxySetting::on_proxyPort_textChanged(const QString &strPort)
{
    if (strPort.length() <= 0)
        return;
    int nPort = strPort.toInt();
    if (nPort == SERVER_PORT || nPort == SOCKET_PORT ){
        QMessageBox qbox(QMessageBox::Question,tr("通知"),
                          tr("此端口已经被占用，无法使用"), QMessageBox::Yes, this);
        qbox.setButtonText(QMessageBox::Yes, "确定");
        qbox.exec();
    }
    if(nPort >= 65536){
        QMessageBox qbox(QMessageBox::Question,tr("通知"),
                          tr("无效端口,该在1~65535范围内."), QMessageBox::Yes, this);
        qbox.setButtonText(QMessageBox::Yes, "确定");
        qbox.exec();
    }
}


void dlgProxySetting::on_combComPort_currentIndexChanged(int index)
{
    if(!m_bInitComport)
        return;
    QString portName = ui->combComPort->currentText();
    qDebug() << "port changed:" << portName;
    m_Com.open(portName);
    if(!m_Com.isOpened())
        turnOnLed(false);
    else
        turnOnLed(true);
}

void dlgProxySetting::on_combProxyMode_currentIndexChanged(int index)
{
    PROXY_MODE pMode = PROXY_MODE(index);
    changeItemControl(pMode);
}

