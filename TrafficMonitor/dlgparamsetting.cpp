#include <QDebug>
#include "dlgparamsetting.h"
#include "ui_dlgparamsetting.h"
#include "utils.h"

DlgParamSetting::DlgParamSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgParamSetting)
{
    TURN_ON_MIN = 16;       // turn on LED between 17~21 clock
    TURN_ON_MAX = 20;
    TURN_OFF_MIN = 4;       // turn off LED between 4~8 clock
    TURN_OFF_MAX = 8;
    MAX_TIMEOUT = 4;     // max timeout value 8 second

    ui->setupUi(this);
    setFixedSize(width(), height());
}

DlgParamSetting::DlgParamSetting(QWidget *parent, PANEL_PARM paramSets):
    QDialog(parent),
    ui(new Ui::DlgParamSetting),
    _param(paramSets)
{
    TURN_ON_MIN = 17;       // turn on LED between 17~21 clock
    TURN_ON_MAX = 21;
    TURN_OFF_MIN = 4;       // turn off LED between 4~8 clock
    TURN_OFF_MAX = 8;
    MAX_TIMEOUT = 4;     // max timeout value 8 second
    ui->setupUi(this);
    initParameters();
    setWindowTitle("设置辅助参数");


}

void DlgParamSetting::initParameters(){

    for (int i = TURN_ON_MIN; i <= TURN_ON_MAX; i ++) {
        QString item = QString("%1").arg(i);
        ui->comboTurnOnTime->addItem(item);
    }

    for (int i = TURN_OFF_MIN; i <= TURN_OFF_MAX; i ++) {
        QString item = QString("%1").arg(i);
        ui->comboTurnOffTime->addItem(item);
    }

    for(int i = 0; i <= MAX_TIMEOUT; i ++){
        QString item = QString("%1").arg(i);
        ui->comboTimeOut->addItem(item);
    }

    int index = qMin(qMax(_param.ledOnTime - TURN_ON_MIN, 0), TURN_ON_MAX-TURN_ON_MIN);
    ui->comboTurnOnTime->setCurrentIndex(index);
    index = qMin(qMax(_param.ledOffTime - TURN_OFF_MIN, 0), TURN_OFF_MAX-TURN_OFF_MIN);
    ui->comboTurnOffTime->setCurrentIndex(index);
    index =  qMin(qMax(_param.timeOut - 4, 0), MAX_TIMEOUT);
    ui->comboTimeOut->setCurrentIndex(index);
    ui->checkLed->setStyleSheet("QCheckBox { color: white }");
    ui->check5Wheel->setStyleSheet("QCheckBox { color: white }");
    ui->checkTimeEnable->setStyleSheet("QCheckBox { color: white }");

    visibleLedTimeSetting(_param.enable_led);
    ui->check5Wheel->setChecked(_param.enable5Wheel);
    ui->checkTimeEnable->setChecked(_param.enableTimeCalib);

    ui->label_9->hide();
    ui->checkTimeEnable->hide();

    uint8_t devType = _param.dev_type;
    if(devType == e_dev_hisi3519){ // hide time calibration and control light logic
        ui->frmLight->setVisible(false);
        ui->frmStartLight->setVisible(false);
        ui->frmStopLight->setVisible(false);
    }
    ui->frmCalibrate->setVisible(false);

}


DlgParamSetting::~DlgParamSetting()
{
    delete ui;
}

void DlgParamSetting:: onParamSetResult( bool bSuccess){
    qDebug() << "success to receive subsequent parameters at DlgParamSetting" << endl;
    QString msg = (bSuccess) ? "参数更新成功" :"参数更新失败";
    promp_error_message(this, msg);
    close();
}


void DlgParamSetting::visibleLedTimeSetting(bool bEnable){
    if(bEnable){
        ui->label->show();
        ui->label_2->show();
        ui->label_4->show();
        ui->label_5->show();
        ui->comboTurnOnTime->show();
        ui->comboTurnOffTime->show();
        ui->checkLed->setChecked(true);
    }
    else{
        ui->label->hide();
        ui->label_2->hide();
        ui->label_4->hide();
        ui->label_5->hide();
        ui->comboTurnOnTime->hide();
        ui->comboTurnOffTime->hide();
        ui->checkLed->setChecked(false);
    }

}

void DlgParamSetting::on_btnOK_clicked()
{
    int nValue = ui->comboTurnOnTime->currentIndex() + TURN_ON_MIN;
    _param.ledOnTime = nValue;
    nValue = ui->comboTurnOffTime->currentIndex() + TURN_OFF_MIN;
    _param.ledOffTime = nValue;
    nValue = ui->comboTimeOut->currentIndex() + 4;
    _param.timeOut = nValue;
    _param.enable_led = ui->checkLed->isChecked();
    _param.enable5Wheel = ui->check5Wheel->isChecked();
    _param.enableTimeCalib = ui->checkTimeEnable->isChecked();
    qDebug() << "Modify parameters: " << _param.ledOnTime << _param.ledOffTime << _param.timeOut;
    emit modifyParam(_param);
}

void DlgParamSetting::on_btnClose_clicked()
{
    close();
}

void DlgParamSetting::on_checkLed_stateChanged(int checkState)
{
    if(checkState == 0){
        visibleLedTimeSetting(false);
    }
    else{
        qDebug("eanble combobox");
        visibleLedTimeSetting(true);

    }
}
