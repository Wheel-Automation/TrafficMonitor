#include "mytablewidget.h"
#include <QDebug>
#include <QHeaderView>

MyTableWidget::MyTableWidget(QWidget* parent):
    QTableWidget(parent)
{
    initTableWidget();
}

void MyTableWidget::wheelEvent(QWheelEvent* event){
    qDebug() << "table widget mouse wheel event";
    emit userActionTiggered("MyTableWidget");
}

void MyTableWidget::initTableWidget(){
    setColumnCount(6);
    setRowCount(300);

    setColumnWidth(0, 60);
    setColumnWidth(1, 90);
    setColumnWidth(2, 30);
    setColumnWidth(3, 150);
    setColumnWidth(4, 60);
    //ui->tableWidget->setColumnWidth(3, 40);

    //ui->tableWidget->horizontalScrollBar()->setDisabled(true);
    QString str;
    setHorizontalHeaderItem(0, new QTableWidgetItem(str.fromUtf8("序号")));
    setHorizontalHeaderItem(1, new QTableWidgetItem(str.fromUtf8("主板/电脑IP")));
    setHorizontalHeaderItem(2, new QTableWidgetItem(str.fromUtf8("摄像头序号")));
    setHorizontalHeaderItem(3, new QTableWidgetItem(str.fromUtf8("经过时间")));
    setHorizontalHeaderItem(4, new QTableWidgetItem(str.fromUtf8("轮数")));
    setHorizontalHeaderItem(5, new QTableWidgetItem(str.fromUtf8("车轮类型")));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->horizontalHeader()->setStretchLastSection(true);
    resizeRowsToContents();

    this->verticalHeader()->setVisible(false);
}


