#ifndef DLGPROXYSETTING_H
#define DLGPROXYSETTING_H

#include <QDialog>
#include <QThreadPool>
#include <QSerialPort>
#include "macro.h"
#include "cumstomipeditor.h"
#include "utils.h"
#include "com/com.h"

namespace Ui {
class dlgProxySetting;

}

class dlgProxySetting : public QDialog
{
    Q_OBJECT

public:

    explicit dlgProxySetting( PROXY_SETTING setting,  QWidget *parent = nullptr);

    PROXY_SETTING getParameter(){return _setting; }
    ~dlgProxySetting();

signals:
    void modifyProxy(PROXY_SETTING param );
    void unmodifyProxy();

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_btnSendData_clicked();

    void on_combWheel_currentIndexChanged(const QString &arg1);

    void onComportError(QString erroMessag);

    void on_proxyPort_textChanged(const QString &arg1);

    void onSendBuffer(QString szBuffer, QString serverIP, int serverPort, bool bSuccess);

    void on_combComPort_currentIndexChanged(int index);

    void on_combProxyMode_currentIndexChanged(int index);

    // void accept() override;
    // void reject() override;

private:
    void    initializeUI();
    void    changeItemControl(PROXY_MODE proxyMode);

    bool    checkProxyServerPort();
    bool    checkProxyComPort();
    void    initComPort();
    void    turnOnLed(bool bOn);

private:
    Ui::dlgProxySetting *ui;
    //PANEL_PARM          _panel;
    PROXY_SETTING         _setting;
    //CustomIpEditor*     ip_server;
    int                 m_testWheel;
    QThreadPool         m_proxyTask;
    QString             m_selfIP;
    QString             iconConnect, iconDisconnect ;
    Com                 m_Com;
    bool                m_bInitComport;
    int                 m_nWheelID;
};

#endif // DLGPROXYSETTING_H
