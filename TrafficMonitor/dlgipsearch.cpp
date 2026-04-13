#include <iostream>
#include <QThread>
#include <QCloseEvent>
#include "DevSearchSDK.h"
#include "macro.h"
#include "dlgipsearch.h"
#include "ui_dlgipsearch.h"
#include "device_serach_worker.h"
#include "utils.h"



using namespace std;
static int g_dev_idx = 0;
static dlgipsearch* g_dlgSearch = NULL;

dlgipsearch::dlgipsearch(QWidget *parent, int devType) :
    QDialog(parent),
    ui(new Ui::dlgipsearch),
    _bStarted(false),
    _srch(nullptr),
    _srch_rv(nullptr),
	_cntTime(0),
	_interval(100)
{
    ui->setupUi(this);
    initUI();
    setFixedSize(width(), height());
    _searchTime = 5 * 1000;
	_timer = new QTimer(this); 
	_timer->setInterval(_interval); 

    // init IP search library
    GSF_DEV_SEARCH_INIT();

	connect(_timer, &QTimer::timeout, this, &dlgipsearch::timerEvent); 
    g_dlgSearch = this;
    m_devType = devType;
}

dlgipsearch::~dlgipsearch()
{
    GSF_DEV_SEARCH_UNINIT();
	_timer->stop(); 
    _timer->deleteLater();
    delete ui;
}

/*
 * setup ui:
 */
void dlgipsearch::initUI(){
    setWindowTitle("搜索主板IP");
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    // init start/stop ip address
    QString selfIP = GetSelfAddress();

    // init tableformat
    ui->table_list->setColumnCount(2);
    ui->table_list->setRowCount(10);

    ui->table_list->setColumnWidth(0, 30);
    ui->table_list->setColumnWidth(1, 120);
    // ui->table_list->setColumnWidth(2, 90);

    ui->table_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->table_list->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->table_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_list->horizontalHeader()->setStretchLastSection(true);
    ui->table_list->resizeRowsToContents();
    ui->table_list->verticalHeader()->setVisible(false);

    ui->table_list->setHorizontalHeaderItem(0, new QTableWidgetItem("序号"));
    ui->table_list->setHorizontalHeaderItem(1, new QTableWidgetItem("主板IP"));
    // ui->table_list->setHorizontalHeaderItem(2, new QTableWidgetItem("主板名称"));
	ui->table_list->horizontalHeader()->setMaximumSectionSize(100);
	ui->table_list->horizontalHeader()->setMinimumSectionSize(20);

    ui->table_list->setStyleSheet("QTableView::item:selected{ color:rgb(27, 68, 255); background:rgb(180, 180, 180); font-weight:900; }");
    ui->table_list->horizontalHeader()->setStyleSheet("::section {background-color: #808080; color:white;}");

	ui->prog_bar->setVisible(false);
	ui->prog_bar->setTextVisible(false);
    this->ui->prog_bar->setRange(0, 100);
    this->ui->prog_bar->setValue(50);

    ui->btn_start->setText("搜索");
	// this->ui->prog_bar->setStyleSheet("color: blue; background-color: yellow");

    // connect event handler
    //connect(ui->btn_start, SIGNAL(clicked), this, SLOT(onButtonEvent()));
    connect(ui->btn_start, &QPushButton::released, this, &dlgipsearch::onButtonStart);
    connect(ui->btn_clear, &QPushButton::released, this, &dlgipsearch::OnButtonClear);
}

int dev_search_cb(const NET_DEV_SADPINFO *pSadpInfo, void *pUser)
{
    if (pSadpInfo)
    {
        const char* pIpAddr = pSadpInfo->WifiNIC.EnableWifi ? pSadpInfo->WifiNIC.NicCfg.IPAddr.IpV4 : pSadpInfo->EthCfg.EthNICs[0].IPAddr.IpV4;

        qDebug() << "find IP device in local. count" << g_dev_idx++ << "device id" << pSadpInfo->SadpID << "device IP" <<pIpAddr ;
        QString ip = QString::fromUtf8(pIpAddr);
        QString devname = QString::fromUtf8(pSadpInfo->SadpID);
        // g_dlgSearch->addIpAddress(ip, devname);
        QMetaObject::invokeMethod(g_dlgSearch,
                                  "onIPRetreived",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, ip),
                                  Q_ARG(QString, devname));

    }
    else
    {
        qDebug() <<"search dev end. total found dev" << g_dev_idx;
    }
    return 0;
}

void    dlgipsearch:: addIpAddress(QString ip, QString devname){
    qDebug() << "Add device to device list";
    QMetaObject::invokeMethod(this,
                              "onIPRetreived",
                              Qt::QueuedConnection,
                              Q_ARG(QString, ip),
                              Q_ARG(QString, devname));
}

void    dlgipsearch::startLocalIpSearch(){
    GSF_DevSearchVersion version;
    GSF_DevSearchParam param;
    ZeroMemory(&version, sizeof(version));
    ZeroMemory(&param, sizeof(param));

    GSF_DEV_SEARCH_VERSION(&version);

    qDebug() << "Start IP device search in local";

    g_dev_idx = 0;

    //GSF_SEARCH_DEV_SET_LOG_CFG(SEARCHDEV_LOG_LEVEL_ERR, search_dev_log_cb);
    param.Callback = dev_search_cb;
    GSF_DEV_SEARCH_START(&param);
}

void    dlgipsearch::stopLocalIpsearch(){
    GSF_DEV_SEARCH_STOP();
}

void dlgipsearch::onButtonStart(){


    if(m_devType == e_dev_jetson || m_devType == e_dev_c308x){
        if(_bStarted){ // stop ip search thread
            if(_srch != nullptr){
                _srch->stopSearch();
             }

        }
        else{// start ip search thread



            // QThread* thread = new QThread;
            if(_srch != nullptr){
                delete _srch;
                _srch = nullptr;
            }
            QThread * thread = new QThread();
            _srch = new ipsearch(51088);
            _srch ->moveToThread(thread);
            connect(thread, SIGNAL(started()), _srch , SLOT(run()));
            connect(this, SIGNAL(stopSearch()), _srch, SLOT(stopSearch()));
            connect(_srch , SIGNAL(finished()), thread, SLOT(quit()));
            connect(_srch , &ipsearch::finished, [=](){
                _srch ->deleteLater();
                _srch = nullptr;
                _bStarted = false;
                ui->btn_start->setText("开始");
            });
            connect(_srch , SIGNAL(foundDeviceIP(QString, QString)), this, SLOT(onIPRetreived(QString, QString)));
            thread->start();
            _bStarted = true;
            ui->btn_start->setText("停止");
        }
    }
    else if(m_devType == e_dev_rv1126 || m_devType == e_dev_hisi3519){
        if(_bStarted){ // stop ip search thread
			return; 
        }
        else{// start ip search thread
            ui->btn_start->setText("搜索中");
            // start search for hi3519
            startLocalIpSearch();

            // start search thread for rv1126
            QThread * thread = new QThread();
            //_srch = new ipsearch(51088);
            _srch_rv = new device_serach_worker(_searchTime);
            _srch_rv ->moveToThread(thread);
            connect(thread, SIGNAL(started()), _srch_rv , SLOT(run()));
            connect(this, SIGNAL(stopSearch()), _srch_rv, SLOT(stopSearch()));
            connect(_srch_rv , SIGNAL(finished()), thread, SLOT(quit()));
            connect(_srch_rv , &device_serach_worker::finished, [=](){
                _srch_rv ->deleteLater();
				QMutexLocker locker(&_mutex); 
                _bStarted = false;
				locker.unlock();
                // _srch_rv = nullptr;
                stopLocalIpsearch();
            });
            connect(_srch_rv , SIGNAL(foundDeviceIP(QString, QString)), this, SLOT(onIPRetreived(QString, QString)));
            thread->start();
			startProgTimer(); 
        }
    }

}

void dlgipsearch::timerEvent(){
	bool bCompleted = false;
	QMutexLocker lock(&_mutex); 
	bCompleted = !_bStarted; 
	lock.unlock(); 
	if (bCompleted) {
		ui->btn_clear->setEnabled(true);
		ui->btn_start->setEnabled(true);
		ui->prog_bar->setVisible(false); 
        ui->btn_start->setText("搜索");
        _timer->stop();
	}
	else {
		_cntTime++;
		int percent = min((_cntTime * _interval * 100 / (float)_searchTime), 99.0f);
		ui->prog_bar->setValue(percent);
	}
}

void	dlgipsearch::startProgTimer() {
	clearDeviceList();
	_cntTime = 1;
	ui->btn_start->setEnabled(false);
	ui->btn_clear->setEnabled(false); 
	ui->prog_bar->setVisible(true);
	ui->prog_bar->setValue(0);
	_bStarted = true;
	_timer->start(); 
}

void dlgipsearch::closeEvent (QCloseEvent *event)
{
	QMutexLocker lock(&_mutex); 
	bool bcompleted = !_bStarted; 
	lock.unlock(); 
	if (!bcompleted) {
		event->ignore();
		return; 
	}
	emit(stopSearch());
    QThread::msleep(200);
 }


void dlgipsearch::OnButtonClear(){
	clearDeviceList(); 
}

void	dlgipsearch::clearDeviceList() {
	int nRow = _searchList.size();
	for (int i = 0; i < nRow; i++) {
		ui->table_list->removeRow(0);
	}
	_searchList.clear();
}

// add the item
void  dlgipsearch::onIPRetreived(QString device_ip, QString device_name){
    qDebug() << "Found new camera IP" << device_ip << ":" << device_name;
    int nIndex = _searchList.size();
    bool bFind = false;
    for (int i = 0; i < nIndex; i ++) {
        QString ip = _searchList.at(i).dev_ip;
        if(ip == device_ip)
        {
            bFind = true;
            break;
        }
    }
    if(bFind)
        return;

    QTableWidgetItem* item;
    item = new QTableWidgetItem(QString("%1").arg(nIndex + 1));
    item->setTextAlignment(Qt::AlignHCenter);
    item->setForeground(QBrush(QColor(255, 255, 255)));
    ui->table_list->setItem(nIndex, 0, item);

    item = new QTableWidgetItem(device_ip);
    item->setTextAlignment(Qt::AlignHCenter);
    item->setForeground(QBrush(QColor(255, 255, 255)));
    ui->table_list->setItem(nIndex, 1, item);

    Device device;
    device.dev_ip = device_ip;
    device.dev_name = device_name;
    _searchList.append(device);
}


