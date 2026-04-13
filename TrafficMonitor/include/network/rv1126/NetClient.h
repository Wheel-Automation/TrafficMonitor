#ifndef __NETCLIENT_H_
#define __NETCLIENT_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NETCLIENT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NETCLIENT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef __cplusplus
extern "C" {
#endif


#ifdef NETCLIENT_EXPORTS
#define NETCLIENT_API __declspec(dllexport)
#else
#define NETCLIENT_API __declspec(dllimport)
#endif

#include "NetLib.h"

#define NETCLIENT_CALL __stdcall


//initialize client library
/**************************************************************************
* Function Name  : InitClientLib
* Description    : Init Network library
* Parameters     : NULL
* Return value   : success  : >0, handle of Network library, use for follow-up interface ;
*                  failure  : 0;
***************************************************************************/
NETCLIENT_API long NETCLIENT_CALL InitClientLib();

//release resources
/**************************************************************************
* Function Name  : DeInitClientLib
* Description    : Unload Network library
* Parameters     : lHandle, [in] handle of Network library.
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL DeInitClientLib(long lHandle);

//login
/**************************************************************************
* Function Name  : Login
* Description    : Login to the server
* Parameters     : lHandle,[in] handle of Network library;
*                  pszSvrIp, [in] server IP string;
*                  nPort, [in] server port;
*                  pszUserName, [in] login account username;
*                  pszPassword, [in] account password;
* Return value   : success  : 0;
*                  failure  : -1;
***************************************************************************/
NETCLIENT_API int  NETCLIENT_CALL Login(long lHandle, char *pszSvrIP, int nPort, char *pszUserName, char *pszPassword);

//logout
/**************************************************************************
* Function Name  : Logout
* Description    : Log out to the server
* Parameters     : lHandle,[in] handle of Network library;                
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL Logout(long lHandle);

NETCLIENT_API bool NETCLIENT_CALL SendJpeg(long lHandle, const char *pszFullFilePathName,const char *pszFileName);

//update file
NETCLIENT_API bool NETCLIENT_CALL UpdateFile(long lHandle, const char *pszFullFilePathName,const char *pszFileName);

//update a specific file, nFileType,0:System File, 1:Config File, 2:Custom File;
NETCLIENT_API bool NETCLIENT_CALL UpdateSpecFile(long lHandle, const char *pszFullFilePathName, const char *pszFileName, int nFileType);

//open realtime media stream
/**************************************************************************
* Function Name  : StartPreview, StartChnPreview
* Description    : Open real data stream, include video and audio frame data;
* Parameters     : lHandle,[in] handle of Network library;
                   nChan, [in] video stream channel(more than one sensor);
				   nStreamIndex, [in] video stream type(0:Main, 1:Sub stream);
* Return value   : success  : 0;
*                  failure  : < 0;
***************************************************************************/
NETCLIENT_API int NETCLIENT_CALL StartPreview(long lHandle);
NETCLIENT_API int NETCLIENT_CALL StartChnPreview(long lHandle, int nChan, int nStreamIndex);

//close realtime media stream
/**************************************************************************
* Function Name  : StopPreview
* Description    : Stop real data stream, include video and audio frame data; Now just support one channel stream;
* Parameters     : lHandle,[in] handle of Network library;
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL StopPreview(long lHandle);


/**************************************************************************
* Function Name  : RegistVideoDataCallback
* Description    : Set data Callback interface, provide raw stream frame to user;
* Parameters     : lHandle,[in] handle of Network library;
*                  pFunc, [in] a function pointer to the VIDEODATACALLBACK interface for user define; 
*                  dwUserData, [in] user define parameter for VIDEODATACALLBACK;
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL RegistVideoDataCallback(long lHandle, VIDEODATACALLBACK pFunc, DWORD dwUserData);

NETCLIENT_API int NETCLIENT_CALL StartJpeg(long lHandle);
NETCLIENT_API BOOL NETCLIENT_CALL StopJpeg(long lHandle);

NETCLIENT_API BOOL NETCLIENT_CALL RegistJpegDataCallBack(long lHandle, JPEGDATACALLBACK pFunc, DWORD dwUserData);

NETCLIENT_API int NETCLIENT_CALL GetAliveState(long lHandle);

NETCLIENT_API BOOL NETCLIENT_CALL GetVideoThreadState(long lHandle);

NETCLIENT_API BOOL NETCLIENT_CALL RegistKeyPadCallBack(long lHandle, KEYPADCALLBACK pFunc, DWORD dwUserData);

NETCLIENT_API BOOL NETCLIENT_CALL ReqKeyFrame(long lHandle);

NETCLIENT_API BOOL NETCLIENT_CALL SetWifiCfg(long lHandle,wifi_cfg_t *PCfg);
NETCLIENT_API BOOL NETCLIENT_CALL GetWifiCfg(long lHandle,wifi_cfg_t *PCfg);

NETCLIENT_API BOOL NETCLIENT_CALL GetVersionInfo(long lHandle, version_info_t *pVerInfo);

/**************************************************************************
* Function Name  : GetRtmpInfo
* Description    : Get RTMP broadcast URL and live stream config;
* Parameters     : lHandle,[in] handle of Network library;
*                  pRtmpInfo, [out] a point to Rtmp config(details refer to rtmp_info_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetRtmpInfo(long lHandle, rtmp_info_t *pRtmpInfo);

/**************************************************************************
* Function Name  : SetRtmpInfo
* Description    : Set RTMP broadcast URL and live stream config;
* Parameters     : lHandle,[in] handle of Network library;
*                  pRtmpInfo, [in] a point to Rtmp set(details refer to rtmp_info_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL SetRtmpInfo(long lHandle, rtmp_info_t *pRtmpInfo);


/**************************************************************************
* Function Name  : GetMvrNetInfo
* Description    : Get Mvr network config;
* Parameters     : lHandle,[in] handle of Network library;
*                  pMvrNetInfo, [out] a point to MvrNet config(details refer to mvr_net_info_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetMvrNetInfo(long lHandle, mvr_net_info_t *pMvrNetInfo);

/**************************************************************************
* Function Name  : SetMvrNetInfo
* Description    : Set Mvr network config;
* Parameters     : lHandle,[in] handle of Network library;
*                  pMvrNetInfo, [in] a point to MvrNet set(details refer to mvr_net_info_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL SetMvrNetInfo(long lHandle, mvr_net_info_t *pMvrNetInfo);

/**************************************************************************
* Function Name  : GetLiveStatus
* Description    : Get RTMP broadcast live work status : broadcasting or idle;
* Parameters     : lHandle,[in] handle of Network library;
*                  pLiveInfo, [out] a point to live push work status(details refer to live_info_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetLiveStatus(long lHandle, live_info_t *pLiveInfo);
//SetLiveStatus, Invalid to now;
NETCLIENT_API BOOL NETCLIENT_CALL SetLiveStatus(long lHandle, live_info_t *pLiveInfo);


/**************************************************************************
* Function Name  : SearchDeviceInfo
* Description    : Search online device list;
* Parameters     : pLocalInfo,[in] a point to device list buffer(details refer to local_info_t);
*                  nMaxDev, [in] device list buffer size;
*                  nTimeOut, [in] timeout in millisecond;
*                  pOptInterface, [in] must set NULL;
* Return value   : success  : searched device count;
*                  failure  : <0;
***************************************************************************/
NETCLIENT_API int NETCLIENT_CALL SearchDeviceInfo(local_info_t *pLocalInfo, int nMaxDev, int nTimeOut, char *pOptInterface);

/**************************************************************************
* Function Name  : GetDeviceInfo
* Description    : Get specified device host Info;
* Parameters     : lHandle,[in] handle of Network library;
*                : pLocalInfo,[out] a point to device host information(details refer to local_info_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetDeviceInfo(long lHandle, local_info_t *pLocalInfo);

/**************************************************************************
* Function Name  : SetOsdConfig
* Description    : Set OSD display Info;
* Parameters     : lHandle,[in] handle of Network library;
*                : pOsdConfig,[in] a point to osd config information(details refer to osd_config_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL SetOsdConfig(long lHandle, osd_config_t *pOsdConfig);

/**************************************************************************
* Function Name  : SetPTZConfig
* Description    : Set PTZ Control Info;
* Parameters     : lHandle,[in] handle of Network library;
*                : pPtzConfig,[in] a point to PTZ control command(details refer to PtzConf_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL SetPTZConfig(long lHandle, PtzConf_t *pPtzConfig);

/**************************************************************************
* Function Name  : GetSysCtrl
* Description    : Get System Function;
* Parameters     : lHandle,[in] handle of Network library;
*                : pSysCtrlInfo,[in] a point to System Special Information(details refer to sys_ctrl_result_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetSysCtrl(long lHandle, sys_ctrl_t *pSysCtrlInfo);

/**************************************************************************
* Function Name  : SetSysCtrl
* Description    : Set System Function;
* Parameters     : lHandle,[in] handle of Network library;
*                : pSysCtrlInfo,[in] a point to System Set(details refer to sys_ctrl_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL SetSysCtrl(long lHandle, sys_ctrl_t *pSysCtrlInfo);

/**************************************************************************
* Function Name  : GetNetworkCfg
* Description    : Get Network Config Info;
* Parameters     : lHandle,[in] handle of Network library;
*                : pNetworkCfg,[out] a point to Network information(details refer to NetworkConf_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetNetworkCfg(long lHandle, NetworkConf_t *pNetworkCfg);


/**************************************************************************
* Function Name  : SetNetworkCfg
* Description    : Set Network Config Info;
* Parameters     : lHandle,[in] handle of Network library;
*                : pNetworkCfg,[in] a point to Network information(details refer to NetworkConf_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL SetNetworkCfg(long lHandle, NetworkConf_t *pNetworkCfg);

/**************************************************************************
* Function Name  : GetDevTime
* Description    : Get Device Time;
* Parameters     : lHandle,[in] handle of Network library;
*                : pDevTime,[out] a point to device current time(details refer to net_time_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetDevTime(long lHandle, net_time_t *pDevTime);

/**************************************************************************
* Function Name  : SetDevTime
* Description    : Set Device Time;
* Parameters     : lHandle,[in] handle of Network library;
*                : pDevTime,[out] a point to device current time(details refer to net_time_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL SetDevTime(long lHandle, net_time_t *pDevTime);


/**************************************************************************
* Function Name  : GetUpdateProgress
* Description    : Get Current Update Progress;
* Parameters     : lHandle,[in] handle of Network library;
*                : pTotalSize,[out] bytes length of Update File;
*                : pFinishSize,[out] Has upload bytes length of File;
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetUpdateProgress(long lHandle, long *pnTotalSize, long *pnFinishSize);


// AI. get the roi region. W, H: percent of resolution. 
/**************************************************************************
* Function Name  : GetVideoRoiCfg
* Description    : Get Current Region Of Interest Location;
* Parameters     : lHandle,[in] handle of Network library;
*                : pRoiCfg,[in][out] a point to current ROI(details refer to video_roi_config_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetVideoRoiCfg(long lHandle, video_roi_config_t *pRoiCfg);

//AI,set ai roi region
/**************************************************************************
* Function Name  : SetVideoRoiArea
* Description    : Set Current Region Of Interest Location;
* Parameters     : lHandle,[in] handle of Network library;
*                : pRoiArea,[in][out] a point to current ROI(details refer to video_roi_config_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL SetVideoRoiCfg(long lHandle, video_roi_config_t *pRoiCfg);

//Add by 2022-09-28, increase RK configISP;
//get isp parameter
/**************************************************************************
* Function Name  : GetIspCfg
* Description    : Get Current ISP Config Info;
* Parameters     : lHandle,[in] handle of Network library;
*                : pIspCfg,[in][out] a point to current ISP Config(details refer to net_isp_config_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL GetIspCfg(long lHandle, net_isp_config_t *pIspCfg);

//set ISP parameter
/**************************************************************************
* Function Name  : SetIspCfg
* Description    : Set Current ISP Config Info;
* Parameters     : lHandle,[in] handle of Network library;
*                : pIspCfg,[in] a point to current ISP Config(details refer to net_isp_config_t);
* Return value   : success  : TRUE(1);
*                  failure  : FALSE(0);
***************************************************************************/
NETCLIENT_API BOOL NETCLIENT_CALL SetIspCfg(long lHandle, net_isp_config_t *pIspCfg);
//End by 2022-09-28;


#ifdef __cplusplus
}
#endif

#endif