#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include "logviewer.h"
#include "ui_logviewer.h"
#include "utils.h"



LogViewer::LogViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogViewer),
    m_nInitDateRow(50),
    m_nInitContentRow(50)
{
    ui->setupUi(this);
    setFixedSize(width(), height());
    initUIControls();
    loadLogFileNames();
    setWindowTitle("车胎检测系统日志");
}

LogViewer::~LogViewer()
{
    delete ui;
}

void  LogViewer::initUIControls(){

    ui->logTitle->setText("");
    // init date table view
    ui->tblDate->setColumnCount(2);
    ui->tblDate->setRowCount(m_nInitDateRow);

    ui->tblDate->setColumnWidth(0, 60);
    ui->tblDate->setColumnWidth(1, 80);


    //ui->tableWidget->horizontalScrollBar()->setDisabled(true);
    QString str;
    ui->tblDate->setHorizontalHeaderItem(0, new QTableWidgetItem(str.fromUtf8("序号")));
    ui->tblDate->setHorizontalHeaderItem(1, new QTableWidgetItem(str.fromUtf8("日期")));
    ui->tblDate->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblDate->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblDate->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblDate->horizontalHeader()->setStretchLastSection(true);
    ui->tblDate->resizeRowsToContents();

    ui->tblDate->verticalHeader()->setVisible(false);

     ui->tblDate->setStyleSheet("QTableView::item:selected{ color:rgb(27, 68, 255); background:rgb(180, 180, 180); font-weight:900; }");
     ui->tblDate->horizontalHeader()->setStyleSheet("::section {background-color: #808080; color:white;}");

     // init content table view
     ui->tblContent->setColumnCount(4);
     ui->tblContent->setRowCount(m_nInitDateRow);

     ui->tblContent->setColumnWidth(0, 60);
     ui->tblContent->setColumnWidth(1, 100);
     ui->tblContent->setColumnWidth(2, 40);
     ui->tblContent->setColumnWidth(3, 80);

     ui->tblContent->setHorizontalHeaderItem(0, new QTableWidgetItem(str.fromUtf8("序号")));
     ui->tblContent->setHorizontalHeaderItem(1, new QTableWidgetItem(str.fromUtf8("时间")));
     ui->tblContent->setHorizontalHeaderItem(2, new QTableWidgetItem(str.fromUtf8("车胎数")));
     ui->tblContent->setHorizontalHeaderItem(3, new QTableWidgetItem(str.fromUtf8("车胎类型")));
     ui->tblContent->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
     ui->tblContent->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
     ui->tblContent->setEditTriggers(QAbstractItemView::NoEditTriggers);
     ui->tblContent->horizontalHeader()->setStretchLastSection(true);
     ui->tblContent->resizeRowsToContents();
     ui->tblContent->verticalHeader()->setVisible(false);
     ui->tblContent->setStyleSheet("QTableView::item:selected{ color:rgb(27, 68, 255); background:rgb(180, 180, 180); font-weight:900; }");
     ui->tblContent->horizontalHeader()->setStyleSheet("::section {background-color: #808080; color:white;}");
}

// load only log file names from log folder
void   LogViewer::loadLogFileNames(){
    QString logDir = Utils::getLogHomeDir();
    QStringList deviceList = Utils::findAllSubDirectories(logDir);

    QString message;
    message = QString("log home directory: %1: %2").arg(logDir).arg(deviceList.size());
    log_file(message);

    int cnt = 0;
    for(auto subDir: deviceList){
        QStringList logFiles = Utils::findAllLogFiles(subDir);

        message = QString("log device directory: %1").arg(subDir);
        log_file(message);

        for (QString f : logFiles){
            message = QString("found log files: %1").arg(f);
            log_file(message);
        }
        if(logFiles.size() <= 0){
            log_file("Cannot find any log files");
            continue;
        }
        //m_logFileMap[subDir] = logFiles;
        // add to combbox
        QString ip = Utils::FilePathToName(subDir);
        m_logFileMap.append(qMakePair(ip, logFiles));

        ui->combIP->addItem(ip);
        ui->combIP->setItemData(cnt, QBrush(Qt::white), Qt::TextColorRole);
    }
    if(m_logFileMap.size() > 0){
        ui->combIP->setCurrentIndex(0);
        on_combIP_currentIndexChanged(0);
    }
}

// load contents of one log file
void   LogViewer::loadLogFile(QString logFile){

    QList<LOG_RECORD> records;
    QFile inputFile(logFile);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList elems = line.split(',', Qt::SkipEmptyParts);
          if(elems.size() != 3)
              continue;
          LOG_RECORD rec(elems[0], elems[1], elems[2]);
          records.append(rec);
       }
       inputFile.close();
    }
    // add to log content table
    ui->tblContent->clearContents();
    int nRow = qMax(m_nInitContentRow, records.size());
    ui->tblContent->resizeRowsToContents();
    ui->tblContent->setRowCount(nRow);

    for(int i = 0; i < records.size(); i ++){
        QTableWidgetItem* item;
        item = new QTableWidgetItem(QString("%1").arg(i + 1));
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        ui->tblContent->setItem(i, 0, item);

        item = new QTableWidgetItem(QString("%1").arg(records[i]._time));
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        ui->tblContent->setItem(i, 1, item);

        item = new QTableWidgetItem(QString("%1").arg(records[i]._nWheel));
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        ui->tblContent->setItem(i, 2, item);

        item = new QTableWidgetItem(QString("%1").arg(records[i]._nType));
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        ui->tblContent->setItem(i, 3, item);
    }

}

// change the ip device
void LogViewer::on_combIP_currentIndexChanged(int index)
{
    ui->tblDate->clearContents();
    // add to date table view
    if(index >= m_logFileMap.size())
        return;
    QStringList logFiles = m_logFileMap.at(index).second;
    int nlow = qMax(m_nInitDateRow, logFiles.size());
    ui->tblDate->setRowCount(nlow);

    for(int subCnt = 0; subCnt < logFiles.size(); subCnt ++){
        QString logFileName = logFiles[subCnt];

        QTableWidgetItem* item;
        item = new QTableWidgetItem(QString("%1").arg(subCnt + 1));
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        ui->tblDate->setItem(subCnt,0, item);

        QString fileName = Utils::FilePathToName(logFileName);
        QString purName = QFileInfo(fileName).completeBaseName();
        item = new QTableWidgetItem(purName);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        ui->tblDate->setItem(subCnt, 1, item);
    }

    qDebug() << "success to update log file list";

}

void LogViewer::on_tblDate_cellDoubleClicked(int row, int column)
{
    //ui->logTitle->setText("");
    qDebug() << "clicked row:" << row;
    int nDeviceIndex = ui->combIP->currentIndex();
    if(nDeviceIndex >= m_logFileMap.size())
        return;
    QStringList files = m_logFileMap.at(nDeviceIndex).second;
    if(row >= files.size())
        return;
    QString logFile = files[row];

    loadLogFile(logFile);

    // update log file title
    QString ip = ui->combIP->currentText();
    QString date = ui->tblDate->item(row,1)->text();

    QStringList elems = date.split('-', Qt::SkipEmptyParts);
    if(elems.size() == 3){
        QString temp = QString("%1年%2月%3日").arg(elems[0]).arg(elems[1]).arg(elems[2]);
        date = temp;
    }

    QString logTitle = QString("%1 / %2 日志").arg(ip).arg(date);
    ui->logTitle->setText(logTitle);
}

