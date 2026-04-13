#include <iostream>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QScrollBar>
#include <QHeaderView>
#include <QGuiApplication>
#include <QDebug>


#include "control_panel.h"
#include "ui_control_panel.h"
#include "cumstomipeditor.h"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "mainwindow.h"
#include    "utils.h"



control_panel::control_panel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::control_panel)
{
    ui->setupUi(this);
    InitUI();
    EnableDetailInput(false);
}

control_panel::~control_panel()
{
    free_memory();
}

void control_panel::free_memory(){
    delete ui;
    delete control_cam_ip;
    delete control_jet_ip;
}

void control_panel::InitUI(){
    control_jet_ip = new CustomIpEditor(ui->frm_jet_ip);
    control_cam_ip = new CustomIpEditor(ui->frm_cam_ip);

    ui->edit_jet_pwd->setStyleSheet("border: 1px solid rgb(80,80, 80);"
                                    "background-color:rgb(140, 140, 140);");
    ui->edit_cam_pwd->setStyleSheet("border: 1px solid rgb(80,80,80);"
                                    "background-color:rgb(140, 140, 140);");

    // add direction combobox
    QIcon *icon_left = new QIcon(":/asset/left_arrow.png");
    QIcon *icon_right = new QIcon(":/asset/right_arrow.png");
    ui->comb_direction->addItem(*icon_left,"    左往右");
    ui->comb_direction->addItem(*icon_right, "   右往左");
    ui->comb_direction->setCurrentIndex(0);
    ui->comb_direction->setStyleSheet("background:rgb(140, 140, 140); "
                                "selection-background-color: rgb(73, 198, 182);");

}



// jetson connection request
void control_panel::on_btn_connect_clicked()
{
    panel.jtip  = control_jet_ip->getIP();
    panel.jtpwd = ui->edit_jet_pwd->text();
    emit signal_jetson_connnect(panel);
}



PANEL_PARM control_panel::get_panel_info(){
    return panel;
}


std::string  control_panel::get_cam_url(){
    QString szIP = control_cam_ip->getIP();
    QString szPwd = ui->edit_cam_pwd->text();
    QString szCamName = ui->edit_cam_name->text();
    QString url = QString("rtsp://%1:%2@%3:554/Streaming/Channels/102").arg(szCamName, szPwd, szIP);
    std::string std_url = url.toLocal8Bit().constData();
    return std_url;
}


// parameter changed successfully
void control_panel::on_btn_ok_clicked()
{
    bool bVerified = false;
    std::string cam_url = get_cam_url();
    try {
        cv::VideoCapture cap(cam_url);
        if(!cap.isOpened()){
            qDebug()<<"Cannot open rtsp streaming: "<< QString::fromStdString(cam_url);
        }
        else{
            bVerified = true;
            cap.release();
        }
    } catch (...) {
        qDebug()<<"Cannot open rtsp streaming: "<< QString::fromStdString(cam_url);
        return;
    }
    if(bVerified){ // verified url
        panel.camip = control_cam_ip->getIP();
        panel.campwd = ui->edit_cam_pwd->text();
        panel.camname = ui->edit_cam_name->text();
        panel.e_direct = DIRECTION(ui->comb_direction->currentIndex());
        emit signal_panel_ok(panel);
    }
    else{ // invalid url, revert into original
        qDebug()<<"No Implement error in control_panel::on_btn_ok_clicked";
        control_cam_ip->SetText(panel.camip);
        ui->edit_cam_pwd->setText(panel.campwd);
        ui->edit_cam_name->setText(panel.camname);
    }
}

// canceling the parameter changing
void control_panel::on_btn_cancel_clicked()
{
    emit signal_panel_cancel();
}


void control_panel::set_parameter(PANEL_PARM& param){
    panel = param;
    // set a value into widget
    EnableDetailInput(true);
    control_cam_ip->SetText(panel.camip);
    ui->edit_cam_pwd->setText(panel.campwd);
    ui->edit_cam_name->setText(panel.camname);

    if(panel.e_direct == DIRECTION_LR)
        ui->comb_direction->setCurrentIndex(0);
    else
        ui->comb_direction->setCurrentIndex(1);

}

// Enable the subwidget for input details
void control_panel::EnableDetailInput(bool benable){
    control_cam_ip->setEnabled(benable);
    control_cam_ip->setEnable_sub(benable);
    ui->edit_cam_pwd->setEnabled(benable);
    ui->edit_cam_name->setEnabled(benable);
    ui->comb_direction->setEnabled(benable);
    ui->btn_ok->setEnabled(benable);
    ui->btn_cancel->setEnabled(benable);
}
