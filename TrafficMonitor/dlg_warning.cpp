#include "dlg_warning.h"
#include "ui_dlg_warning.h"
#include <QObject>
#include <QDebug>




dlg_warning::dlg_warning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_warning)
{
    ui->setupUi(this);
    QObject::connect(this, SIGNAL(signal_close()), this, SLOT(close()));
    //connect(ui->btn_okay, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    setFixedSize(width(), height());
}

dlg_warning::dlg_warning(QWidget* parent, QString msg, PROMPT_TYPE type):
QDialog(parent),
ui(new Ui::dlg_warning)
{
    ui->setupUi(this);
    ui->label->setText(msg);
    if(type == e_prompt_note)
        setWindowTitle("通知");
    if(type == e_prompt_error)
        setWindowTitle("提示");
    if(type == e_prompt_warning)
        setWindowTitle("警告");
}

dlg_warning::~dlg_warning()
{
    delete ui;
}

void dlg_warning::on_btn_okay_clicked()
{
    //emit (signal_close());
    close();
}


