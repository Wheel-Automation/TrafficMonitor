#include <QPainter>
#include <QDebug>
#include "led.h"
#include "ui_led.h"
#include "utils.h"
led::led(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::led)
{
    m_state = e_disable;

    ui->setupUi(this);
    m_color[e_turnon] = QColor(7, 245, 44);
    m_color[e_turnoff] = QColor(202, 11, 68);

}

led::~led()
{
    delete ui;
}

void led::control_state(LET_STATE state){
    m_state = state;
    this->update();
}

void led::paintEvent(QPaintEvent *event){

    int W = width();
    int H = height();
    int r = H / 2;
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing, true);

    if (m_state == e_disable)
    {
        painter.eraseRect(QRect(0,0,W, H));
    }
    else{
        painter.setBrush(QBrush(m_color[m_state]));
        painter.setPen(QPen(m_color[m_state]));
        painter.drawEllipse(QRect(0, 0, 2 * r, 2 * r));
    }

}
