#include "dlglaunchpassword.h"
#include "ui_dlglaunchpassword.h"

DlgLaunchPassword::DlgLaunchPassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgLaunchPassword),
    _bVerified(false)
{
    ui->setupUi(this);
    QString title = "输入密码";
    setWindowTitle(title);
    setFixedSize(width(), height());
    connect(ui->btn_okay, &QPushButton::released, this, &DlgLaunchPassword::onButtonOkay);
}

DlgLaunchPassword::~DlgLaunchPassword()
{
    delete ui;
}

void DlgLaunchPassword::closeEvent(QCloseEvent *event){

}

void DlgLaunchPassword::onButtonOkay(){
    QString ss = ui->line_pwd->text();
    if(ss == "s123456")
        _bVerified = true;
    close();
    // close();
}
