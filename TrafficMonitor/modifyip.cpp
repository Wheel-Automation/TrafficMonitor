#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QCloseEvent>
#include <cstring>
// #include <Windows.h>
#include "macro.h"
#include "DevSearchSDK.h"
#include "utils.h"
#include "modifyip.h"
#include "ui_modifyip.h"
#include "NetClient.h"
#include "NetLib.h"
#include "cgi/cgiclient.h"

#define INVALID_LIBRARY     "错误：网络错误"
#define LOGIN_FAILED        "错误：主板注册失败"
#define GET_NETCFG_FAILED   "错误：获取网络设置失败"
#define GET_NETCFG_SUCCESS   "获取网络设置成功"
#define SET_NETCFG_FAILED   "错误：更新网络设置失败"
#define SET_NETCFG_SUCCESS  "更新网络设置成功"

namespace {
struct HisiSearchContext {
    QString targetIp;
    NET_DEV_SADPINFO sadpInfo;
    bool found;
};

int hisiModifySearchCb(const NET_DEV_SADPINFO *sadpInfo, void *userParam)
{
    qDebug()<<"Haisi modify IP callback function is called";
    if (sadpInfo == nullptr || userParam == nullptr) {
        qDebug() << "Invalid sadp infor or invalid user parameters";
        return 0;
    }

    HisiSearchContext *ctx = reinterpret_cast<HisiSearchContext *>(userParam);
    const char *devIp = sadpInfo->WifiNIC.EnableWifi ? sadpInfo->WifiNIC.NicCfg.IPAddr.IpV4
                                                     : sadpInfo->EthCfg.EthNICs[0].IPAddr.IpV4;
    if (ctx->targetIp != QString::fromUtf8(devIp)) {
        qDebug() << "Target IP isn't matched";
        return 0;
    }

    memcpy_s(&ctx->sadpInfo, sizeof(ctx->sadpInfo), sadpInfo, sizeof(NET_DEV_SADPINFO));
    ctx->found = true;
    qDebug() << "Success to change IP address";
    return 0;
}

typedef int (DEVSEARCH_CALL *GsfSetInfoFn)(const NET_DEV_MODIFY_NET *Param, BOOL bSync);

GsfSetInfoFn resolveSetInfoProc()
{
    static GsfSetInfoFn setInfoProc = nullptr;
    static bool triedResolve = false;
    if (triedResolve) {
        return setInfoProc;
    }
    triedResolve = true;

    HMODULE sdkModule = GetModuleHandleA("DevSearchSDK.dll");
    if (sdkModule == nullptr) {
        sdkModule = LoadLibraryA("DevSearchSDK.dll");
    }
    if (sdkModule == nullptr) {
        qDebug() << "failed to load DevSearchSDK.dll";
        return nullptr;
    }

    setInfoProc = reinterpret_cast<GsfSetInfoFn>(GetProcAddress(sdkModule, "GSF_DEV_SEARCH_SET_INFO"));
    if (setInfoProc == nullptr) {
        qDebug() << "GSF_DEV_SEARCH_SET_INFO is not exported by current sdk dll";
    }
    return setInfoProc;
}
}

ModifyIP::ModifyIP(QWidget *parent, IP_SET ipsets, uint8_t camType) :
    QDialog(parent),
    ui(new Ui::ModifyIP),
    _camType(camType),
    _cgiClient(nullptr)
{
    ui->setupUi(this);
    _dev_ip = ipsets.ip;
    _ip_sets.clear_sub_info();
    _ip_sets.ip = _dev_ip;
    InitUI();

    _g_handle = InitClientLib();
    setFixedSize(width(), height());
    if(_camType == e_dev_hisi3519){ // create cgi instance
        _cgiClient = new CgiClient(this);

        // connect slot
        connect(_cgiClient, &CgiClient::requestFinished, this, &ModifyIP::on_cgi_requestFinished);
        connect(_cgiClient, &CgiClient::requestError, this, &ModifyIP::on_cgi_requestFailed);
    }
 }


ModifyIP::~ModifyIP()
{
    delete _ip;
    delete _ip_gateway;
    delete ui;
    delete _ip_dns;
    if(_cgiClient){
        delete _cgiClient;
    }


}

void ModifyIP::InitUI()
{
    _ip = new CustomIpEditor(ui->frm_ip);
    _ip_mask = new CustomIpEditor(ui->frm_ip_mask);
    _ip_gateway = new CustomIpEditor(ui->frm_mask_getway);
    _ip_dns = new CustomIpEditor(ui->frm_ip_dns);
    _ip_dns2 = new CustomIpEditor(ui->frm_ip_dns2);
    if(_camType == e_dev_jetson || _camType == e_dev_c308x){
        _ip->SetText(_ip_sets.ip);
        _ip_mask->SetText(_ip_sets.ip_mask);
        _ip_gateway->SetText(_ip_sets.ip_gateway);
        _ip_dns->SetText(_ip_sets.ip_dns);
        _ip_dns->setVisible(false);

        // hide username and password
        enable_loginInput(false);
        ui->btn_cancel->setText("取消");
    }
    else if(_camType == e_dev_rv1126){
        _ip_sets.clear_sub_info();
        ui->btn_cancel->setText("获取");
        show_ip();
        enable_loginInput(false);
    }else if(_camType == e_dev_hisi3519){
        enable_loginInput(true);
        // hide cancel button
        _ip_sets.clear_sub_info();
        show_ip();
        ui->input_name->setText("admin");
        ui->input_pwd->setText("12345");
        ui->input_pwd->setEchoMode(QLineEdit::Password);
        ui->btn_cancel->setText("获取");
    }
    setWindowTitle("设置主板IP地址");
}


void ModifyIP::enable_loginInput(bool enable){
    ui->lbl_username->setEnabled(enable);
    ui->lbl_pwd->setEnabled(enable);
    ui->input_name->setEnabled(enable);
    ui->input_pwd->setEnabled(enable);
}

// check if ip is in black list
bool ModifyIP:: checkIpSets(std::vector<int> sectMask, std::vector<int> blackList, std::vector<int> sectIp){

    // get subnetwork position
    int last_position = 3;
    for(int i = 3; i >= 1; i--){
        if(sectMask[i] > 0){
            if(sectMask[i] == 255)
                last_position = i + 1;
            else
                last_position = i;
            break;
        }
    }
    int last_ip = sectIp[last_position];
    if(std::find(blackList.begin(), blackList.end(), last_ip) != blackList.end()){ // last ip number in black list
        return false;
    }
    return true;
}

// check if valid gateway
bool ModifyIP::checkGateWay(std::vector<int> sectMask, std::vector<int> sectIp, std::vector<int> sectGateway){
    // get subnetwork position
    int last_position = 3;
    for(int i = 3; i >= 1; i --){
        if(sectMask[i] > 0){
            if(sectMask[i] == 255)
                last_position = i + 1;
            else
                last_position = i;
            break;
        }
    }
    // ip should be same as gateway up to last position
    bool unmatched = false;
    for(int i = 0; i < last_position; i ++){
        if(sectIp[i] != sectGateway[i]){
            unmatched = true;
            break;
        }
    }
    return !unmatched;
}


void ModifyIP::on_cgi_requestFinished(const QByteArray &response, int requestType){
    qDebug() << "CGI request Success. rquest type = " << getCgiRequestDesc(requestType) << ", Response = " << response;
    if(requestType == e_cgi_get_eth){
        _eth = parseEthernetResponse(QString::fromUtf8(response));

        // update the IP address
        _dev_ip = _eth.ipaddr;
        _ip_sets.ip = _eth.ipaddr;
        _ip_sets.ip_dns = _eth.dns1;
        _ip_sets.ip_dns2 = _eth.dns2;
        _ip_sets.ip_mask = _eth.netmask;
        _ip_sets.ip_gateway = _eth.gateway;
        show_ip();
    }
    if(requestType == e_cgi_set_eth){
        promp_error_message(this, SET_NETCFG_SUCCESS);
        if(_ip_sets.ip != _dev_ip)
        {
            emit(signal_reset_ip2(_ip_sets.ip));
        }
        close();


    }
}

void ModifyIP::on_cgi_requestFailed(const QString& error, int requestType){
    qDebug() << "CGI request failed. rquest type = " << getCgiRequestDesc(requestType);
    if(requestType == e_cgi_get_eth){
        promp_error_message(this, "读取ETH失败");
    }
    else if(requestType == e_cgi_set_eth){
        promp_error_message(this, SET_NETCFG_FAILED);
        close();
    }
}

QString ModifyIP::getCgiRequestDesc(uint8_t reqType){
    QString reqDes = "Unknown request";
    switch(reqType){
        case  e_cgi_login:
            reqDes = "LOGIN";
            break;
        case e_cgi_get_eth:
            reqDes = "GET_ETH";
            break;
        case e_cgi_set_eth:
            reqDes = "SET_ETH";
            break;
        default:
            break;
    }
    return reqDes;
}


EthernetInfo ModifyIP::parseEthernetResponse(const QString &response)
{
    EthernetInfo info;

    // Remove HTML tags and normalize newlines
    QString clean = response;
    clean.replace("<html>", "").replace("</html>", "");
    clean.replace("\\n", "\n").replace("\\r", "");
    clean = clean.trimmed();

    // Split into lines
    QStringList lines = clean.split('\n', Qt::SkipEmptyParts);

    for (QString line : lines) {
        line = line.trimmed();
        if (!line.contains('=')) continue;

        const QStringList parts = line.split('=', Qt::SkipEmptyParts);
        if (parts.size() < 2) continue;

        QString key = parts[0].trimmed();
        QString value = parts[1].trimmed();

        // Match fields
        if (key.endsWith("ipaddr"))       info.ipaddr = value;
        else if (key.endsWith("netmask")) info.netmask = value;
        else if (key.endsWith("gateway")) info.gateway = value;
        else if (key.endsWith("dhcp"))    info.dhcp = (value == "1");
        else if (key.endsWith("autoDns")) info.autoDns = (value == "1");
        else if (key.endsWith("dns1"))    info.dns1 = value;
        else if (key.endsWith("dns2"))    info.dns2 = value;
        else if (key.endsWith("mac"))     info.mac = value;
        else if (key.endsWith("ERR.des")) info.errDescription = value;
    }

    return info;
}

bool ModifyIP::modifyHisiIpBySearchSdk(const QString &user, const QString &pwd)
{
    if (GSF_DEV_SEARCH_INIT() != 0) {
        qDebug() << "failed to initialize search sdk";
        return false;
    }

    bool success = false;
    HisiSearchContext ctx;
    memset(&ctx.sadpInfo, 0, sizeof(ctx.sadpInfo));
    ctx.targetIp = _dev_ip;
    ctx.found = false;

    GSF_DevSearchParam param;
    ZeroMemory(&param, sizeof(param));
    param.Callback = hisiModifySearchCb;
    param.UserParam = &ctx;

    int startRet = GSF_DEV_SEARCH_START(&param);
    if (startRet != 0) {
        qDebug() << "failed to start hisi search. ret =" << startRet;
        GSF_DEV_SEARCH_UNINIT();
        return false;
    }

    QElapsedTimer timer;
    timer.start();
    while (!ctx.found && timer.elapsed() < 5000) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        QThread::msleep(100);
    }
    GSF_DEV_SEARCH_STOP();

    if (!ctx.found) {
        qDebug() << "target hisi device not found by local search" << _dev_ip;
        GSF_DEV_SEARCH_UNINIT();
        return false;
    }

    NET_DEV_MODIFY_NET netCfg;
    ZeroMemory(&netCfg, sizeof(netCfg));
    memcpy_s(netCfg.SadpID, sizeof(netCfg.SadpID), ctx.sadpInfo.SadpID, sizeof(ctx.sadpInfo.SadpID));
    memcpy_s(&netCfg.EthCfg, sizeof(netCfg.EthCfg), &ctx.sadpInfo.EthCfg, sizeof(ctx.sadpInfo.EthCfg));
    memcpy_s(&netCfg.WifiNIC, sizeof(netCfg.WifiNIC), &ctx.sadpInfo.WifiNIC, sizeof(ctx.sadpInfo.WifiNIC));
    memcpy_s(&netCfg.ServiceCfg, sizeof(netCfg.ServiceCfg), &ctx.sadpInfo.ServiceCfg, sizeof(ctx.sadpInfo.ServiceCfg));
    netCfg.ProtolType = ctx.sadpInfo.ProtolType;

    std::string userStd = user.toStdString();
    std::string pwdStd = pwd.toStdString();
    strncpy_s(netCfg.UserName, sizeof(netCfg.UserName), userStd.c_str(), _TRUNCATE);
    strncpy_s(netCfg.Passwd, sizeof(netCfg.Passwd), pwdStd.c_str(), _TRUNCATE);
    if (netCfg.EthCfg.NICCount == 0) {
        netCfg.EthCfg.NICCount = 1;
    }
    strncpy_s(netCfg.EthCfg.EthNICs[0].IPAddr.IpV4, sizeof(netCfg.EthCfg.EthNICs[0].IPAddr.IpV4), _ip->getStdIP().c_str(), _TRUNCATE);
    strncpy_s(netCfg.EthCfg.EthNICs[0].Netmask.IpV4, sizeof(netCfg.EthCfg.EthNICs[0].Netmask.IpV4), _ip_mask->getStdIP().c_str(), _TRUNCATE);
    strncpy_s(netCfg.EthCfg.EthNICs[0].Gateway.IpV4, sizeof(netCfg.EthCfg.EthNICs[0].Gateway.IpV4), _ip_gateway->getStdIP().c_str(), _TRUNCATE);
    strncpy_s(netCfg.EthCfg.EthNICs[0].DNS1.IpV4, sizeof(netCfg.EthCfg.EthNICs[0].DNS1.IpV4), _ip_dns->getStdIP().c_str(), _TRUNCATE);
    strncpy_s(netCfg.EthCfg.EthNICs[0].DNS2.IpV4, sizeof(netCfg.EthCfg.EthNICs[0].DNS2.IpV4), _ip_dns2->getStdIP().c_str(), _TRUNCATE);
    netCfg.EthCfg.EthNICs[0].EnableDHCP = 0;
    netCfg.EthCfg.EthNICs[0].EnableAutoDNS = 0;

    GsfSetInfoFn setInfoProc = resolveSetInfoProc();
    if (setInfoProc == nullptr) {
        GSF_DEV_SEARCH_UNINIT();
        return false;
    }

    int setRet = setInfoProc(&netCfg, TRUE);
    success = (setRet == 0);
    if (!success) {
        qDebug() << "failed to set hisi ip by sdk, ret =" << setRet;
    }
    qDebug() << "success to set hisi ip by sdk, ret =";
    GSF_DEV_SEARCH_UNINIT();
    return success;
}


void ModifyIP::on_btn_reset_ip_clicked()
{
    // check validity
    if(!_ip->isValidIP())
    {
        promp_error_message(this, "错误：无效IP");
        return;
    }
    std::vector<int> black_suffix;
    if(!_ip_mask->isValidMask(black_suffix))
    {
        promp_error_message(this, "错误：无效子网掩码");
        return;
    }
    if(!_ip_gateway->isValidIP())
    {
        promp_error_message(this, "错误：无效网关");
        return;
    }
    if(!_ip_dns->isValidIP()){
        promp_error_message(this, "错误：无效DNS");
        return;
    }

    std::vector<int> sectionIP = _ip->getSections();
    std::vector<int> sectionMask = _ip_mask->getSections();
    std::vector<int> sectionGate = _ip_gateway->getSections();

    qDebug() << "try to reset the camera IP. Device IP = "<< _dev_ip << "camera type = " << _camType;
    if(!checkIpSets(sectionMask, black_suffix, sectionIP)){
        promp_error_message(this, "错误：掩码不允许此IP");
        return;
    }
    if(!checkGateWay(sectionMask, sectionIP, sectionGate)){
        promp_error_message(this, "错误：IP和网关不在同一个网段");
        return;
    }
    // promp_error_message(this, "IP校检成功");

    if(_camType == e_dev_jetson || _camType == e_dev_c308x){
        _ip_sets.ip = _ip->getIP();
        _ip_sets.ip_mask = _ip_mask->getIP();
        _ip_sets.ip_gateway = _ip_gateway->getIP();
        _ip_sets.ip_dns = _ip_dns->getIP();
        emit(signal_reset_ip(_ip_sets));
        return;
    }
    else if(_camType == e_dev_rv1126){
        if(!_ip_dns2->isValidIP()){
            promp_error_message(this, "错误：无效备选DNS");
            return;
        }
        if(_g_handle <= 0){
            qDebug() << "failed to init client library";
            promp_error_message(this, INVALID_LIBRARY);
            return;
        }
        bool res = login_device();
        if (!res)
        {
            qDebug() << "Login Server Failed!\n";
            promp_error_message(this, LOGIN_FAILED);
            return;
        }

        // change the ip address
        NetworkConf_t netInfo, setInfo;
        memset(&setInfo, 0, sizeof(setInfo));

        strcpy_s(setInfo.szIpAddr, _ip->getStdIP().c_str());
        strcpy_s(setInfo.szNetMask, _ip_mask->getStdIP().c_str());
        strcpy_s(setInfo.szGateWay, _ip_gateway->getStdIP().c_str());
        strcpy_s(setInfo.szDns1, _ip_dns->getStdIP().c_str());
        strcpy_s(setInfo.szDns2, _ip_dns2->getStdIP().c_str());
        setInfo.iDhcpFlag = 0;
        bool bRet = SetNetworkCfg(_g_handle, &setInfo);
        if (bRet == FALSE)
        {
            promp_error_message(this, SET_NETCFG_FAILED);
            logout_device();
            close();
        }
        else{
            promp_error_message(this, SET_NETCFG_SUCCESS);
            _ip_sets = IP_SET(_ip->getIP(), _ip_mask->getIP(), _ip_gateway->getIP(), _ip_dns->getIP(), _ip_dns2->getIP());
            if(_ip_sets.ip != _dev_ip)
            {
                emit(signal_reset_ip2(_ip_sets.ip));
            }
            logout_device();
            close();
        }
    }
    else if(_camType == e_dev_hisi3519){// cgi interface
        QString name = ui->input_name->text();
        QString pwd = ui->input_pwd->text();
        if(name.isEmpty()){
             promp_error_message(this, "错误：无效用户名");
             return;
        }
        if(pwd.isEmpty()){
            promp_error_message(this, "错误：无效密码");
            return;
        }
        if(!_ip_dns2->isValidIP()){
            promp_error_message(this, "错误：无效备选DNS");
            return;
        }

        if(!modifyHisiIpBySearchSdk(name, pwd)){
            // fallback to old CGI path when sdk doesn't export SET_INFO
            _eth.ipaddr = _ip->getIP();
            _eth.dns1 = _ip_dns->getIP();
            _eth.dns2 = _ip_dns2->getIP();
            _eth.netmask = _ip_mask->getIP();
            _eth.gateway = _ip_gateway->getIP();
            _eth.dhcp = 0;
            _cgiClient->sendModifyIprequest(_dev_ip, name, pwd, _eth);
            return;
        }

        _ip_sets = IP_SET(_ip->getIP(), _ip_mask->getIP(), _ip_gateway->getIP(), _ip_dns->getIP(), _ip_dns2->getIP());
        promp_error_message(this, SET_NETCFG_SUCCESS);
        if(_ip_sets.ip != _dev_ip)
        {
            emit(signal_reset_ip2(_ip_sets.ip));
        }
        close();

    }
}


void        ModifyIP::on_btn_cancel_clicked()
{

    if(_camType == e_dev_jetson || _camType == e_dev_c308x){ // click cancel button
        close();
    }
    else if(_camType == e_dev_rv1126){ // read network configuration from rv1126
        _ip_sets.clear_sub_info();
        show_ip();
        if(_g_handle <= 0){
            qDebug() << "failed to init client library";
            promp_error_message(this, INVALID_LIBRARY);
            return;
        }

        NetworkConf_t netInfo;
        memset(&netInfo, 0, sizeof(netInfo));
        bool res = login_device();
        if (!res)
        {
            qDebug() << "Login Server Failed!\n";
            promp_error_message(this, LOGIN_FAILED);
            return;
        }

        //获取网络配置;
        BOOL bRet = GetNetworkCfg(_g_handle, &netInfo);
        if (bRet == FALSE)
        {
            qDebug() << "Get Network Info Failed";
            promp_error_message(this, GET_NETCFG_FAILED);
            logout_device();
            return;
        }
        logout_device();
        std::string mask = netInfo.szNetMask;
        std::string gate = netInfo.szGateWay;
        std::string dns = netInfo.szDns1;
        std::string dns2 = netInfo.szDns2;
        _ip_sets.ip = QString().fromUtf8(netInfo.szIpAddr);
        _ip_sets.ip_mask = QString().fromUtf8(netInfo.szNetMask);
        _ip_sets.ip_gateway = QString().fromUtf8(netInfo.szGateWay);
        _ip_sets.ip_dns = QString().fromUtf8(netInfo.szDns1);
        _ip_sets.ip_dns2 = QString().fromUtf8(netInfo.szDns2);
        show_ip();


        // success to change device ip address
        // promp_error_message(this, SET_NETCFG_SUCCESS);
    }

    else if(_camType == e_dev_hisi3519){ // read ip configuration by CGI interface
        // check username and input
        QString name = ui->input_name->text();
        QString pwd = ui->input_pwd->text();
        if(name.isEmpty()){
             promp_error_message(this, "错误：无效用户名");
             return;
        }
        if(pwd.isEmpty()){
            promp_error_message(this, "错误：无效密码");
            return;
        }

        _ip_sets.clear_sub_info();
        show_ip();

        // send cgi request to get ethernet configuration
        _cgiClient->sendReadEthernetRequest(_dev_ip, name, pwd);
    }
}

bool        ModifyIP:: login_device(){
    if(_g_handle <= 0){
        qDebug() << "invalid library handle";
        return false;
    }
    bool result = false;
    char szSvrIp[20] = {0,};
    std::string ss = _dev_ip.toStdString();
    strcpy_s(szSvrIp, ss.c_str());

    int nPort = ROCKCHIP_PORT;
    char szUser[20] = "admin";
    char szPwd[20] = "123456";
    NetworkConf_t netInfo;
    memset(&netInfo, 0, sizeof(netInfo));
    int nRet = Login(_g_handle, szSvrIp, nPort, szUser, szPwd);
    result = (nRet >= 0)? true: false;
    return result;
}

void        ModifyIP:: logout_device(){
    if(_g_handle <= 0)
    {
        qDebug() << "invalid library handle";
        return;
    }
    Logout(_g_handle);
}


void        ModifyIP::show_ip(){
    _ip->SetText(_ip_sets.ip);
    _ip_mask->SetText(_ip_sets.ip_mask);
    _ip_gateway->SetText(_ip_sets.ip_gateway);
    _ip_dns->SetText(_ip_sets.ip_dns);
    _ip_dns2->SetText(_ip_sets.ip_dns2);
}


void        ModifyIP::reset_device_ip(bool res){
    if(res){
        promp_error_message(this, "主板IP设置成功");
        close();
        return;
    }
    promp_error_message(this, "主板IP设置失败");
}

void        ModifyIP::closeEvent(QCloseEvent *event){
    if(DEVTYPE == e_dev_rv1126){
        if(_g_handle > 0){
            DeInitClientLib(_g_handle);
            _g_handle = 0;
        }
    }
   event->accept();
}

