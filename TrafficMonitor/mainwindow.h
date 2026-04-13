#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QPoint>
#include <QMenu>
#include <QAction>
#include <QMutex>
#include <QMenuBar>
#include <QTimer>
#include <QElapsedTimer>
#include <QSystemTrayIcon>
#include <QReadWriteLock>
#include <QMap>
#include <QTableWidgetItem>
#include <QWaitCondition>
#include <QMutex>
#include <control_panel.h>
#include "opencv2/opencv.hpp"
#include "utils.h"
#include "cameraview.h"
#include "camViewerWorker.h"
#include "dlg_login.h"
#include "dlg_camera_setting.h"
//#include "serverthread.h"
#include "longconnectserver.h"
#include "modifyip.h"
#include "state_panel.h"
#include "dlgparamsetting.h"
#include "devicelogwnd.h"
#include "setting/settingmanager.h"
#include "setting/logwriterthread.h"
#include "com/proxythread.h"
#include "com/com.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

private:
    void    InitUI();
    void    CreateMenubar();
    void    CreateAction();
    void    CreateUISignalConnection();
    void    InitListView();
    void    CreateControPanel();
    void    create_camera_viewer();
    void    initAPPState();
    void    startCamThread();
    void    stopCamera();

    void    startConnectThread(int request_type = e_get_param);
    void    startServerThread();
    void    closeServerThread();
    void    free_memory();
    void    draw_setting(QImage& img, int w, int h);
    void    closeEvent (QCloseEvent *event) override;
    void    enableDisplay(bool benable);
    void    startAssetTimer();
    bool    check_stability(bool bCheckReamin = false);
    bool    loadSetting();
    bool    SaveSetting();
    bool    EnableMonitor(bool bJMG);
    void    UpdateStatePanel();
    void    onExitProcess();
    void    startProxyThread();

signals:
    //void    stopCamera();
    void    changeURL(QString param);
    void    emptysignal();
    void    changePlayState(int state);
    void    changeConnectState(int state);

public slots:
    void    slot_socket_report(int, int, PANEL_PARM, SETTING);
    void    slot_jetson_connect(PANEL_PARM); // connect request from control panel
    void    slot_camera_connected(bool bconnected);
    void    slot_panel_ok(PANEL_PARM);
    void    slot_panel_cancel();
    void    onShowStream();
    void    onHideStream();

    void    display_one_frame(uchar* buffer, int w, int h, int c, bool draw_setting = true);
    void    slot_mouse_move(QMouseEvent*);
    void    slot_mouse_press(QMouseEvent*);
    void    slot_mouse_release();
    void    slot_receive_result(QString cam_ip, QString cam_index, int nCrossType, int nWheels, QString strClss, QString szBuffer);
    void    slot_receive_status(int cam_index, QString szBuffer);

    void    slot_check_jmg_connect();
    void    slot_check_status_connect();
    void    action_login_device();
    void    action_camera_manage();
    void    action_reset_ip();
    void    action_restart_device();
    void    action_parameter_setting();
    void    action_ip_search();
    void    action_proxy_setting(); // set the proxy ip address and port;

    void    action_log_setting();
    void    action_log_view();
    void    onComportClosed();
    void    onComportDataTransfered(QString camIP, QString portName, int nW, int nType, QString tranferedBuffer);
    void    onReceivedTcpProxyResult(QString szBuffer, QString szIP, int nPort, bool bSuccess);
private :
    void    add_device_log(QString szLog, bool bStatusLog = true);
    void    startDetectionResultLog();
    bool    eventFilter(QObject *obj, QEvent *event) override;
    void    connectAutoHideConnections();
    void    sendToTrayIcon();
private:

    bool    _bMouseMoved;
    bool    _bEnabledisplay;
    QMenu * menuMain;
    QAction * actLoginDevice; // login device
    QAction* actManangeCamera;  // modify roi parameter
    QAction* actResetIP;        // reset the jetson IP

public:
    PANEL_PARM  get_panel_param();

private:
    Ui::MainWindow *ui;
    SETTING         _roi;
    PANEL_PARM      m_param;
    bool                            m_bEnableProxy;
    QMap<QString, PROXY_SETTING>    m_proxyTable;
    QThreadPool                     m_proxyTaskPool;

    CameraView*      m_viewer;
    camViewerWorker* m_camworker;
    Server**         m_socketServer;
    int              m_nSocketServer;
    QStringList      m_ipAddresses;

    dlg_login*      dlgLogin;
    dlg_camera_setting*  dlg_cam_set;

    DlgParamSetting*        dlgParamSetting;
    ModifyIP*        dlg_reset_ip;

    int              _pick_index;
    bool             _camera_closed;
    bool            _bLogin;
    bool            _bFirstLogin;
    int             _nRecords;
    int             m_nRunHours; // running hours
    bool            m_bForceStop;
    bool            m_bStartRtstReqeust;
    bool            m_isJMG;
    QMutex          m_mutexCamera;
    QElapsedTimer           m_cam1_status_timer; // last timer of cam 1
    QElapsedTimer           m_cam2_status_timer; // last timer of cam 2
    QElapsedTimer           m_network_timer;
    QElapsedTimer           m_port_timer;
    bool                    m_bFirstReceived[2]; // received at least one status report
    QTimer                  m_check_status_timer;// timer for check status
    QTimer                  m_autoHideTimer;
    QElapsedTimer           m_calib_timer;

    /////////////////// trayicon related    //////////////////
public:
    void setVisible(bool visible) override;

private slots:
    void setIcon(int index);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();
    void onUserActionTriggered(QString actionType);
    void on_tableWidget_cellClicked(int row, int column);

private:
    void    initializeTrayLogic();
    void    createTrayIcon();
    void    createActions();
    void    showTrayBallonMessage(QString message, MSGTYPE messageType, int duration = 10);
    void    enableProxy(bool bEnable);
    void    sendProxyData(QString camIP, int nWheel, QString nWheelType, QString szBuffer);

private:
    QAction *restoreAction;
    QAction *quitAction;
    LET_STATE       m_lastState;
    QIcon           m_iconNormal, m_iconDisconneced;
    QSystemTrayIcon *trayIcon;
    QMenu           *trayIconMenu;
    QSize           m_windowSize;  // size of main window
    QSize           m_frameSize;    // size of frame region
   //////////////////// setting related //////////////////////

private:
    SettingManager      m_settingManager;
    QReadWriteLock      m_proxyLock;
    int                 m_cntLogLine;
    bool                m_bShowLog;
    logWriterThread*    m_logthread;
    bool                m_bOpendLogWindow;
    bool                m_bExiting;

    proxyThread*        m_proxyThread;
};

#endif // MAINWINDOW_H

// qtryicon: https://doc.qt.io/qt-6/qtwidgets-desktop-systray-example.html
