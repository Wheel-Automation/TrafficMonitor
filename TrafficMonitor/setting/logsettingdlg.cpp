#include <QIntValidator>
#include <QMessageBox>
#include "logsettingdlg.h"
#include "ui_logsettingdlg.h"

LogSettingDlg::LogSettingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogSettingDlg)
{
    ui->setupUi(this);
    setWindowTitle("设置日志参数");
    setFixedSize(width(), height());
    ui->editLogDay->setValidator( new QIntValidator(1, 365, this));
    QString style = "QLineEdit{ border-width: 1px; border-style: solid; border-color: rgb(92,92,92); color:rgb(240,240,240)}";
    ui->editLogDay->setStyleSheet(style);
}

LogSettingDlg::~LogSettingDlg()
{
    delete ui;
}

void LogSettingDlg::setKeepDay(int nDay){
    ui->editLogDay->setText(QString("%1").arg(nDay));

}

void LogSettingDlg::on_editLogDay_textChanged(const QString &textSaveDay)
{
}

void LogSettingDlg::on_btnOkay_clicked()
{
    // validate parameter
    if(!checkSaveDay(ui->editLogDay->text())){
        QMessageBox qbox(QMessageBox::Question,tr("通知"),
                          tr("无效参数,该在1~365范围内."), QMessageBox::Yes, this);
        qbox.setButtonText(QMessageBox::Yes, "确定");
        qbox.exec();
        return;
    }
    int nDay = ui->editLogDay->text().toInt();
    emit upDateSaveDay(nDay);
    close();
}

void LogSettingDlg::on_btnCancel_clicked()
{
    close();
}

bool LogSettingDlg::checkSaveDay(QString szSaveDayText){
    bool bOk;
    int nDay = szSaveDayText.toInt(&bOk);
    if(!bOk)
        return false;
    if(nDay <= 0 || nDay >= 366){
        return false;
    }
    return true;
}
