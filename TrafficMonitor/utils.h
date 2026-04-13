#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QMetaType>
#include <QPointF>
#include <QPoint>
#include <QList>


#define report_type         "report_type"
#define camera_index        "camera"
#define camera_ip_key           "camera_ip"
#define cross_type_key      "cross_type"
#define wheels_key          "wheels"
#define wheels_kinds        "kinds"


enum DIRECTION{
    DIRECTION_RL = 0,
    DIRECTION_LR = 1
};

enum SUPT_DEV_TYPE{ // device type supported
    e_dev_jetson = 0,
    e_dev_c308x = 1,
    e_dev_rv1126 = 2,
    e_dev_hisi3519 = 3,
    e_dev_unknow = 4,       // unknown device type
};


enum HI3519_CGI_REQ{ // device type supported
    e_cgi_invalid= 0,
    e_cgi_login,
    e_cgi_get_eth,
    e_cgi_set_eth,


};



enum UPDATE_SETTING_RESULT
{
    SETTUP_FAILED = 0,
    SETTUP_ROI_SUCCESS = 1,
    CAMERA_RUNNING = 2,
    CAMERA_NOT_RUNNING = 3,
    SETUP_CAMERA_IP_REPEATED = 4,
    PWD_INCORRECT = 5,
    ONE_DIRECT_BOARD = 6,
    DBL_DIRECT_BOARD = 7
};

enum REPORT_TYPE{
    report_det_result = 0,
    report_cur_state = 1
};

enum DETECT_MODE{ // detection kind(one direction or two direction)
    DETECT_ONE_DIRECT = 0,
    DETECT_TWO_DIRECT = 1,
};

enum LET_STATE{
    e_turnoff = 0,
    e_turnon = 1,
    e_disable = 2,
    e_state_cnt = 3
};

enum DETECT_STATE{
    e_run_1st_cam = 1, // run 1st camera
    e_run_2nd_cam = 2,  // run 2nd camera
    e_run_dbl_cam = 3   // run 3rt camera
};

enum PROXY_MODE{
    e_proxy_disable = 0,    // disable proxy
    e_proxy_tcp = 1,        // tcp proxy
    e_proxy_rs = 2,          // rs-232 proxy
    e_proxy_count
};

struct PROXY_SETTING{
    QString     password;      // password of this device
    PROXY_MODE proxyMode;      // 0: disable, 1: tcp, 2: rs-232 proxy
    QString proxyIP;    // server ip. used at tcp proxy mode
    int proxyPort;      // server port. used at tcp proxy mode
    QString comPort;        // com port, used at rs-232 proxy mode
    PROXY_SETTING(){
        password = "123456";
        proxyMode = e_proxy_tcp;
        proxyIP = "127.0.0.1";
        proxyPort = 51087;
        comPort = "COM3";
    }
};

typedef struct _PANEL_PARM{
    PROXY_SETTING  proxy;

    DIRECTION e_direct; // detect direction
    QString   jtip;     // jetson ip
    QString    jtpwd;   //jetson password
    QString     dev_name;   // jetson device name
    int         dev_type;   // device type.
    int         cam_index;
    QString     camip;  //cam ip
    QString     serverip;
    int         nLogSaveDay;
    int         heartRate;

    // reset ip param
    QString jt_new_ip;
    QString jt_new_mask;
    QString jt_new_gateway;
    QString jt_new_dns;
    DETECT_MODE _det_mode;
    int     logSaveDay;
    int     serverPort;         // server port of windows
    int     clientPort;           // port of send detection data
    bool    enable_led;
    bool    enable5Wheel;
    bool    enableTimeCalib;
    int     ledOnTime;
    int     ledOffTime;
    int     timeOut;
}PANEL_PARM;

typedef struct _KEY{
    unsigned short key1;
    unsigned short key2;
    unsigned short key3;
    unsigned short key4;
}KEY;

typedef struct _ROI{
    QPointF  ptRegion[6];
    float    ptRef;
}SETTING;

typedef struct _IP_SET{
    QString     ip;
    QString     ip_mask;
    QString     ip_gateway;
    QString     ip_dns;
    QString     ip_dns2;
    _IP_SET(){
        ip = "0.0.0.0";
        ip_mask = "0.0.0.0";
        ip_gateway = "0.0.0.0";
        ip_dns = "0.0.0.0";
        ip_dns2 = "0.0.0.0";
    }
    _IP_SET(QString _ip, QString _ip_mask, QString _ip_gateway, QString _ip_dns, QString _ip_dns2 = ""){
        ip = _ip; ip_mask = _ip_mask; ip_gateway = _ip_gateway; ip_dns = _ip_dns; ip_dns2 = _ip_dns2;
    }
    void clear_sub_info(){
        ip = "";  ip_mask = "", ip_gateway = "", ip_dns = ""; ip_dns2 = "";
    }

}IP_SET;

// connection type with jetson
typedef enum _cmd_type{
    e_get_param = 1,
    e_set_param = 2,
    e_reset_dev_pwd = 3,
    e_reset_ip = 4,
    e_restart_device = 5,
    e_calib_time = 6
}CMD_TYPE;

// dlg type
typedef enum _PROMPT_TYPE{
    e_prompt_warning = 0,
    e_prompt_error = 1,
    e_prompt_note = 2
}PROMPT_TYPE;

// message type to show trayicon message
typedef enum _MSGTYPE{
    MSG_INFO = 0,
    MSG_WARN,
    MSG_ERR,
    MSGTYPE_MAX
}MSGTYPE;


Q_DECLARE_METATYPE(PANEL_PARM);
Q_DECLARE_METATYPE(SETTING)
typedef struct _APP_STATE{
    bool    bCameraRun;
}APP_STATE;



void            register_datatype();
int             distance(QPoint pt1, QPoint pt2);
QPointF*        QJsontoQPointArray(QJsonArray jsonArray);
void            promp_error_message(QWidget* parent, QString errormsg);
QString         GetSelfAddress();
QStringList     GetSelfMultiAddress();

// convert ipaddress into gateway
bool            GetGateWay(QString ip, QString & ipGateWay);
// log file
void            log_file(QString message);


class Utils{
public:

    static QString              getLogHomeDir();
    static QStringList          findAllLogFiles(QString dirPath);
    static QStringList          findAllSubDirectories(QString dirPath);
    static QString              FilePathToName(QString fullPath);

};

#endif // UTILS_H


