#ifndef MODIFYIP_H
#define MODIFYIP_H

#include <QDialog>
#include <QObject>
#include <QCloseEvent>
#include <QString>
#include <vector>
#include "utils.h"
#include "cumstomipeditor.h"
#include "cgi/cgiclient.h"

namespace Ui {
    class ModifyIP;
}

class ModifyIP : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyIP(QWidget *parent = nullptr, IP_SET ipsets = IP_SET(), uint8_t camType = e_dev_jetson);
    void    reset_device_ip(bool res);
    ~ModifyIP();

signals:
    void signal_reset_ip(IP_SET ipsets);
    void signal_reset_ip2(QString ip);
private slots:
    void on_btn_reset_ip_clicked();
    void on_btn_cancel_clicked();

    // slots for cgi request
    void on_cgi_requestFinished(const QByteArray &data, int requestType);
    void on_cgi_requestFailed(const QString& error, int request_type);


private:
    bool modifyHisiIpBySearchSdk(const QString &user, const QString &pwd);
    void InitUI();
    void show_ip();
    bool login_device();
    void logout_device();
    void	closeEvent(QCloseEvent *e) override;

    void enable_loginInput(bool enable);
    bool checkIpSets(std::vector<int> sectMask, std::vector<int> blackList, std::vector<int> sectIp);
    bool checkGateWay(std::vector<int> sectMask, std::vector<int> sectIp, std::vector<int> sectGateway);

    // get the description of cgi request enums
    QString getCgiRequestDesc(uint8_t reqType);

    // converet ethernet response to structure
    EthernetInfo parseEthernetResponse(const QString &response);


private:
    Ui::ModifyIP *ui;
    IP_SET      _ip_sets;
    QString     _dev_ip;
    CustomIpEditor*     _ip;
    CustomIpEditor*     _ip_mask;
    CustomIpEditor*     _ip_gateway;
    CustomIpEditor*     _ip_dns;
    CustomIpEditor*     _ip_dns2;
    long                _g_handle;

    uint8_t             _camType;
    // cgi client used in hi3519
    CgiClient*          _cgiClient;
    EthernetInfo        _eth;
};

#endif // MODIFYIP_H
