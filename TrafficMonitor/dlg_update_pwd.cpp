#include "dlg_update_pwd.h"
#include "ui_dlg_update_pwd.h"
#include "utils.h"
dlg_update_pwd::dlg_update_pwd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_update_pwd)
{
    ui->setupUi(this);
    initUI();
    setFixedSize(width(), height());
}

dlg_update_pwd::~dlg_update_pwd()
{
    delete ui;
}

void dlg_update_pwd::initUI(){
    setWindowTitle("更新密码");
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    ui->btn_ok->setDisabled(true);
    szCurPwd = "";
    szRePwd = "";
}

void dlg_update_pwd::on_btn_ok_clicked()
{
    if(szCurPwd.length() <= 0){
        promp_error_message(this, "请输入有效密码");
        return;
    }
    emit(signal_new_pwd(true, szCurPwd));
    close();
}

void dlg_update_pwd::on_btn_cancel_clicked()
{
    emit(signal_new_pwd(false, ""));
    close();
}


void dlg_update_pwd::on_edit_pwd_textChanged(const QString &cur_pwd)
{
    szCurPwd = ui->edit_pwd->text();
    ui->edit_cfm_pwd->setText("");
    ui->btn_ok->setEnabled(false);
}

void dlg_update_pwd::on_edit_cfm_pwd_textChanged(const QString &cur_repwd)
{
    szRePwd = ui->edit_cfm_pwd->text();
    if(szRePwd == szCurPwd && szRePwd.length() > 0)
        ui->btn_ok->setEnabled(true);
    else
        ui->btn_ok->setEnabled(false);
}
