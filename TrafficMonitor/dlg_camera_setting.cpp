#include <QRegExpValidator>
#include <QDebug>
#include "dlg_camera_setting.h"
#include "ui_dlg_camera_setting.h"
#include "utils.h"



dlg_camera_setting::dlg_camera_setting(QWidget *parent, PANEL_PARM panel) :
    QDialog(parent),
    ui(new Ui::dlg_camera_setting),
    // ip_cam(nullptr),
    ip_server(nullptr),
    _panel(panel)
{
    ui->setupUi(this);
    initUI();
    if (panel.cam_index == 0)
        setWindowTitle("控制摄像头1");
    else
        setWindowTitle("控制摄像头2");
    setFixedSize(width(), height());
}

void dlg_camera_setting::initUI(){
    //ip_cam = new CustomIpEditor(ui->frm_cam_ip);
    ip_server = new CustomIpEditor(ui->frm_server_ip);


    ui->pushButton->setContentsMargins(0,0, 0, 0);

    // add direction combobox
    QIcon *icon_left = new QIcon(":/asset/left_arrow.png");
    QIcon *icon_right = new QIcon(":/asset/right_arrow.png");
    ui->comb_direct->addItem(*icon_right,"    右往左");
    ui->comb_direct->addItem(*icon_left, "   左往右");
    ui->comb_direct->setCurrentIndex(0);

    qDebug() << "device name: "<<_panel.dev_name<<"\n";

    ui->line_dev_name->setText(_panel.dev_name);

    ui->comb_direct->setCurrentIndex((int)_panel.e_direct);
    ip_server->SetText(_panel.serverip);

    ui->label_devname->setVisible(false);
    ui->line_dev_name->setVisible(false);

#if 0
    qDebug() << "camera ip address" << _panel.camip << endl;
    if(QString::compare(_panel.camip, QString("0.0.0.0"), Qt::CaseInsensitive) == 0){
        ip_cam->enableItem(false);
        //ui->edit_cam_pwd->setEnabled(false);
        qDebug() << "success to disable item one new device";
    }
#endif
}

dlg_camera_setting::~dlg_camera_setting()
{
    freeMemory();
    delete ui;
}

void dlg_camera_setting::freeMemory(){
    // if(ip_cam)
    //     delete ip_cam;
    if(ip_server)
        delete ip_server;
}

void dlg_camera_setting::on_pushButton_clicked() // okay button clicked
{
#if 0
    if(!ip_cam->isValidIP()){
        QString msg = "摄像头IP错误";
        promp_error_message(this, msg);

        return;
    }
#endif

    if(!ip_server->isValidIP()){
        promp_error_message(this, "服务器IP错误");
        return;
    }

    _panel.dev_name = ui->line_dev_name->text();
    _panel.camip = QString("0.0.0.0");
    _panel.e_direct = DIRECTION(ui->comb_direct->currentIndex());
    _panel.serverip = ip_server->getIP();

    emit(signal_get_param(_panel));

    //close();
}

void dlg_camera_setting::on_pushButton_2_clicked() // cancel button clicked
{
    close();
}

void dlg_camera_setting::on_set_param(bool bSucces){
    if(bSucces){
        promp_error_message(this, "设置参数成功");
        close();
    }
   else
        promp_error_message(this, "设置参数失败");
}
void dlg_camera_setting::enable_editable(bool bEditable){
    //ui->pushButton->setEnabled(bEditable);
}
