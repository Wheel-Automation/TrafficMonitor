#include "warningdlg.h"
#include "ui_warningdlg.h"
#include <QDialog>

WarningDlg::WarningDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WarningDlg)
{
    ui->setupUi(this);
}

WarningDlg::~WarningDlg()
{
    delete ui;
}
