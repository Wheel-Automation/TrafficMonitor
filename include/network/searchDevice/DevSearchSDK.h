#ifndef __DEV_SEARCH_LIB_H__
#define __DEV_SEARCH_LIB_H__

#include "NET_DEV_STRUCT.H"

#ifdef DEVSEARCHLIB_EXPORTS
#define DEVSEARCH_API __declspec(dllexport)
#else
#define DEVSEARCH_API __declspec(dllimport)
#endif

#define DEVSEARCH_CALL 

#ifdef __cplusplus
extern "C" {
#endif

#define GSF_VERSION_LEN 64

typedef enum _DEVSEARCH_PROTOL_TYPE
{
	DEVSEARCH_PROTOL_GSF,		// 私有协议
	DEVSEARCH_PROTOL_ONVIF,		// ONVIF协议
	DEVSEARCH_PROTOL_JXJ,		// JXJ协议
	DEVSEARCH_PROTOL_GLINK_P2P,	// GOOLINK P2P协议
	DEVSEARCH_PROTOL_TUTK_P2P,	// TUTK P2P协议
	DEVSEARCH_PROTOL_GSF_V2,	// 私有协议特殊版本V2
	DEVSEARCH_PROTOL_GSF_V3,	// 私有协议特殊版本V3
}DEVSEARCH_PROTOL_TYPE;

typedef enum _DEVSEARCH_LOG_LEVEL
{
	DEVSEARCH_LOG_LEVEL_NONE,	// 没有日志打印
	DEVSEARCH_LOG_LEVEL_ERR,	// 打印错误日志
	DEVSEARCH_LOG_LEVEL_WARN,	// 打印警告、错误日志
	DEVSEARCH_LOG_LEVEL_INFO,	// 打印除调试日志外的所有日志
	DEVSEARCH_LOG_LEVEL_DEBUG,	// 打印所有日志，包括调试日志
}DEVSEARCH_LOG_LEVEL;

typedef struct _GSF_DevSearchVersion
{
	char Version[GSF_VERSION_LEN];
}GSF_DevSearchVersion;

typedef int (DEVSEARCH_CALL *GsfLogCallback)(char *log);
typedef int (DEVSEARCH_CALL *GsfDevSearchCallback)(const NET_DEV_SADPINFO* SadpInfo, void* UserParam);
typedef int (DEVSEARCH_CALL *GsfNicCallback)(char *name, char *desc, char *mac, char *ip, void *user);

typedef struct _GSF_DevSearchParam
{
	DEVSEARCH_PROTOL_TYPE  ProtolType;
	void* UserParam;
	int bUseMultiCastOnly;	// 搜索协议仅使用组播, 默认使用广播+组播
	GsfDevSearchCallback Callback;	// 搜索到设备后回调，每次回调一条信息
	GsfNicCallback NicCallback;	// 绑定的网卡信息回调
	void* NicUserParam;
}GSF_DevSearchParam;

//版本信息
DEVSEARCH_API void DEVSEARCH_CALL GSF_DEV_SEARCH_VERSION(GSF_DevSearchVersion* Version);
//初始化
DEVSEARCH_API int DEVSEARCH_CALL GSF_DEV_SEARCH_INIT();
//初始化
DEVSEARCH_API void DEVSEARCH_CALL GSF_DEV_SEARCH_UNINIT();
//设置日志等级
DEVSEARCH_API void DEVSEARCH_CALL GSF_DEV_SEARCH_SET_LOG_CFG(DEVSEARCH_LOG_LEVEL Level, GsfLogCallback LogCB);
//搜索设备
DEVSEARCH_API int DEVSEARCH_CALL GSF_DEV_SEARCH_START(const GSF_DevSearchParam* Param);
//停止搜索设备
DEVSEARCH_API int DEVSEARCH_CALL GSF_DEV_SEARCH_STOP();

//设置设备IP信息
DEVSEARCH_API int DEVSEARCH_CALL GSF_DEV_SEARCH_SET_INFO(const NET_DEV_MODIFY_NET* Param, BOOL bSync = TRUE);

#ifdef __cplusplus
}
#endif

#endif