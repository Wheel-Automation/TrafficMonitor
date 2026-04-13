#include <QDebug>
#include <QMessageBox>
#include "dlg_login.h"
#include "ui_dlg_login.h"
#include "dlg_update_pwd.h"
#include "dlg_warning.h"
#include "macro.h"

dlg_login::dlg_login(QWidget *parent,
    bool bPrevLogin, // already logged in?
    QString devIP,  // device ip
    QString devPwd, // device password
    DETECT_MODE det_mode    // one direction or tow-drection detection
    ,int nPrevCam):         // prev selected camera index
    QDialog(parent),
    ui(new Ui::dlg_login)
    , _szDevIP(devIP)
    , _szDevPwd(devPwd)
    , _bLogedIn(bPrevLogin)
    ,_nPrevCamera(nPrevCam)

{
    ui->setupUi(this);
    initUI();
    setFixedSize(width(), height());
}

dlg_login::~dlg_login()
{
    delete  device_ip;
    delete ui;
}

void dlg_login::on_btn_ok_clicked()
{
    if(!device_ip->isValidIP())
    {
        promp_error_message(this, "主板IP错误");
        return;
    }
    if(ui->edit_pwd->text().length() <= 0){
        promp_error_message(this, "主板密码错误");
        return;
    }

    QString szIp = device_ip->getIP();
    QString szPwd = ui->edit_pwd->text();
    //// int cam_index = ui->comb_cam_index->currentIndex();
    DETECT_MODE mode = DETECT_ONE_DIRECT;
    emit signal_login(mode, szIp, szPwd, 0);
}

void dlg_login::initUI(){
    setWindowTitle("主板注册管理");
    ui->label_pwd->setVisible(false);
    ui->edit_pwd->setVisible(false);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    device_ip = new CustomIpEditor(ui->frm_ip);
    if(!_bLogedIn)
        ui->btn_reset_pwd->setDisabled(true);
    device_ip->SetText(_szDevIP);
    ui->edit_pwd->setText(_szDevPwd);
    device_ip->setFocus();


}

void dlg_login::on_btn_reset_pwd_clicked()
{
    dlg_update_pwd* dlg = new dlg_update_pwd(this);
    connect(dlg, &dlg_update_pwd::signal_new_pwd, [=](bool bUpdated, QString szNewPwd){
        if(!bUpdated)
            return;
        _szDevPwd= szNewPwd;
        ui->edit_pwd->setText(_szDevPwd);
        emit(update_password(_szDevPwd));
    });
    dlg->exec();
    delete  dlg;
}

void dlg_login::on_log_in_success(bool bSuccess){
    QString msg = (bSuccess)? "注册成功": "注册失败";
    //QMessageBox::question( this, "提示", tr(msg.toUtf8()),QMessageBox::Yes, QMessageBox::Yes);
    promp_error_message(this, msg);
    if(bSuccess){
        ui->btn_reset_pwd->setEnabled(true);
        ui->btn_ok->setEnabled(true);
        close();
    }
}

void dlg_login::on_reset_pwd_event(bool bSuccess){
    QString msg = (bSuccess) ? "更新密码成功" :"更新密码失败";
    // QMessageBox::question( this, "提示", tr(msg.toUtf8()),QMessageBox::Yes, QMessageBox::Yes);
    promp_error_message(this, msg);
}

void dlg_login::on_btn_close_clicked()
{
    close();
}
