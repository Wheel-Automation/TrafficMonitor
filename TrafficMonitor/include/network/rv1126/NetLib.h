#ifndef __NET_LIB_H
#define __NET_LIB_H

#include "Windows.h"
#include "ai_def.h"


//command and handle
#define NVR_PROTOCOL_TCP 0x00
#define NVR_PROTOCOL_UDP 0x01
#define NVR_PROTOCOL_MCAST 0x02

//ACK define
#define ACK_ERROR_OTHER			0xff					//other errors
#define ACK_SUCCESS				0x00					//success
#define ACK_ERROR_NAME			0x01					//user name error
#define ACK_ERROR_PWD			0x02					//password error
#define ACK_ERROR_RIGHT			0x03					//permission error
#define ACK_ERROR_CHANNEL		0x04					//channle error
#define ACK_ERROR_PTZ_BUSY		0x05					//ptz is busy
#define ACK_SEARCH_COMPLETE		0x06					//serach completed
#define ACK_FAILED				0x07					//operation failed

#define MAX_MAC_ADDR_LEN   20
#define MAX_MAX_CRYPTO_LEN_LEN  512 //password length. 256

//message define
#define MSG_LOGIN						0xA001			//login
#define MSG_LOGOUT						0xAFFF			//logout Modify by 20210325, old:oxA00D;
#define MSG_START_PREVIEW			    0xA002	
#define MSG_STOP_PREVIEW				0xA003
#define MSG_START_JPEG					0xA004
#define MSG_STOP_JPEG					0xA005
#define MSG_STREAM_DATA					0xB001
#define MSG_CONTROL_DATA				0xA801
#define MSG_TYPE_HEART					0xA802
#define	MSG_START_PUSH_JPEG				0xA009
#define MSG_TYPE_KEY_PAD				0xA00A
#define MSG_TYPE_REQ_IFRAME				0xA00B
#define MSG_START_UPDATE_FILE			0xA00C          //update file;
#define MSG_TYPE_SET_WIFI_CFG			0xA007
#define	MSG_TYPE_GET_WIFI_CFG			0xA006
#define MSG_TYPE_UPDATE_LOCAL_INFO	    0xA00D	        //search
#define MSG_TYPE_GET_VERSION			0xA00E          //version

#define MSG_TYPE_SET_RTMP_INFO		    0xA012
#define MSG_TYPE_GET_RTMP_INFO		    0xA013
#define MSG_TYPE_GET_LIVE_STATUS	    0xA014
#define MSG_TYPE_SET_LIVE_STATUS	    0xA015
#define MSG_TYPE_GET_MVRNET_INFO		0xA016   
#define MSG_TYPE_SET_MVRNET_INFO		0xA017
#define MSG_TYPE_SET_OSDCONF_INFO       0xA018
#define MSG_TYPE_PTZ_CONTROL            0xA019        //Add by 2021-12-03, PTZ control
#define MSG_TYPE_SET_SYS_CTRL           0xA01A        //Add by 2021-21-28
#define MSG_TYPE_GET_SYS_CTRL           0xA01B
#define MSG_TYPE_SET_NETWORK_CFG        0xA01C        //Add by 2021-12-28
#define MSG_TYPE_GET_NETWORK_CFG        0xA01D
#define MSG_TYPE_SET_SYSTIME            0xA01E        //Add by 2022-06-06
#define MSG_TYPE_GET_SYSTIME            0xA01F
#define MSG_TYPE_SET_VIDEO_ROI          0xA020        //Add by 2022-07-21
#define MSG_TYPE_GET_VIDEO_ROI          0xA021
#define MSG_TYPE_SET_ISP_CONFIG         0xA022        //Add by 2022-09-28, RK set ISP parameters
#define MSG_TYPE_GET_ISP_CONFIG         0xA023
#define MSG_START_UPDATE_CFGFILE		0xA024        //Add by 2023-02-24, update the specific file


#define MSG_TYPE_ALARM					0x01

//////key pad //////////

#define KEY_TYPE_UNKNOW	0xFF
#define KEY_TYPE_FREEZE	0x01
#define KEY_TYPE_UP		0x02
#define KEY_TYPE_DOWN		0x03
#define KEY_TYPE_PWOER_DOWN	0x04
#define KEY_TYPE_DEL	    0x05
#define KEY_TYPE_FULLSCRN	0x06

//message index
#define INDEX_MSG_LOGIN					  0x0001
#define INDEX_MSG_LOGOUT				  0x0002
#define INDEX_MSG_START_PREVIEW			  0x0003
#define INDEX_MSG_STOP_PREVIEW			  0x0004
#define INDEX_MSG_START_JPEG			  0x0005
#define INDEX_MSG_STOP_JPEG				  0x0006
#define INDEX_MSG_STREAM_DATA			  0x0007
#define INDEX_MSG_CONTROL_DATA			  0x0008
#define INDEX_MSG_TYPE_HEART			  0x0009
#define INDEX_MSG_TYPE_ALARM			  0x000a
#define INDEX_MSG_TYPE_SET_WIFI_CFG		  0x000b
#define INDEX_MSG_TYPE_GET_WIFI_CFG		  0x000c
#define INDEX_MSG_TYPE_REQ_IFRAME		  0x000d
#define INDEX_MSG_TYPE_GET_VERSION		  0x000e

#define INDEX_MSG_TYPE_SET_RTMP_INFO      0x000f
#define INDEX_MSG_TYPE_GET_RTMP_INFO	  0x0010
#define INDEX_MSG_TYPE_GET_LIVE_STATUS	  0x0011
#define INDEX_MSG_TYPE_SET_LIVE_STATUS	  0x0012
#define INDEX_MSG_TYPE_GET_MVRNET_INFO	  0x0013  
#define INDEX_MSG_TYPE_SET_MVRNET_INFO	  0x0014
#define INDEX_MSG_TYPE_UPDATE_LOCAL_INFO  0x0015
#define INDEX_MSG_TYPE_SET_OSDCONF_INFO   0x0016

#define INDEX_MSG_TYPE_PTZ_CONTROL        0x0017        //Add by 2021-12-03, 
#define INDEX_MSG_TYPE_SET_SYS_CTRL       0x0018        //Add by 2021-12-28
#define INDEX_MSG_TYPE_GET_SYS_CTRL       0x0019
#define INDEX_MSG_TYPE_GET_NETWORK_CFG    0x001A        //Add by 2021-12-28;
#define INDEX_MSG_TYPE_SET_NETWORK_CFG    0x001B
#define INDEX_MSG_TYPE_GET_SYSTIME        0x001C        //Add by 2022-06-06;
#define INDEX_MSG_TYPE_SET_SYSTIME        0x001D
#define INDEX_MSG_TYPE_SET_VIDEO_ROI      0x001E        //Add by 2022-07-21;
#define INDEX_MSG_TYPE_GET_VIDEO_ROI      0x001F
#define INDEX_MSG_TYPE_SET_ISP_CONFIG     0x0020        //Add by 2022-09-28
#define INDEX_MSG_TYPE_GET_ISP_CONFIG     0x0021


///////////////// 媒体流类型////////////////////////////
#define TYPE_I_FRAME			0x01					//I frame
#define TYPE_P_FRAME			0x02					//P frame


#define TYPE_H264_DATA			0x01

#define TYPE_JPEG_DATA			0x02					// jpeg frame

#define TYPE_UPDATE_DATA		0x03					// update frame


///////////////data parket  ///////////////////////
#define DATA_UNIT_FLAG			0x00					//message body
#define DATA_START_FLAG			0x01					//header
#define DATA_STOP_FLAG			0x02					//suffix

//////////////////// resolution /////////////////////////

#define RESOLUTION_704x576		0x01
#define RESOLUTION_704x480		0x02
#define RESOLUTION_1280x720		0x03
#define RESOLUTION_800x600		0x04
#define RESOLUTION_1280x1024	0x05
#define RESOLUTION_1024x768		0x06

#define ERRCODE_NO_CHANNEL		0x0001

#define MAX_FRAME_UNIT_NUM		48						//每个帧可以分成最多48个单元
#define MAX_FRAME_UNIT_LEN		16*1024					//每个单元最多8k，每个帧最大长度为360k
#define MAX_FRAME_POOL_NUM		30						//最多缓存50帧的数据 8fps 4 sec data
#define MAX_JPEG_POOL_NUM		10						//最多缓存10个图片循环
#define MAX_UNIT_NUM			500						//最多有150个公共缓冲池供使用
#define FRAME_INDEX_IMAGE		0						//则里面专门存储图片，高优先级发送
#define FRAME_INDEX_PREVIEW		1						//这里面专门存储实时数据流，低优先级发送

#define MAX_CHANNEL_CONNECT		4						//每次登录最多有4个媒体通道可以用， 省去通道管理
#define MAX_CLIENT_CONNECT		4						//最多可以允许4个用户登录

#define MAX_USER_LEN	        32                      //Login user max length;
#define MAX_PASSWD_LEN	        32                      //Login pwd max length;
#define RTMP_URL_LEN	        128                     //RTMP URL length;
#define RTMP_STREAMNAME_LEN	    128                     //RTMP Stream Name length;
#define MAX_OSD_CONTEXT_LEN     256                     //OSD string Max length;
#define MAX_IPADDR_LEN		    20

#define SYS_CTRL_TYPE_FLIP      0x01                    //Add by 2021-12-28
#define SYS_CTRL_TYPE_WSN       0x02
#define SYS_CTRL_TYPE_IRCUT     0x03
#define SYS_CTRL_TYPE_WLED      0x04
#define SYS_CTRL_TYPE_AUTH      0x05
#define SYS_CTRL_TYPE_MAC       0x06
#define SYS_CTRL_TYPE_INFO      0x07
#define MAX_COMM_DATA_LEN       512

//Add by 2022-10-21, 增加新协议;
//新类型：RockChip, 从0x54532000开始; 2022-10-24;
//1126和1109相同，显示为1126;
#define DEV_TYPE_RK1126_IPC       0x54532000
#define DEV_TYPE_RK1126_DUALIPC   0x54532100
#define DEV_TYPE_RK1126_AIBOX     0x54532200
#define DEV_TYPE_RK1126_NVR       0x54532300
#define DEV_TYPE_RK1126_DVR       0x54532400

//3568,3566显示为356X;
#define DEV_TYPE_RK356X_IPC       0x54533000
#define DEV_TYPE_RK356X_AIBOX     0x54533100
#define DEV_TYPE_RK356X_NVR       0x54533200
#define DEV_TYPE_RK356X_DVR       0x54533300

//1106和1103相同，显示为1106;
#define DEV_TYPE_RK1106_IPC       0x54534000
#define DEV_TYPE_RK1106_DUALIPC   0x54534100

typedef enum __COMM_SENSOR_TYPE
{
	IMX327 = 0, //200W
	IMX335 = 1, //500W
	IMX347 = 2, //400W
	IMX415 = 3, //800W
	SC530AI = 4, //500W
	SC230AI = 5, //200W
} COMM_SENSOR_TYPE;
#define DEFAULT_ADCHIP_TYPE   IMX327
#define DEFAULT_SUB_TYPE      0 //reserve
#define DEFAULT_DEV_TYPE      DEV_TYPE_RK1126_IPC

#define COMM_ADDRSIZE            20
#define MAX_COMM_SERIAL_NO_LEN   40
#define MAX_COMM_SN_LEN          128
#define COMM_MAX_CHANNEL_NUM     4
#define COMM_VERSION_LEN         24
//End by 2022-10-21


struct __stream_ext_t__
{
	unsigned char data_flag; 							//当为视频数据时，指示数据的1开始，2结束，0中间
	unsigned char data_type;							//数据类型， 2:jpeg，1:h264 ?
	//unsigned short channel; 							//发送媒体流时，指示通道号, Modify by 2022-04-21;
	unsigned char channel; 							    //发送媒体流时，指示通道号
	unsigned char stream_index;                         //0:Main Stream; 1:sub stream;
};
typedef struct __stream_ext_t__  stream_ext_t;

struct __ctrl_ext_t__
{
	unsigned char total_packet;
	unsigned char curr_packet;
	unsigned short msg_type;							//子类型
};
typedef struct __ctrl_ext_t__ ctrl_ext_t;

struct _msg_head_t_
{
	unsigned char headFlag; 							//协议标示头，1字节，固定 0xff
	unsigned char version; 								//版本号，当前为 1
	unsigned short ack;									//控制命令时，回应消息
	unsigned int sessionId; 							//sessionid，登陆设备后，设备返回唯一的sessionid
	unsigned int seqnum; 								//seq号，启动后从0开始加，到达最大后从0开始,同一命令的seq不变，两边各自使用各自的seq
	unsigned short msgId; 								//消息码
	unsigned short result;								//消息结果
	union
	{
		stream_ext_t stream_ext;
		ctrl_ext_t ctrl_ext;
	}ext;
	unsigned int data_len;								//消息数据长度，不包含消息头的
	int key_type;
	unsigned int reserve;
};
typedef struct _msg_head_t_ msg_head_t;

struct __net_msg_t
{
	msg_head_t	msg_head;		//消息头
	char	msg_data[1024];		//消息数据
};

typedef struct __net_msg_t net_msg_t;
struct __net_frame_head_t
{
	unsigned int frame_size;
	unsigned int frame_no;								//jpeg frame_no is zero
	unsigned char video_resolution;
	unsigned char frame_type;							// I,P
	unsigned char frame_rate;  
	unsigned char video_standard;         
	unsigned int hi_pts;								//usec
	unsigned int lo_pts;								//usec
};
typedef struct __net_frame_head_t net_frame_head_t;

struct __jpeg_head_t__
{
	int len;											//文件大小
	char name[128];										//文件名称
};
typedef struct __jpeg_head_t__ jpeg_head_t;

struct __wifi_cfg_t__
{
	char nType;		// 0: ap   1: sta
	char nChannel;	// 作为ap判断是 2g :[1,6,11],[2,7,12],[3,8,13],[4,9,14]互不干扰 还是 5g：36 38 40 ...64, 100 102 ... 144 , 149 151... 165
	char res[2];	// 保留用
	char ssid[64];
	char passwd[64];
	char ipaddr[24];
	char netmask[24];
	char gw[24];
};
typedef struct __wifi_cfg_t__ wifi_cfg_t;

struct __version_info_t__
{
	char version[64];
};
typedef struct __version_info_t__  version_info_t;

struct __login_info_t_
{
	char szUserID[MAX_USER_LEN];
	char szPassword[MAX_PASSWD_LEN];
};
typedef struct __login_info_t_ login_info_t;

struct __rtmp_info_t_
{   
	int  enable;
	char szUrl[RTMP_URL_LEN];
	char szStreamName[RTMP_STREAMNAME_LEN];
};
typedef struct __rtmp_info_t_ rtmp_info_t;

struct __mvr_net_info_t_
{
	char szUrl[RTMP_URL_LEN];
	char szName[MAX_USER_LEN];
	char szPassword[MAX_PASSWD_LEN];
	int  nPort;
};
typedef struct __mvr_net_info_t_ mvr_net_info_t;

/*struct __local_info_t__
{
	char local_ip[20];      //连接的本机ip地址,网络字节序
	int  local_port;        //连接的本机端口
	char local_serial[64];  //本机的序列号+设备名称:mac +名称
	//char live_status;	 	//Live broardcast work status;修改为sn_status;
	char sn_status;         //是否已写号;  
	char wifi_flag;         //网络使用有线还是无线;0:wireless; 1:wire(无线);
	char stream_cnt;        //MVR NET 观看人数;
	char res;               //Reserve;
};*/

//Modify by 2022-10-21;
struct __local_info_t__
{
	char local_ip[COMM_ADDRSIZE];             //连接的本机IP地址,网络字节序
	int  local_port;                          //连接的本机端口
	char mac_addr[COMM_ADDRSIZE];             //MAC地址; 最大长度20;
	char serial_info[40];                     //序列号;
	int  device_type;                         //从0x54532000开始，参看设备类型具体说明;
	char auth_flag;                           //是否鉴权;0:未鉴权,1:已经鉴权;
	char led_type;                            //暂时未使用;
	char cam_count;                           //camera个数;
	char res;                                 //保留;
};
typedef struct __local_info_t__ local_info_t;

struct __live_info_t_
{
	char status;
	char wifi_flag;         //网络使用有线还是无线;0:wireless; 1:wire(无线);
	char stream_cnt;        //MVR NET 观看人数;
	char res;
};
typedef struct __live_info_t_ live_info_t;

//////// osd  //////
struct __mvr_osd_info_t_
{
	int iPointX;
	int iPointY;
	int iContextLen;
	char szContext[MAX_OSD_CONTEXT_LEN];  //UTF-8编码格式;
};
typedef struct __mvr_osd_info_t_  mvr_osd_info_t;

struct __mvr_osd_attr_t_
{
	int iFontColor;  //32位RGB值,Bit:[31-24]位为Alpha值，无效分量；后续[23-0]依次为RGB分量，每个8位;
	int iFontSize;   
	int iBgColor;    //32位RGB值,同上，目前无效;
	int iTrans;      //透明度，目前无效;
};
typedef __mvr_osd_attr_t_  mvr_osd_attr_t;

struct __osd_config_t_
{
	int iIndex;             //第几个index;
	int iOsdEnable;         //0:Disabled; 1:Enable;
	int iEnableTime;        //是否使能时间戳;0:Disabled; 1:Enable;
	mvr_osd_attr_t stOsdAttr;
	mvr_osd_info_t stOsdInfo;
};
typedef __osd_config_t_ osd_config_t;

/*typedef enum
{
	PTZ_CMD_STOP = 0,
	PTZ_CMD_MENU_UP,
	PTZ_CMD_MENU_DOWN,
	PTZ_CMD_MENU_LEFT,
	PTZ_CMD_MENU_RIGHT,
	PTZ_CMD_MENU_ENTER,	//5
	PTZ_CMD_MENU_IRIS_PLUS,
	PTZ_CMD_MENU_IRIS_MINUS,
	PTZ_CMD_MENU_FOCUS_PLUS, // +
	PTZ_CMD_MENU_FOCUS_MINUS,// -
	PTZ_CMD_MENU_ZOOM_PLUS,//  10 +
	PTZ_CMD_MENU_ZOOM_MINUS,// -
	PTZ_CMD_PRESET_SET,
	PTZ_CMD_PRESET_CALL,
	PTZ_CMD_PRESET_DEL,
	PTZ_CMD_NULL
} PTZ_CMD_E;*/

typedef enum
{
	PTZ_CMD_UPSTART = 0x01,
	PTZ_CMD_UPSTOP,
	PTZ_CMD_DOWNSTART,
	PTZ_CMD_DOWNSTOP,
	PTZ_CMD_LEFTSTART,        //5;    
	PTZ_CMD_LEFTSTOP,
	PTZ_CMD_RIGHTSTART,
	PTZ_CMD_RIGHTSTOP,
	PTZ_CMD_LEFTUPSTART,
	PTZ_CMD_LEFTUPSTOP,      //10;  
	PTZ_CMD_RIGHTUPSTART,
	PTZ_CMD_RIGHTUPSTOP,
	PTZ_CMD_LEFTDOWNSTART,
	PTZ_CMD_LEFTDOWNSTOP,
	PTZ_CMD_RIGHTDOWNSTART,  //0x0f(15);
	PTZ_CMD_RITHTDOWNSTOP,   //0x10;  

	PTZ_CMD_PREVPOINTSET,
	PTZ_CMD_PREVPOINTCALL,
	PTZ_CMD_PREVPOINTDEL,
	PTZ_CMD_ZOOMADDSTART,    //20;   
	PTZ_CMD_ZOOMADDSTOP,
	PTZ_CMD_ZOOMSUBSTART,
	PTZ_CMD_ZOOMSUBSTOP,
	PTZ_CMD_FOCUSADDSTART,
	PTZ_CMD_FOCUSADDSTOP,    //0x19(25);    
	PTZ_CMD_FOCUSSUBSTART,
	PTZ_CMD_FOCUSSUBSTOP,
	PTZ_CMD_IRISADDSTART,
	PTZ_CMD_IRISADDSTOP,
	PTZ_CMD_IRISSUBSTART,    //0x1e(30); 

	PTZ_CMD_IRISSUBSTOP,
} PTZ_CMD_E;

typedef struct __PtzConf_t
{
	int chann;
	int cmd;
	int param1; //预置位时表示Pos:0-254,其他表示为Speed:1-100;
	int param2;
	int speed;  //保留,全部用param1;
	int reserve[4];
} PtzConf_t;

typedef struct __sys_ctrl_t_
{
	char ctrl_type; // 0x01: flip  0x02  write sn: 0x03: color->black   0x04 : white led ctrl
	char ctrl_val; //
	char res[3];
	int  data_len;
	char data[MAX_COMM_DATA_LEN];
} sys_ctrl_t;

/*typedef struct __sys_ctrl_result_t_
{
	char flip_flag;
	char sn_flag;
	char ir_flag;
	char white_led_flag;
	char xx_flag;  //板子是否授权
	char res[3];
} sys_ctrl_result_t;*/

typedef struct __net_sys_ctrl_flip_t
{
	int flip;
} net_sys_ctrl_flip_t;

typedef struct __net_sys_ctrl_wsn_t
{
	char sn[MAX_COMM_SN_LEN];
} net_sys_ctrl_wsn_t;

typedef struct __net_sys_ctrl_ircut_t
{
	int reverse; // a--->b or b--->a
} net_sys_ctrl_ircut_t;

typedef struct __net_sys_ctrl_led_t
{
	int led_type; // a--->b or b--->a
} net_sys_ctrl_led_t;

typedef struct __net_sys_ctrl_auth_t
{
	int auth; //
} net_sys_ctrl_auth_t;

typedef struct __net_sys_ctrl_mac_t
{
	char device_serial_no[32]; //serial
	char mac_addr[COMM_ADDRSIZE];//20
} net_sys_ctrl_mac_t;

typedef struct __net_sys_ctrl_info_t
{
	unsigned int dev_type;    /* 设备类型:后两位为通道个数,IPC,NVR,DVR,AIBOX,DUALIPC*/
	unsigned int sub_type;    /* 设备子类型 用于配置工具配置,球机，枪机，半球 */

	unsigned char   channel_num;  /*产品的总通道数, max:4*/
	unsigned char   prober_num;   /*报警输入探头数*/
	unsigned char   output_num;   /*输出总数*/
	unsigned char   serial_num;   /*串口总数*/

	unsigned char   hd_num;           /* hard disk number, max:4 */
	unsigned char   ptz_serial_no;    /* 云台控制使用的固定串口号 */
	unsigned char   wifi_enable;
	unsigned char   enable_4G;
	unsigned char   ad_type[4];    /* imx347,imx415,imx327 ...... */
	unsigned int    resolv_capable_m[COMM_MAX_CHANNEL_NUM];
	/* 设备各通道能力级 主码流
	*  bit0:QCIF bit1:CIF bit2:HD1 bit3:D1 bit4:720p
	*  bit5:QVGA bit6:VGA bit7:HI_SXGA bit8:XGA bit9:1080i*/
	unsigned int    resolv_capable_s[COMM_MAX_CHANNEL_NUM];

	unsigned char   sys_analogNum;  //可以接入的模拟通道数量, max:4;
	unsigned char   sys_digitNum;   //可以接入的数字通道数量，用于解码器和nvr, max:4;
	unsigned char   unused0[2];

	unsigned short  maxWidth;  //解码最大宽
	unsigned short  maxHeigh;  //解码最大高

	char  device_serial_no[32];   /* 产品序列号 */
	char  product_type[COMM_VERSION_LEN]; /* 产品型号 */
	char  mac_addr[COMM_ADDRSIZE];//20
	char  leave_factory_date[COMM_VERSION_LEN]; /* 出厂日期,年月日 */
} net_sys_ctrl_info_t;

typedef struct __NetworkConf_t_
{
	int iDhcpFlag;
	int iWifiFlag;
	char szIpAddr[MAX_IPADDR_LEN];
	char szNetMask[MAX_IPADDR_LEN];
	char szGateWay[MAX_IPADDR_LEN];
	char szDns1[MAX_IPADDR_LEN];
	char szDns2[MAX_IPADDR_LEN];
} NetworkConf_t;

typedef struct _net_time_t_
{
	int year;
	int mon;
	int day;
	int hour;
	int minute;
	int sec;
	int timezone;    //    +/- 24*60*60 second
} net_time_t;

typedef struct __net_time_seg_t
{
	unsigned char start_hour;
	unsigned char start_minute;
	unsigned char start_second;
	unsigned char unused;
} net_time_seg_t;

//Add by 2022-09-28, 增加RK设置ISP;
typedef struct _net_isp_exposure_t
{
	int iExpMode; //auto   manual   spec
	int iExpvalue;
	int iExpMaxVal;
	int iExpMinVal;
	int iGainMode;
	int iGainValue;
	///////////////////
	int iAeSpeed;
	int iAeSetPoint; //day target ae
	int iAeSetDayPoint; //night target ae
	int iAeSetPointEn; //auto target ae
} net_isp_exposure_t;

typedef struct _net_isp_dayNight_t
{
	int iMode; //auto  day night spec
	int iSens; //0....7
	net_time_seg_t start;
	net_time_seg_t end;
	int iFilterTime; //3...10
	int iLedMode;    //red, white/red    white
	int iLedLightValue; //0...10
} net_isp_dayNight_t;

typedef struct __net_isp_blc_t
{
	int iHdrMode; // normal   hdr2
	int iHdrLevel; //0,1,2,3
	int iBlcMode; // close,open
	int iBlcValue; // 0-100
	int iHlcMode; // close open
	int iHlcLevel; // 0-100
	int iHlcEncValue; //0-100
} net_isp_blc_t;

/*typedef struct __net_isp_wb_t
{
	int iWbMode; //auto,manul
	int iRGainValue; //0-100
	int iGGainValue; //0-100
	int iBGainValue; //0-100
} net_isp_wb_t;*/

typedef struct __net_isp_wbgain_t
{
	int iRGainValue;   //0 -100
	int iGRGainValue;  //0-100
	int iGBGainValue;  //0-100
	int iBGainValue;   //0-100
} net_isp_wbgain_t;

typedef struct __net_isp_wbscene_t
{
	int iSeneMode;
	//RK_AIQ_WBCT_INCANDESCENT = 0, 
	//RK_AIQ_WBCT_FLUORESCENT, 
	//RK_AIQ_WBCT_WARM_FLUORESCENT,
	//RK_AIQ_WBCT_DAYLIGHT,
	//RK_AIQ_WBCT_CLOUDY_DAYLIGHT,
	//RK_AIQ_WBCT_TWILIGHT,
	//RK_AIQ_WBCT_SHADE
} net_isp_wbscene_t;

typedef struct __net_isp_wbcct_t
{
	int cct;   //相关色温
	int ccri;  //相关显色指数;
} net_isp_wbcct_t;

typedef struct __net_isp_wb_t
{
	int iWbMode;       //auto,manual-wbgain,manual-wbscene,manual-wbcct
	net_isp_wbgain_t wbgain;
	net_isp_wbscene_t wbscene;
	net_isp_wbcct_t wbcct;
} net_isp_wb_t;

typedef struct __net_isp_nr_t
{
	int iNrMode;        //0 default,1 timeNR 2 spaceNR, 3 time and space
	int iTimeFilter;    //0-100
	int iSpaceFilter;   //0-100
} net_isp_nr_t;

typedef struct __net_isp_forg_t
{
	int iForgMode; //close open auto
	int iForgLevel;
} net_isp_forg_t;

typedef struct __net_isp_abc_t
{
	int iAbcMode;
	int iAbcLevel;
} net_isp_abc_t;  //Aberration correction

typedef struct __net_isp_mirror_t
{
	int iMirrorMode;
} net_isp_mirror_t;

#define ISP_TYPE_EXPOSURE_CONFIG    0x01
#define ISP_TYPE_DAYNIGHT_CONFIG    0x02
#define ISP_TYPE_BLC_CONFIG         0x03
#define ISP_TYPE_WB_CONFIG          0x04
#define ISP_TYPE_NR_CONFIG          0x05
#define ISP_TYPE_FORG_CONFIG        0x06
#define ISP_TYPE_ABC_CONFIG         0x07
#define ISP_TYPE_MIRROR_CONFIG      0x08
typedef struct __net_isp_config_t
{
	int chann_no;
	int isp_type;   //参数子类型,对应相应的参数结构体;
	union 
	{
		net_isp_exposure_t exposure;
		net_isp_dayNight_t dayNight;
		net_isp_blc_t blc;
		net_isp_wb_t wb;
		net_isp_nr_t nr;
		net_isp_forg_t forg;
		net_isp_abc_t abc;
		net_isp_mirror_t mirror;
	} isp_config;
} net_isp_config_t;
//End by 2022-09-28;

typedef int (CALLBACK *VIDEODATACALLBACK) (char* pBuf, long lSize, DWORD dwUserData);
typedef int (CALLBACK  *JPEGDATACALLBACK) (char* pBuf, long lSize, DWORD dwUserData);
typedef int (CALLBACK *KEYPADCALLBACK)(int KeyType, DWORD dwUserData);
#endif
