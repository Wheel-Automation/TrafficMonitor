#include <QHBoxLayout>
#include "state_panel.h"
#include "ui_state_panel.h"
#include "led.h"
#include <QDebug>

state_panel::state_panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::state_panel)
{
    ui->setupUi(this);
    initUI();
}

state_panel::~state_panel()
{
    delete ui;
}

void state_panel::initUI(){

    // set icon
    //button->setIcon(QIcon(":/icons/..."));
    //button->setIconSize(QSize(65, 65));
    //ui->btnPlay->setIcon(QIcon(":/asset/play.png"));
    //ui->btnPlay->setIconSize(QSize(20, 20));

    iconPlay = "border-image:url(:/asset/on.png)";
    iconStop = "border-image:url(:/asset/off.png)";
    iconConnect = "border-image:url(:/asset/connected.png)";
    iconDisconnect = "border-image:url(:/asset/disconnect.png)";

    ui->btnPlay->setCheckable(true);
    onChangePlayState(e_disable);
    onChangeConnectionState(e_disable);
 }

void state_panel::onChangeConnectionState(int state){

    if(state == e_turnon){
        //qDebug("device connected : ");
        ui->btnConnect->setVisible(true);
        ui->btnConnect->setStyleSheet(iconConnect);
    }
    else if(state == e_turnoff){
        //qDebug("device disconnected : ");
        ui->btnConnect->setVisible(true);
        ui->btnConnect->setStyleSheet(iconDisconnect);
    }
    else{
        //qDebug("device state unknown: ");
        ui->btnConnect->setVisible(true);
        ui->btnConnect->setStyleSheet(iconDisconnect);
    }
}

void   state_panel:: onChangePlayState(int state){

    if(state == e_turnon){
        ui->btnPlay->setVisible(true);
        ui->btnPlay->setStyleSheet(iconPlay);
        ui->btnPlay->setChecked(true);
    }
    else if(state == e_turnoff){
        ui->btnPlay->setVisible(true);
        ui->btnPlay->setStyleSheet(iconStop);
        ui->btnPlay->setChecked(false);
    }
    else{
        ui->btnPlay->setVisible(true);
        ui->btnPlay->setStyleSheet(iconStop);
        ui->btnPlay->setChecked(false);
    }
    _playState = LET_STATE(state);
}


void state_panel::set_deviceip(QString device_ip){
    ui->label_jetson->setText(device_ip);
}
void    state_panel::set_cameraip(QString camera_ip){
    // ui->label_camera->setText(camera_ip);
}

void state_panel::on_btnPlay_toggled(bool checked)
{
    if(checked){
        ui->btnPlay->setVisible(true);
        ui->btnPlay->setStyleSheet(iconPlay);
        emit showStream();
    }
    else{
        ui->btnPlay->setVisible(true);
        ui->btnPlay->setStyleSheet(iconStop);
        emit hideStream();
    }
}

