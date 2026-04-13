#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QScrollBar>
#include <QtMath>
#include <QHeaderView>
#include <QThread>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QBrush>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QTimer>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QKeySequence>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "control_panel.h"
#include "camViewerWorker.h"
#include "connectworker.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "QReadWriteLock"
#include "cameraview.h"
#include "utils.h"
#include "dlg_warning.h"
#include "dlg_login.h"
#include "modifyip.h"
#include "encryptlib.h"
#include "macro.h"
#include "state_panel.h"
#include "longconnectserver.h"
#include "dlgipsearch.h"
#include "dlglaunchpassword.h"
#include "dlgparamsetting.h"
#include "dlgproxysetting.h"
#include "setting/logviewer.h"
#include "setting/logsettingdlg.h"
#include "com/com.h"
#include "proxyworker.h"

#define     REG_POINT_HEIGHT    0.95
#define     NOT_IMPLEMENT_ERROR     "Not Implement error at"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,_pick_index(-1)
    ,dlgLogin(nullptr)
    ,dlg_cam_set(nullptr)
    ,dlg_reset_ip(nullptr)
    ,dlgParamSetting(nullptr)
    ,_bLogin(false)
    ,_bFirstLogin(false)
    ,_bMouseMoved(false)
    ,_nRecords(-1)
    ,m_nRunHours(0)
    ,m_bForceStop(false)
    ,m_isJMG(true)
    ,m_cntLogLine(0)
    ,m_bShowLog(false)
    ,m_logthread(nullptr)
    ,m_bOpendLogWindow(false)
    ,m_nSocketServer(0)
    , m_bEnableProxy(true)
    ,m_bExiting(false)
    ,m_proxyThread(nullptr)
{
    initializeTrayLogic();
    startAssetTimer();
    startProxyThread();
    loadSetting();
    m_camworker = nullptr;
    m_proxyTaskPool.setMaxThreadCount(1);


    qDebug() << "self ip address" << m_ipAddresses;

    initAPPState();
    ui->setupUi(this);
    InitUI();

    connect(this, &MainWindow::changePlayState, ui->panelWrap, &state_panel::onChangePlayState);
    connect(this, &MainWindow::changeConnectState, ui->panelWrap, &state_panel::onChangeConnectionState);

    m_windowSize = QSize(width(), height());
    CreateAction();
    CreateUISignalConnection();
#if AUTO_LOGIN
    startConnectThread(e_get_param);
#endif

    startServerThread();
    startDetectionResultLog();
    log_file("success to initialize");
    m_calib_timer.start();

#if 1   // hide camera view at defult
    ui->frm_play_rgn->setVisible(false);
    ui->textLog->setVisible(true);
#endif
    connectAutoHideConnections();
}


MainWindow::~MainWindow()
{
    free_memory();
}

// make a connection to hide a main window after some time. This time is defined at macro.h
void    MainWindow::connectAutoHideConnections(){

    if(m_viewer != nullptr)
        m_viewer->installEventFilter(this);
    ui->menu->installEventFilter(this);
    ui->menu_tool->installEventFilter(this);
    ui->menu_log->installEventFilter(this);
    ui->centralwidget->installEventFilter(this);
    connect(ui->textLog, SIGNAL(itemClicked(QString)), this, SLOT(onUserActionTriggered(QString)));
    connect(&m_autoHideTimer, &QTimer::timeout, [=](){
        qDebug() << "auto hide main window";
        sendToTrayIcon();
        m_autoHideTimer.stop();
    });
    m_autoHideTimer.start(AUTOHIDE_TIME);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if(obj == ui->menu || obj == ui->menu_tool || obj == ui->menu_log){
        if(event->type() == QEvent::MouseMove)
            onUserActionTriggered("MenuItem");
    }
    else if(obj == m_viewer){
        if(event->type() == QEvent::MouseButtonPress )
            onUserActionTriggered("FrameViewer");
    }
    return QObject::eventFilter(obj, event);
}


void MainWindow::free_memory(){
    m_bExiting = true;
    if(m_camworker){
        enableDisplay(false);
        m_camworker->stop_working();
    }
    delete m_viewer;
    delete ui;
}


void MainWindow::initAPPState(){
    // init roi
    _roi.ptRegion[0] = QPointF(0.1, 0.1);
    _roi.ptRegion[1] = QPointF(0.1, 0.9);
    _roi.ptRegion[2] = QPointF(0.9, 0.9);
    _roi.ptRegion[3] = QPointF(0.9, 0.1);
    _roi.ptRegion[4] = QPointF(0.7, 0.2);
    _roi.ptRegion[5] = QPointF(0.4, 0.15);
    _roi.ptRef = 0.3;
}

void MainWindow::InitUI(){
    // CreateControPanel();
    create_camera_viewer();
    InitListView();

    QString selfIP = GetSelfAddress();
    this->setWindowTitle("主板车胎检测监控系统（SK-V21)");
    ui->textLog->setVisible(false); // play state
    setFixedSize(width(), height());
}

bool  MainWindow::loadSetting(){

#if 0
    QFileInfo check_file(_szSettingFile);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) { // use the saved value
        try {
            QFile inputFile(_szSettingFile);
            if (inputFile.open(QIODevice::ReadOnly))
            {
               QTextStream in(&inputFile);
               int i = 0;
               for ( ;i < 4;i ++) {
                    QString line = in.readLine();
                    if (i == 0)
                        m_param.jtip = line;
                    else if(i == 1)
                        m_param.jtpwd = line;
                    else if(i == 2)
                        m_param.cam_index = 0; //line.toInt();
                    else if(i == 3)
                        m_param._det_mode = DETECT_ONE_DIRECT; // (DETECT_MODE)line.toInt();
               }
               inputFile.close();
            }
            log_file("success to load setting file");

        } catch (...) {
            m_param.jtip = "192.168.101.167";
            m_param.jtpwd = "123456";
            m_param.cam_index = 0;
            m_param._det_mode = DETECT_TWO_DIRECT;
            log_file("exception in loading the setting");
        }
    }
    else { // use the default value
        m_param.jtip = "192.168.101.167";
        m_param.jtpwd = "123456";
        m_param.cam_index = 0;
        m_param._det_mode = DETECT_TWO_DIRECT;
        log_file("load default setting");
    }

    m_param.serverPort = SERVER_PORT;
    m_param.clientPort = SOCKET_PORT;

    m_param.proxy.proxyIP = "127.0.0.1";
    m_param.proxy.proxyPort = 51087;
    m_param.proxy.enableProxy = true;
#else
    m_settingManager.loadSetting(m_param);
    m_settingManager.loadProxySetting(m_proxyTable);
#endif
    return true;

}
bool  MainWindow::SaveSetting(){
#if 0
    QFile file(_szSettingFile);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << m_param.jtip << endl;
        stream << m_param.jtpwd << endl;
        stream << m_param.cam_index << endl;
        stream << m_param._det_mode<< endl;
    }
    log_file("success to save setting");
#else
    m_settingManager.saveSetting(m_param);
    m_settingManager.saveProxySetting(m_proxyTable);
#endif
    return true;
}

// start log writter thread
void    MainWindow::startDetectionResultLog(){

    QThread *thread = new QThread();
    m_logthread = new logWriterThread();
    m_logthread->changeKeepLogDays(m_param.nLogSaveDay);
    m_logthread->moveToThread(thread);
    connect(thread, SIGNAL(started()), m_logthread, SLOT(run()));
    connect(m_logthread, &logWriterThread::finished, [=](){
        m_logthread->deleteLater();
        m_logthread = nullptr;
        thread->quit();
    });
    connect(thread, &QThread::finished, [=](){
        thread->deleteLater();
    });
    thread->start();
}



// Attach control panel
void MainWindow::CreateControPanel(){
#if 0
    c_panel = new control_panel(this);
    auto  panel_layout =  new QHBoxLayout();
    panel_layout->addWidget(c_panel);
    // ui->frm_control_rgn->setLayout(panel_layout);
    panel_layout->setMargin(1);

    QObject::connect(c_panel, SIGNAL(signal_jetson_connnect(PANEL_PARM)), this, SLOT (slot_jetson_connect(PANEL_PARM)));
    QObject::connect(c_panel, SIGNAL(signal_panel_ok(PANEL_PARM)), this, SLOT(slot_panel_ok(PANEL_PARM)));
    QObject::connect(c_panel, SIGNAL(signal_panel_cancel()), this, SLOT(slot_panel_cancel()));
#endif
}

/*
 * check the existance of JMG, if bCheckReamin, write the remaining count
 */
bool MainWindow::check_stability(bool bCheckReamin){
#if 0
#define MULT_CHECK	5
    int nFailed_cnt = 0;
    for(int i = 0; i < MULT_CHECK; i ++)
    {
        bool res = false;
        if(i == 0 && bCheckReamin)
            res = EncryptLib::CheckValidity(true);
        else
            res = EncryptLib::CheckValidity(false);
        if(!res)
            nFailed_cnt += 1;
        Sleep(10);
    }
    if(nFailed_cnt == MULT_CHECK){
        log_file("failed to detect jmg");
        return false;
    }
    log_file("success to load jmg");
    return true;
#else
    return false;
#endif
}

void MainWindow::create_camera_viewer(){
    m_viewer = new CameraView(ui->frm_play_rgn);
    auto view_layout = new QHBoxLayout();
    view_layout->addWidget(m_viewer);
    ui->frm_play_rgn->setLayout(view_layout);
    view_layout->setMargin(1);

    QObject::connect(m_viewer, SIGNAL(signal_mouse_moved(QMouseEvent*)), this, SLOT(slot_mouse_move(QMouseEvent*)));
    QObject::connect(m_viewer, SIGNAL(signal_mouse_pressed(QMouseEvent*)), this, SLOT(slot_mouse_press(QMouseEvent*)));
    QObject::connect(m_viewer, SIGNAL(signal_mouse_released()), this, SLOT(slot_mouse_release()));
}

void MainWindow::InitListView(){
    // wheel count,time
#if 0
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setRowCount(300);

    ui->tableWidget->setColumnWidth(0, 60);
    ui->tableWidget->setColumnWidth(1, 90);
    ui->tableWidget->setColumnWidth(2, 30);
    ui->tableWidget->setColumnWidth(3, 150);
    ui->tableWidget->setColumnWidth(4, 60);
    //ui->tableWidget->setColumnWidth(3, 40);

    //ui->tableWidget->horizontalScrollBar()->setDisabled(true);
    QString str;
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(str.fromUtf8("序号")));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(str.fromUtf8("主板/电脑IP")));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(str.fromUtf8("摄像头序号")));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(str.fromUtf8("经过时间")));
    ui->tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem(str.fromUtf8("轮数")));
    ui->tableWidget->setHorizontalHeaderItem(5, new QTableWidgetItem(str.fromUtf8("车轮类型")));
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->resizeRowsToContents();

    ui->tableWidget->verticalHeader()->setVisible(false);

#endif
    ui->tableWidget->horizontalHeader()->setStyleSheet("::section {background-color: #808080; color:white;}");
    ui->tableWidget->setStyleSheet("QTableView::item:selected{ color:rgb(27, 68, 255); background:rgb(180, 180, 180); font-weight:900; }");


     auto header = ui->tableWidget->horizontalHeader();
     connect(header, &QHeaderView::sectionClicked, [=](int logicalIndex){
         QString text = ui->tableWidget->horizontalHeaderItem(logicalIndex)->text();
        // qDebug() << "qTableWidget item header clocked:" << logicalIndex << text;
        onUserActionTriggered("Table Widget Header clicked");
     });
     connect(ui->tableWidget, &MyTableWidget::userActionTiggered, this, &MainWindow::onUserActionTriggered);
}

void MainWindow::slot_panel_ok(PANEL_PARM panel_param){
    m_param = panel_param;

    // set all parameters into jetson
    startConnectThread(int(e_set_param));

    // if set is okay?
//    if(m_camworker == nullptr                                       // not started yet
//            || m_camworker->get_current_ip() != m_param.camip)
       //startCamThread();

}

void MainWindow::slot_panel_cancel(){
    printf("panel cancel");
}


void MainWindow::slot_jetson_connect(PANEL_PARM param){
    m_param = param;
#if 0
    m_param.jtip = "192.168.101.167";
    m_param.jtpwd = "fengqi-8059";
#endif
    startConnectThread(int(e_get_param));
}


void MainWindow::startConnectThread(int request_type){

    QThread *thread = new QThread();
    connectWorker* connecter = new connectWorker(m_param, _roi, request_type);
    if(request_type == e_calib_time){
        connecter->setProxyMap(m_proxyTable);
    }
    connecter->moveToThread(thread);
    connect(thread, SIGNAL(started()), connecter, SLOT(process()));
    connect(connecter, SIGNAL (finished()), thread, SLOT (quit()));
    connect(connecter, SIGNAL(report(int, int, PANEL_PARM, SETTING)), this, SLOT(slot_socket_report(int, int, PANEL_PARM, SETTING )));
    connect(connecter, &connectWorker::finished, [=]() {
        connecter->deleteLater();

    });
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();
}

void MainWindow::startServerThread(){

    m_ipAddresses = GetSelfMultiAddress();
    qDebug() << "starting socket server thread";
    for(auto hostIP: m_ipAddresses){
        qDebug() << "host ip address" << hostIP;
    }

    m_nSocketServer = m_ipAddresses.size();
    m_socketServer = new Server*[m_nSocketServer];
    for(int i = 0; i < m_nSocketServer; i ++)
        m_socketServer[i] = nullptr;

    for(int cnt = 0; cnt < m_nSocketServer; cnt ++ ){
        m_socketServer[cnt] = new Server();
        m_socketServer[cnt]->SetServerIP(m_ipAddresses[cnt]);
        m_socketServer[cnt]->SetServerPort(uint16_t(m_param.serverPort));
        connect(m_socketServer[cnt], &Server::signal_stop, [=](){
            // m_socketServer[cnt]->deleteLater();
            // m_socketServer[cnt] = nullptr;
        });
        bool bres = connect(m_socketServer[cnt], SIGNAL(signal_received_data(QString, QString, int, int, QString, QString)), this, SLOT(slot_receive_result(QString, QString, int, int, QString, QString)));
        Q_ASSERT(bres);

        bres = connect(m_socketServer[cnt], SIGNAL(signal_recevied_status(int, QString)), this, SLOT(slot_receive_status(int, QString)));
        Q_ASSERT(bres);
        m_socketServer[cnt]->onstart();
    }
}

void    MainWindow:: closeServerThread(){
    qDebug() << "stop socket server thread";
    for(int i = 0; i < m_nSocketServer; i ++){
        if(m_socketServer[i] != nullptr)
        {
            m_socketServer[i]->stopThread();
            QThread::msleep(1000);
            delete  m_socketServer[i];
            m_socketServer[i] = nullptr;
        }
    }
    if(m_socketServer!= nullptr)
        delete[] m_socketServer;
    m_nSocketServer = 0;
    qDebug() << "stopped all socket thread";
}

PANEL_PARM MainWindow::get_panel_param(){
    return m_param;
}

void    MainWindow:: enableProxy(bool bEnable){
    m_proxyLock.lockForWrite();
    m_bEnableProxy = bEnable;
    m_proxyLock.unlock();
}


void   MainWindow:: sendProxyData(QString camIP, int nWheels, QString szWheelType, QString szBuffer){
    bool bEnableProxy =false;
    m_proxyLock.lockForRead();
    bEnableProxy = m_bEnableProxy;
    m_proxyLock.unlock();

    if(!bEnableProxy)
        return;

    int nWheelType = szWheelType.toInt();

    PROXY_SETTING setting;
    bool bFindProxySetting = false;
    m_proxyLock.lockForRead();
    bFindProxySetting = m_proxyTable.contains(camIP);
    if(bFindProxySetting)
        setting = m_proxyTable[camIP];
    m_proxyLock.unlock();

    PROXY_MODE proxyMode = setting.proxyMode;
    QString serverIP = setting.proxyIP;
    int serverPort = setting.proxyPort;
    QString comPort = setting.comPort;

    if(!bFindProxySetting || (proxyMode != e_proxy_rs && proxyMode != e_proxy_tcp)){
        qDebug()<<"Cannot find proxy setting for this device:" << camIP;
        szBuffer += " : 禁止转发";
        add_device_log(szBuffer);
        return;
    }

    if(proxyMode == e_proxy_tcp){// send this data to tcp server
        //m_proxyTaskPool.clear();
        proxyWorker * task = new proxyWorker();
        task->setParameter(serverIP, serverPort, szBuffer);
        task->setAutoDelete(true);
        connect(task, &proxyWorker::sentBuffer, this, &MainWindow::onReceivedTcpProxyResult);
        m_proxyTaskPool.start(task);
    }
    else if(proxyMode == e_proxy_rs){ // send this data to com port
        if(m_proxyThread != nullptr)
            m_proxyThread->addData(camIP, comPort, nWheels, nWheelType);
    }
    //add_device_log(szBuffer);
}


void    MainWindow::onReceivedTcpProxyResult(QString szBuffer, QString serverIP, int serverPort, bool bSuccess){
    QString suffix = QString(" : TCP转发到 %1 (%2)").arg(serverIP).arg(serverPort);
    if(!bSuccess)
        suffix += " ,转发失败";
    szBuffer += suffix;
    add_device_log(szBuffer);
}


void  MainWindow::slot_receive_result(QString cam_ip, QString cam_index, int nCrossType, int nWheels, QString strClss, QString szDeviceLog){
    qDebug()<<("receive data in slot")<<endl;

    sendProxyData(cam_ip, nWheels, strClss, szDeviceLog);

    // display on device logwindow
    //add_device_log(szDeviceLog, false);
    // add to log file

    if(m_logthread != nullptr)
        m_logthread->addLog(cam_ip, nWheels, strClss);

    if(!_bLogin)
        return;


    QTableWidget* tbl = ui->tableWidget;

    if (_nRecords > 300 && nCrossType == 1){
        tbl->clearContents();
        _nRecords = -1;
    }
    if (nCrossType == 1){ // add new detection
        _nRecords += 1;
        QTableWidgetItem* item;
        item = new QTableWidgetItem(QString("%1").arg(_nRecords + 1));
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        tbl->setItem(_nRecords, 0, item);

        item = new QTableWidgetItem(cam_ip);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        tbl->setItem(_nRecords, 1, item);

        item = new QTableWidgetItem(cam_index);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        tbl->setItem(_nRecords, 2, item);

        QDateTime date = QDateTime::currentDateTime();

        QString formattedTime = date.toString("hh:mm:ss yyyy-MM-dd");

        item = new QTableWidgetItem(formattedTime);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        tbl->setItem(_nRecords, 3, item);

        item = new QTableWidgetItem(QString("%1").arg(nWheels));
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        tbl->setItem(_nRecords, 4,item);

        item = new QTableWidgetItem(strClss);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setForeground(QBrush(QColor(255, 255, 255)));
        tbl->setItem(_nRecords, 5, item);

        //tbl->scrollToItem(item, QAbstractItemView::ScrollHint::PositionAtCenter);
    }
    else if (nCrossType == 2 && _nRecords > 0){
        QTableWidgetItem* item = tbl->item(_nRecords, 3);
        int nPrevWhl = (item->text()).toInt();
        nPrevWhl += nWheels;
        item->setText(QString("%1").arg(nPrevWhl));
        tbl->setItem(_nRecords, 3, item);
    }
    log_file("recieve detection result");
}

/*
 * receive the camera state from jetson
*/
void MainWindow::slot_receive_status(int cam_index, QString szDeviceLog){
    m_network_timer.restart();

    add_device_log(szDeviceLog);
    if(cam_index == e_run_1st_cam || cam_index == e_run_dbl_cam){ //
        m_bFirstReceived[0] = true;
        m_cam1_status_timer.restart();
    }
    if(cam_index == e_run_2nd_cam || cam_index == e_run_dbl_cam){
        m_bFirstReceived[1] = true;
        m_cam2_status_timer.restart();
    }
    log_file("receive status report");
}

/*
 *  function: add_device_log
 *  param: szBuffer: log content
 *          bStatusLog: if true, status log(show as green color), if false, detection result(show as yellow color)
*/
void  MainWindow::add_device_log(QString szBuffer, bool bStatusLog ){
    // send time calibration request
    qint64 nElapsed = m_calib_timer.elapsed()/1000;
    if(nElapsed > 60 * 60){// send time-calib request at every 1 hour
          startConnectThread(e_calib_time);
          m_calib_timer.restart();
          return;
    }

    // if(!m_bShowLog)
    //     return;

    m_cntLogLine ++;
    if(m_cntLogLine > 1000)
    {
        m_cntLogLine = 0;
        ui->textLog->clear();
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    szBuffer = "  " + timestamp  + " : " + szBuffer;
    ui->textLog->appendPlainText(szBuffer);
    ui->textLog->moveCursor (QTextCursor::End);
}

void    MainWindow:: stopCamera(){
    if(m_camworker != nullptr){
        m_camworker->stop_working();
    }
}

void MainWindow::startCamThread(){

    if( m_camworker != nullptr ) // already camera thread is running
    {
        // QString newIP = get_panel_param().jtip;
        // enableDisplay(false);
        // m_camworker->setCameraIP(m_param.jtip);
        qDebug() << "still camera is running";
        changePlayState(e_turnon);
        return;
    }

    qDebug() << "creating camera thread" << endl;
    QThread *thread = new QThread();
    m_camworker = new camViewerWorker();

    // set camera property to camviewer
    m_camworker->setCameraIP(m_param.jtip);
    m_camworker->setDeviceType(m_param.dev_type);

    CHECK_TRUE(connect(thread, &QThread::started, m_camworker, &camViewerWorker::process));

    CHECK_TRUE(connect(m_camworker, &camViewerWorker::finished, [=](){
        m_camworker->deleteLater();
        m_camworker = nullptr;
        thread->quit();
        qDebug()<<"Called slot for camera thread  terminated";
        if(!m_bExiting)
            changePlayState(e_turnoff);
    }));

    CHECK_TRUE(connect(m_camworker, SIGNAL(signal_one_frame(uchar* , int , int , int )), this, SLOT(display_one_frame(uchar* , int , int , int))));
    CHECK_TRUE(connect(m_camworker, &camViewerWorker::connected, [=](){
        changePlayState(e_turnon);
        enableDisplay(true);
        ui->detectAction->setDisabled(false);
        ui->networkAction->setDisabled(false);
        log_file("camera connected");

    }));
    CHECK_TRUE(connect(m_camworker, &camViewerWorker::rtspConnectFailed, [=](){
        qDebug() << "Failed to connect rtsp";
        enableDisplay(false);
        changePlayState(e_turnoff);

    } ));

    CHECK_TRUE(connect(thread, &QThread::finished, [=](){
        thread->deleteLater();
        qDebug()<<"delete thread";
    }));

    m_camworker->moveToThread(thread);
    thread->start();
    qDebug() <<"started camera started. camera thread lanuched";
}




void  MainWindow::slot_camera_connected(bool bconnected){

    return;
}
void MainWindow::display_one_frame(uchar* _inbuf, int w, int h, int c, bool bDrawSetting){


        if (!_bEnabledisplay)
            return;
        if(m_camworker == nullptr || _inbuf == nullptr)
            return;
        try {
            int frame_size = w * h * 3;
            uchar* buffer = new uchar[frame_size];

            QReadWriteLock   lock;
            lock.lockForRead();
            memcpy(buffer, _inbuf, frame_size);
            lock.unlock();

            cv::Mat img(cv::Size(w, h), CV_8UC3, buffer);
            int W = m_viewer->size().width();
            int H = m_viewer->size().height();
            cv::resize(img, img, cv::Size(W, H));
            QImage qimg(img.data, W, H, c * W, QImage::Format_RGB888);
            if(bDrawSetting)
                draw_setting(qimg, W, H);
            QPixmap qpix = QPixmap::fromImage(qimg);
            m_viewer->setPixmap(qpix);
            delete[] buffer;
        } catch (...) {
            log_file("exception at display_one_frame");
        }


}


void MainWindow::draw_setting(QImage& image, int w, int h){

    QBrush brLine = QBrush(QColor(81, 224,88,200));
    QBrush brNode = QBrush(QColor(81, 224,88,200));
    QPen penLine = QPen(brLine, 1, Qt::DashDotLine);
    QPainter painter(&image);
    painter.setPen(penLine);
    painter.setBrush(brNode);

    QPoint corners[6];
    int i = 0;
    for (i = 0; i < 6; i ++) {
        corners[i] = QPoint(_roi.ptRegion[i].rx() * w, _roi.ptRegion[i].ry() * h );
    }

    for (i = 0; i < 6; i ++) {
        int next = (i + 1) % 6;
        painter.drawLine(corners[i], corners[next]);
    }

    penLine = QPen(QBrush(QColor(245, 59, 159)), 1, Qt::SolidLine);
    brNode = QBrush(QColor(245, 59, 159));
    painter.setPen(penLine);
    painter.setBrush(brNode);

    int nodeR = 5;
    for (int i = 0; i < 6; i ++) {
        QRect nR(QPoint(corners[i] - QPoint(nodeR, nodeR)), QSize(2 * nodeR, 2 * nodeR));
        painter.drawEllipse(nR);
    }

    penLine = QPen(QBrush(QColor(84, 59, 159)), 3, Qt::SolidLine);
    painter.setPen(penLine);
    // draw ref line
    int x = qMin(qMax(int(_roi.ptRef * w), 30), w - 30);
    int y = h * REG_POINT_HEIGHT;
    int rad = 9;

    painter.drawLine(QPoint(x - rad, y), QPoint(x + rad, y));
    painter.drawLine(QPoint(x, y - rad), QPoint(x, y + rad));
    return;
}



void  MainWindow::slot_mouse_move(QMouseEvent* event){

    if(_pick_index == -1)
        return;

    int W = m_viewer->size().width();
    int H = m_viewer->size().height();
    QPoint ptref(event->x(), event->y());

#define MGN 3

    if (ptref.x() <= 10 || ptref.x() >= W - MGN || ptref.y() <=MGN || ptref.y() >= H - MGN)
        return;

    if(_pick_index < 6)
        _roi.ptRegion[_pick_index ] = QPointF((float)ptref.x() / W, (float)ptref.y() / H);
    else
        _roi.ptRef = (float)ptref.x() / W;
    return;
}


void  MainWindow::slot_mouse_press(QMouseEvent* event){

    if(m_camworker==nullptr)
        return;
    QPoint ptref(event->x(), event->y());
    QPoint ptTemp;

#define NEAREST 20

    int near_dist = 2 * NEAREST;
    int near_index = -1;
    int W = m_viewer->size().width();
    int H = m_viewer->size().height();
    for (int i = 0; i < 6; i ++) {
        ptTemp = QPoint(_roi.ptRegion[i].rx() * W, _roi.ptRegion[i].ry() * H);
        int dist = distance(ptTemp, ptref);
        if (dist < near_dist){
            near_dist = dist;
            near_index = i;
        }
    }
    ptTemp = QPoint(_roi.ptRef * W, H * REG_POINT_HEIGHT);
    int dist = distance(ptTemp, ptref);
    if (dist < near_dist){
        near_dist = dist;
        near_index = 6;
    }

    if(near_dist >= NEAREST)
        return;
    _pick_index = near_index;
    if(_pick_index < 6)
        _roi.ptRegion[_pick_index] = QPointF((float)ptref.x() / W, (float)ptref.y() / H);
    else
        _roi.ptRef = (float)ptref.x() / W;
    return;
}


void MainWindow::slot_mouse_release(){
    if(_pick_index >= 0){
        _bMouseMoved = true;
        startConnectThread(e_set_param);
    }
    _pick_index = -1;
}

/*
 * report from socket thread
 * bsuccess: 0: failed, 1: success, in set setting mode, this value is used to check the setting result
 * report_type: report type
 * pna
 */
void  MainWindow::slot_socket_report(int nResponse, int rpt_type, PANEL_PARM panel, SETTING setting){
    if(nResponse == SETTUP_FAILED || nResponse == PWD_INCORRECT || nResponse == ONE_DIRECT_BOARD || nResponse == DBL_DIRECT_BOARD ) // connection failed
    {
        qDebug()<<"failed to send data to device";
        QString msg;
        if (rpt_type == e_get_param){   // login failed
            _bLogin = false;

            enableDisplay(false);

            // stopCamera();
            changePlayState(e_disable);

            ui->signupAction->setEnabled(true);
            ui->detectAction->setDisabled(true);
            ui->networkAction->setDisabled(true);
            ui->restartDevice->setDisabled(true);
            ui->paramsetAction->setDisabled(true);

            if (nResponse == PWD_INCORRECT){
                promp_error_message(this, "密码错误");
                ui->detectAction->setDisabled(false);
                ui->networkAction->setDisabled(false);

            }
            else if(nResponse == ONE_DIRECT_BOARD){
                promp_error_message(this, "此主板支持单向检测");
                ui->detectAction->setDisabled(false);
                ui->networkAction->setDisabled(false);
            }
            else if(nResponse == DBL_DIRECT_BOARD){
                promp_error_message(this, "此主板支持双向检测");
                ui->detectAction->setDisabled(false);
                ui->networkAction->setDisabled(false);
            }
            if(dlgLogin != nullptr)
                dlgLogin->on_log_in_success(false);

            m_bFirstReceived[0] = m_bFirstReceived[1] = false;
            log_file("login failed");

        }
        else if(rpt_type == e_set_param){
            if(dlgLogin != nullptr){ // password change event in login window
                dlgLogin->on_reset_pwd_event(false);
            }
            else if(dlgParamSetting != nullptr){
                dlgParamSetting->onParamSetResult(false);
            }
            else{
                promp_error_message(this, "设置主板参数失败了");
            }
            log_file("failed to set parameters");
        }
        else if(rpt_type == e_reset_dev_pwd){
            if(dlgLogin){ // password reset failed
                dlgLogin->on_reset_pwd_event(false);
            }
            log_file("failed to reset device password");
        }
        else if(rpt_type == e_reset_ip){
            if(dlg_reset_ip){
                dlg_reset_ip->reset_device_ip(false);
            }
            log_file("failed to reset device ip");
        }
        else if(rpt_type == e_restart_device){
            log_file("failed to restart the device");
        }

        UpdateStatePanel();
        return;
    }
    if(rpt_type == e_get_param){ // login event
        if(dlgLogin != nullptr)
            dlgLogin->on_log_in_success(true);
        ui->signupAction->setEnabled(true);
        ui->detectAction->setEnabled(true);
        ui->networkAction->setEnabled(true);
        ui->restartDevice->setEnabled(true);
        ui->paramsetAction->setEnabled(true);

        _bLogin = true;
        if(!_bFirstLogin){// this is a first login
            changePlayState(e_turnoff);
        }
        else{ // this is a manual login
            changePlayState(e_turnon);
        }
        _bFirstLogin = true;

        _nRecords = -1;
        ui->tableWidget->clearContents();

        m_param = panel;
        // update proxy setting
        QString deviceIP = m_param.jtip;
        if(m_proxyTable.contains(deviceIP)){ // this is known ip
            m_param.proxy = m_proxyTable[deviceIP];
            m_proxyTable[deviceIP].password = m_param.jtpwd;
        }
        else{ // unknown device, set a default proxy setting
            PROXY_SETTING setting;
            m_proxyTable[deviceIP] = setting;
            m_proxyTable[deviceIP].password = m_param.jtpwd;
            m_param.proxy = setting;
        }
        m_param.proxy.password = panel.jtpwd;
        SaveSetting();

        _roi = setting;
        memcpy_s(_roi.ptRegion, sizeof(QPoint) * 6,  setting.ptRegion, sizeof(QPoint) * 6);
        _roi.ptRef = setting.ptRef;

        m_bFirstReceived[0] = m_bFirstReceived[1] = false;
        if(nResponse == CAMERA_RUNNING)
        {
            // startCamThread();

        }
        else if (nResponse == CAMERA_NOT_RUNNING)
        {
            promp_error_message(this, "无法连接摄像头");
            ui->detectAction->setDisabled(false);
            ui->networkAction->setDisabled(false);
        }
        UpdateStatePanel();
        log_file("success to login");
        return;
    }
    else if(rpt_type == e_set_param){
        qDebug()<<"success to set param to device";
        if(dlgLogin){ // result from change password
            qDebug()<<"success to set param to device-1";
            dlgLogin->on_reset_pwd_event(true);
        }
        else{
            if(nResponse == SETTUP_ROI_SUCCESS) {// update roi success
                qDebug()<<"success to set param to device-2";
                qDebug()<<"success to set param to device-4";
                if(dlg_cam_set != nullptr){
                    qDebug()<<"success to set param to device-5";
                    dlg_cam_set->close();
                }
                if(dlgParamSetting != nullptr){
                    qDebug()<<"success to set param to device-6";
                    dlgParamSetting->close();
                }

                if(!_bMouseMoved){
                    promp_error_message(this, "主板参数设置成功");
                    qDebug()<<"success to set param to device-3";
                }
                _bMouseMoved = false;

            }
            else if(nResponse == CAMERA_NOT_RUNNING){ // failed to reconnect camera on jetson
                // stop camera
                enableDisplay(false);
                changePlayState(e_turnoff);
                promp_error_message(this, "错误： 主板摄像头链接失败了");
            }
            else if(nResponse == CAMERA_RUNNING){ // camera changed
                if(dlg_cam_set != nullptr){
                    dlg_cam_set->close();
                }
                if(dlgParamSetting != nullptr){
                    dlgParamSetting->close();
                }
                promp_error_message(this, "主板摄像头链接成功了");
                // startCamThread();
            }
            else if(nResponse == SETUP_CAMERA_IP_REPEATED){
                promp_error_message(this, "错误: IP地址重复了");
            }
        }
        UpdateStatePanel();
        log_file("success to set parameters");
    }
    else if(rpt_type == e_reset_dev_pwd){ // reset password success
        if(dlgLogin){ // password reset failed
            dlgLogin->on_reset_pwd_event(true);
        }
        else
            promp_error_message(this, "注册成功");

        UpdateStatePanel();
        log_file("success to reset device password");
    }
    else if(rpt_type == e_reset_ip){
        if(dlg_reset_ip)
            dlg_reset_ip->reset_device_ip(true);
        else
            promp_error_message(this, "主板IP设置成功,主板重启中. 重新链接主板");
        UpdateStatePanel();
        log_file("success to reset device ip");
    }
    else if(rpt_type == e_restart_device){
        promp_error_message(this, "重启主板成功");
        UpdateStatePanel();
        log_file("success to restart device");
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event) {

}


void MainWindow::CreateMenubar(){
    menuMain = menuBar()->addMenu("管理");
    menuMain->addAction(actLoginDevice);
    menuMain->addAction(actManangeCamera);
    menuMain->addAction(actResetIP);
    ui->detectAction->setDisabled(true);
    actResetIP->setDisabled(true);
}

void MainWindow::CreateUISignalConnection(){
    connect(ui->panelWrap, &state_panel::showStream, this, &MainWindow::onShowStream);
    connect(ui->panelWrap, &state_panel::hideStream, this, &MainWindow::onHideStream);
}

void MainWindow::onShowStream(){
    onUserActionTriggered("StatePanel");
    ui->frm_play_rgn->setVisible(true);
    ui->textLog->setVisible(false);
    startCamThread();
    enableDisplay(true);
    m_bShowLog = false;
}

void MainWindow::onHideStream(){
    onUserActionTriggered("StatePanel");
    enableDisplay(false);
    stopCamera();
    ui->frm_play_rgn->setVisible(false);
    ui->textLog->setVisible(true);

    m_bShowLog  = true;
}

void MainWindow::CreateAction(){

    // manage menu
    ui->signupAction->setToolTip("注册主板");
    ui->detectAction->setToolTip("设置主板检测参数");
    ui->networkAction->setToolTip("设置主板网路参数");
    ui->restartDevice->setToolTip("重启主板");
    ui->paramsetAction->setToolTip("调整参数");
    ui->signupAction->setDisabled(false);
    ui->detectAction->setDisabled(true);
    ui->networkAction->setDisabled(true);
    ui->restartDevice->setDisabled(true);
    ui->paramsetAction->setDisabled(true);

    connect(ui->signupAction, SIGNAL(triggered()), this, SLOT(action_login_device()));
    connect(ui->detectAction, SIGNAL(triggered()), this, SLOT(action_camera_manage()));
    connect(ui->networkAction, SIGNAL(triggered()), this, SLOT(action_reset_ip()));
    connect(ui->restartDevice, SIGNAL(triggered()), this, SLOT(action_restart_device()));
    connect(ui->paramsetAction, SIGNAL(triggered()), this, SLOT(action_parameter_setting()));

    // tool menus
    ui->IpSearchAction->setToolTip("主板IP搜索");
    ui->clientSettingAction->setToolTip("设置客户端IP和端口");
    connect(ui->IpSearchAction, SIGNAL(triggered()), this, SLOT(action_ip_search()));
    connect(ui->clientSettingAction, SIGNAL(triggered()), this, SLOT(action_proxy_setting()));

    // log menu
    ui->logSetting->setToolTip("日志相关设置");
    ui->logView->setToolTip("打开日志窗口");
    connect(ui->logSetting, &QAction::triggered, this, &MainWindow::action_log_setting);
    connect(ui->logView, &QAction::triggered, this, &MainWindow::action_log_view);

}


void MainWindow::action_ip_search(){
    // qFatal("reset ip event occured");
    qDebug() << "reset ip button occured";
    dlgipsearch* ipdlg = new dlgipsearch(this);
    ipdlg->exec();
    delete ipdlg;
}

void    MainWindow::startProxyThread(){

    QThread*   thread = new QThread();
    m_proxyThread = new proxyThread();
    m_proxyThread->moveToThread(thread);

    connect(thread, &QThread::started, m_proxyThread, &proxyThread::process);
    connect(m_proxyThread, &proxyThread::finished, [=](){
        m_proxyThread->deleteLater();
        thread->quit();
    });
    connect(thread, &QThread::finished, [=](){
        thread->deleteLater();
    });
    connect(m_proxyThread, &proxyThread::comportClosed, this,  &MainWindow::onComportClosed);
    connect(m_proxyThread, &proxyThread::proxiedData, this, &MainWindow::onComportDataTransfered);

    thread->start();
}

void    MainWindow::onComportDataTransfered(QString camIP, QString portName, int nW, int nType, QString transferedBuffer){

    QString szString;
    if(transferedBuffer.length() > 0 ){
        szString = QString("摄像头【%1】检测车辆：车胎数(%2)【胎型%3】-转发到%4【%5】").arg(camIP).arg(nW).arg(nType).arg(portName).arg(transferedBuffer);
    }
    else{
        szString = QString("摄像头【%1】检测车辆：车胎数(%2)【胎型%3】-转发到%4. 打不开串口").arg(camIP).arg(nW).arg(nType).arg(portName);
    }
    add_device_log(szString);

}


void MainWindow::onComportClosed(){
    dlgProxySetting* dlg = new dlgProxySetting(m_param.proxy, this);
    connect(dlg, &dlgProxySetting::modifyProxy, [=](PROXY_SETTING proxySetting){
        qDebug() << "proxy setting was changed";
        enableProxy(true);

        m_param.proxy = proxySetting;
        m_proxyLock.lockForWrite();
        m_proxyTable[m_param.jtip] = proxySetting;
        m_proxyLock.unlock();

        SaveSetting();
        m_proxyThread->unlockComport();


    });

    connect(dlg, &dlgProxySetting::unmodifyProxy,[=]() {
        enableProxy(true);
        m_proxyThread->unlockComport();
    });

    connect(dlg, &dlgProxySetting::rejected, [=](){
        enableProxy(true);
        m_proxyThread->unlockComport();
    });

    dlg->exec();
    delete  dlg;
}

void MainWindow::action_proxy_setting(){
    qDebug() << "client proxy setting" ;
    enableProxy(false);
    // close the comport of the current login device
    if(m_proxyThread == nullptr){
        return;
    }
    QString comport = m_param.proxy.comPort;
    m_proxyThread->lockComport(comport);
    return;

}

void    MainWindow::action_log_setting(){
    qDebug() << "change log setting";

    LogSettingDlg* dlg = new LogSettingDlg();
    dlg->setKeepDay(m_param.nLogSaveDay);
   connect(dlg, &LogSettingDlg::upDateSaveDay, [=](int nSaveDay){
        qDebug() << "Update Save day" << nSaveDay;
        m_logthread->changeKeepLogDays(nSaveDay);
        m_param.nLogSaveDay = nSaveDay;
        SaveSetting();
    });
    dlg->exec();
}

void    MainWindow::action_log_view(){
    if(m_bOpendLogWindow){
        qDebug() << "logWindow is already opened";
        return;
    }
    qDebug() << "create log viewer window";
    LogViewer* logViewer = new LogViewer();
    logViewer->show();
}


void MainWindow::startAssetTimer(){

    m_check_status_timer.start(1000 * 5); // every 5 seconds
    connect(&m_check_status_timer, &QTimer::timeout, this, &MainWindow::slot_check_status_connect);

    m_cam1_status_timer.start();
    m_cam2_status_timer.start();
    m_network_timer.start();
    m_port_timer.start();
}

void MainWindow::slot_check_jmg_connect(){
    m_nRunHours = (m_nRunHours + 1) % 24;
    bool bCheckRemain = false;
    if (m_nRunHours == 0)
        bCheckRemain = true;
    m_isJMG  = check_stability(bCheckRemain);
    EnableMonitor(true);
}

void MainWindow::slot_check_status_connect(){

    int nElapsed = int(m_network_timer.elapsed() / 1000);
    if(nElapsed > NETWORK_RESTART_TIME){        // restart network
        qDebug() << "restarting socket server";
        closeServerThread();
        startServerThread();
        m_network_timer.restart();
    }

#if 0
    nElapsed = int(m_port_timer.elapsed() / 1000);
    if(nElapsed > PORT_CHECK_TIME){
        for(int i = 0; i < m_nSocketServer; i ++){ // check the server which doesn't occupy port
            if(m_socketServer[i]->isStarted())
                continue;
            m_socketServer[i]->onstart();
            QString serverIP = m_socketServer[i]->GetServerIP();
            qDebug() << "restart server" << serverIP;
        }
        m_port_timer.restart();
    }
#endif

#if 1
    // send login request for time calibration
    int nSecs1 = m_cam1_status_timer.elapsed() /1000;
    if(nSecs1 > m_param.heartRate * 2 ){ // close first camera
       if(_bLogin)
            changeConnectState(e_turnoff);
        else
           changeConnectState(e_disable);

    }
    else if(m_bFirstReceived[0]){ // open the lst led, and received at least one signal
        changeConnectState(e_turnon);
    }

#endif
}


void MainWindow::action_login_device(){
    dlgLogin = new dlg_login(this, _bLogin, m_param.jtip, m_param.jtpwd, m_param._det_mode, m_param.cam_index);
    connect(dlgLogin, &dlg_login::signal_login, [=](DETECT_MODE det_mode, QString devIP, QString devPwd, int index){ // login device
        m_param._det_mode = det_mode;
        m_param.jtip = devIP;
        m_param.jtpwd = devPwd;
        m_param.cam_index = index;
        startConnectThread(e_get_param);
    });
    connect(dlgLogin, &dlg_login::update_password, [=](QString newPassword){
        m_param.jtpwd = newPassword;
        startConnectThread(e_reset_dev_pwd);
    });
    dlgLogin->exec();
    delete dlgLogin;
    dlgLogin = nullptr;
 }


void MainWindow::action_camera_manage(){
    dlg_cam_set = new dlg_camera_setting(this, m_param);
    connect(dlg_cam_set, &dlg_camera_setting::signal_get_param, [=](PANEL_PARM panel){
        m_param.dev_name = panel.dev_name;
        m_param.camip = panel.camip;
        m_param.e_direct = panel.e_direct;
        m_param.serverip = panel.serverip;
        startConnectThread(e_set_param);
    });
    dlg_cam_set->exec();
    delete  dlg_cam_set;
    dlg_cam_set = nullptr;
}

void MainWindow::action_reset_ip(){
    QString ip = m_param.jtip;
    QString ipMask = "255.255.255.0";
    QString ipGateway ;
    QString ipdns = m_param.jt_new_dns;
    int camType = m_param.dev_type;
    qDebug() << "open ip edit windows. caemra type = " << camType;
    if(camType == e_dev_jetson || camType == e_dev_c308x){
        if(!GetGateWay(ip, ipGateway)){
            promp_error_message(this, "无效主板IP地址");
            return;
        }
        IP_SET ipsets(ip, ipMask, ipGateway, ipdns );

        dlg_reset_ip = new ModifyIP(this, ipsets, camType);

        connect(dlg_reset_ip, &ModifyIP::signal_reset_ip, [=](IP_SET ipsets){
            m_param.jt_new_ip = ipsets.ip;
            m_param.jt_new_mask = ipsets.ip_mask;
            m_param.jt_new_gateway = ipsets.ip_gateway;
            m_param.jt_new_dns = ipsets.ip_dns;
            startConnectThread(e_reset_ip);
        });

        dlg_reset_ip->exec();
        delete dlg_reset_ip;
        dlg_reset_ip = nullptr;
    }
    else if(camType == e_dev_rv1126){
        IP_SET ipsets(ip, "", "", "", "");
        dlg_reset_ip = new ModifyIP(this, ipsets, camType);
        connect(dlg_reset_ip, &ModifyIP::signal_reset_ip2, [=](QString new_ip){
            qDebug() << "device ip address has benn updated";
            m_param.jtip = new_ip;
            UpdateStatePanel();
            changePlayState(e_turnoff);
            // startConnectThread(e_get_param);
        });
        dlg_reset_ip->exec();
        delete dlg_reset_ip;
        dlg_reset_ip = nullptr;
    }else if(camType == e_dev_hisi3519){
        IP_SET ipsets(ip, "", "", "", "");
        dlg_reset_ip = new ModifyIP(this, ipsets, camType);
        connect(dlg_reset_ip, &ModifyIP::signal_reset_ip2, [=](QString new_ip){
            qDebug() << "device ip address has benn updated";
            m_param.jtip = new_ip;
            UpdateStatePanel();
            changePlayState(e_turnoff);
            // startConnectThread(e_get_param);
        });
        dlg_reset_ip->exec();
        delete dlg_reset_ip;
        dlg_reset_ip = nullptr;

    }
}

void MainWindow::action_parameter_setting(){
    if(!_bLogin)
        return;

    dlgParamSetting = new DlgParamSetting(this, m_param);

    connect(dlgParamSetting, &DlgParamSetting::modifyParam, [=](PANEL_PARM panel){
        qDebug() << "update a parameer thread" << endl;
        m_param.enable_led = panel.enable_led;
        m_param.ledOnTime = panel.ledOnTime;
        m_param.ledOffTime = panel.ledOffTime;
        m_param.timeOut = panel.timeOut;
        m_param.enable5Wheel = panel.enable5Wheel;
        startConnectThread(e_set_param);
    });

    dlgParamSetting->exec();
    delete dlgParamSetting;
    dlgParamSetting  = nullptr;

}




//restart the device
void MainWindow::action_restart_device(){
    if(!_bLogin)
        return;

    QMessageBox qbox(QMessageBox::Question,tr("通知?"),
                      tr("重启主板?"), QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, this);
    qbox.setButtonText(QMessageBox::Yes, "确定");
    qbox.setButtonText(QMessageBox::No, "不");
    qbox.setButtonText(QMessageBox::Cancel, "取消");
    int selected = qbox.exec();
    if(selected != (int)QMessageBox::Yes)
        return;

    startConnectThread(e_restart_device );
}



void MainWindow::enableDisplay(bool benable){
    m_mutexCamera.lock();
    _bEnabledisplay = benable;
    m_mutexCamera.unlock();

    if(!benable){
        cv::Mat back= cv::Mat::zeros(cv::Size(100, 100), CV_8UC3);
        back = cv::Scalar(80, 80, 80);
        display_one_frame(back.data, back.cols, back.rows, 3, false);
    }
}

/*
 * if jmg exist, enable all action, or not, disable
 * */
bool MainWindow::EnableMonitor(bool bJMG){
    if(!bJMG){
        ui->signupAction->setEnabled(false);
        ui->detectAction->setEnabled(false);
        ui->networkAction->setEnabled(false);
        ui->restartDevice->setEnabled(false);
    }
    return true;
}

void MainWindow::UpdateStatePanel(){

    if(!_bLogin){
        ui->panelWrap->set_deviceip("");
        ui->panelWrap->set_cameraip("");
        changeConnectState(e_disable);
        return;
    }
    // set device ip
    ui->panelWrap->set_deviceip(m_param.jtip);
    // set camera ip
    QString  cam_ip = "";
    if(m_param.cam_index == 0)
        cam_ip = QString("%1:1号").arg(m_param.camip);
    else
        cam_ip = QString("%1:2号").arg(m_param.camip);
    ui->panelWrap->set_cameraip(cam_ip);
}



void MainWindow::initializeTrayLogic(){
    createActions();
    createTrayIcon();

    m_iconNormal = QIcon(":/asset/normal.png");
    m_iconDisconneced = QIcon(":/images/bad.png");

    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::messageClicked);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    setIcon(0);
    
	trayIcon->show(); 
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);

    QString menuStyle(
                "QMenu {"
                    "background-color: rgb(43,43,43);"
                    "color:rgb(199,199,199)"
                 "}"

                "QMenu::item {"
                    "padding: 2px 20px 2px 20px;"
                    "border: 1px solid transparent; "
                    "spacing: 30px;"
                "}"

                "QMenu::item:selected {"
                     "background: rgb(80,80,80);"

                 "}"

                "QMenu::separator {"
                     "height: 2px;"
                     "margin: 2px 5px 2px 4px;"
                 "}"

                "QMenu::indicator {"
                     "width: 20px;"
                     "height: 13px;"
                 "}"
    );
    trayIconMenu->setStyleSheet(menuStyle);

    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
void  MainWindow::onExitProcess(){
    trayIcon->hide();
    SaveSetting();
    if(m_camworker != nullptr){
        m_camworker->stop_working();
    }
    closeServerThread();

    if(m_logthread != nullptr)
        m_logthread->stop();
    if(m_proxyThread != nullptr)
        m_proxyThread->stopThread();
}

void MainWindow::createActions()
{
    restoreAction = new QAction(tr("&轮轴检测软件"), this);
    //connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);
    connect(restoreAction, &QAction::triggered, [=](bool){
        // startCamThread();
        QWidget::showNormal();
    });
    quitAction = new QAction(tr("&退出"), this);
    //connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(quitAction, &QAction::triggered, [=](bool checked){
        qDebug() << "wants to close app";
        QMessageBox qbox(QMessageBox::Question,tr("通知?"),
                          tr("关闭车胎检测系统?"), QMessageBox::No | QMessageBox::Yes, this);
        qbox.setButtonText(QMessageBox::Yes, "确定");
        qbox.setButtonText(QMessageBox::No, "不");


        int selected = qbox.exec();
        if(selected != int(QMessageBox::Yes))
            return;
        else{
            onExitProcess();
            QCoreApplication::quit();
        }
    });
}

void MainWindow::setVisible(bool visible)
{
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}


void MainWindow::setIcon(int index)
{
    if(index != 0 && index != 1){
        qDebug() << "invalid icon index";
        return;
    }
    QIcon icon = m_iconNormal;
    if(index == 1)
        icon = m_iconDisconneced;
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->setToolTip("车胎检测系统");
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        qDebug() << "tray icon double clicked";
        QWidget::showNormal();
		
        if(_bLogin)
            changePlayState(e_turnoff);
        else
            changePlayState(e_disable);

        if(m_autoHideTimer.isActive())
            m_autoHideTimer.stop();
        m_autoHideTimer.start(AUTOHIDE_TIME);

        break;
    case QSystemTrayIcon::MiddleClick:
        qDebug() << "tray icon middle clicked";
        break;
    default:
        ;
    }
}

void MainWindow::showMessage()
{
    qDebug() <<"showMessage function was called"    ;
}


void MainWindow::messageClicked()
{

}

void MainWindow::closeEvent (QCloseEvent *event)
{
#if 0
    QMessageBox qbox(QMessageBox::Question,tr("通知?"),
                      tr("关闭车胎检测系统?"), QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, this);
    qbox.setButtonText(QMessageBox::Yes, "确定");
    qbox.setButtonText(QMessageBox::No, "不");
    qbox.setButtonText(QMessageBox::Cancel, "取消");
    int selected = qbox.exec();
    if(selected != (int)QMessageBox::Yes)
        event->ignore();
    else{
        SaveSetting();
        if(m_camworker != nullptr){
            m_camworker->stop_working();
        }
        event->accept();
    }
#else
    sendToTrayIcon();
    event->ignore();
#endif
}

// send the main window into trayicon
void    MainWindow::sendToTrayIcon(){
	showTrayBallonMessage(QString("正在后台运行"), MSG_INFO, 3);
	hide();
	changePlayState(e_turnoff);
    if (!trayIcon->isVisible()) {
        trayIcon->show();
	}
}

void MainWindow::showTrayBallonMessage(QString message, MSGTYPE msgType, int delay){
    int t = int(msgType);
    if(t < 0 || t >= int(MSGTYPE_MAX)){
        qDebug() << "Invalid message type";
        return;
    }

    QIcon icon;
    QString title;
    if(msgType == MSG_INFO){
        icon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
        title = QString("车胎检测系统-通知");
    }
    else if(msgType == MSG_WARN){
        icon = style()->standardIcon(QStyle::SP_MessageBoxWarning);
        title = QString("车胎检测系统-警告");
    }
    else if(msgType == MSG_ERR){
        icon = style()->standardIcon(QStyle::SP_MessageBoxCritical);
        title = QString("车胎检测系统-错误");
    }
    trayIcon->showMessage(title, message, icon, delay * 1000);
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    onUserActionTriggered("TableWidgetItem clicked");
}

void MainWindow::onUserActionTriggered(QString actionType){

    if(m_autoHideTimer.isActive())
        m_autoHideTimer.stop();
    m_autoHideTimer.start(AUTOHIDE_TIME);
}
