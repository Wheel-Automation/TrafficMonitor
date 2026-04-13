#include "warning_window.h"
#include "ui_warning_window.h"

warning_window::warning_window(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::warning_window)
{
    ui->setupUi(this);
}

warning_window::~warning_window()
{
    delete ui;
}
