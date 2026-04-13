#ifndef _SKINPPWTL_H_
#define _SKINPPWTL_H_

#ifdef _SKINPP_STATIC
	#define SKINPPWTL_API
#else
	#ifdef SKINPPWTL_EXPORTS
		#define SKINPPWTL_API __declspec(dllexport)
	#else
		#define SKINPPWTL_API __declspec(dllimport)
	#endif
#endif

#define WM_TOOLBARPAINTPRE (WM_USER + 802)
#define WM_TOOLBARPAINTEND (WM_USER + 803)

#define SM_LBUTTONUP (WM_USER + 804)

//°´Å¥×´Ì¬
enum BUTTONSTATE 
{
	NORMAL	= 0,
	PRESS	= 1,
	DISABLE	= 2,
	HOT		= 3,
	FOCUS   = 4,
	LAST	= 5
};

//»æÖÆÀàÐÍ
enum DRAWTYPE
{
	BUTTON = 0,
	SCROLLARROWUP = 1,
	SCROLLARROWDOWN = 2,
	SPLITTERBARHORZ = 3,
	SPLITTERBARVERT = 4,
	SPLITTERBARBORDER = 5,
	LISTHEAD = 6
};

//»ñµÃÆ¤·ô×ÊÔ´µÄÀàÐÍ
enum SKINOBJTYPE
{
	DIALOGTYPE = 0,
	BUTTONTYPE = 1,
	CHECKBOXTYPE = 2,
	RADIOBOXTYPE = 3,
	STATICTYPE = 4,
	TRACKBARTYPE = 5,
};

struct ListBoxItem
{
	HIMAGELIST	hImageList;
	int			nImageIndex;

	ListBoxItem()
	{
		hImageList = NULL;
		nImageIndex = -1;
	}
};

#define REST_BITMAP      0x0001 //.bmp
#define REST_ICON        0x0002 //.ico
#define REST_CURSOR      0x0003 //.cur
#define REST_ANIMATE	 0x0004 //.ani

typedef struct _ResourceInfo
{
	HGDIOBJ hGdiObj;//[OUT]
	DWORD   dwType; //[OUT]
	int		nWidth; //[OUT]
	int		nHeight;//[OUT]

	TCHAR	szResImageName[_MAX_FNAME];//[IN]
	BOOL	bHorzSplit;//[IN]
	int		nLength;//[IN]
	int		nCount; //[IN]
	int		nIndex; //[IN]
	
	_ResourceInfo()
	{
		hGdiObj = NULL;
		dwType = REST_BITMAP;
		nWidth = 0;
		nHeight = 0;

		_tcscpy(szResImageName,_T(""));
		bHorzSplit = TRUE;
		nLength = -1;
		nCount = -1;
		nIndex = -1;
	}
	
}ResInfo,* PRESINFO;

//////////////////////////////////////////////////////////////////////////

//¼ÓÔØÆ¤·ô
//SkinFile		:Æ¤·ôÂ·¾¶,×¢Òâ¿ÉÒÔÊÇ*.ssk,Ò²¿ÉÒÔÊÇÆ¤·ôÄ¿Â¼ÖÐµÄINIÎÄ¼þ.
//bFromIni		:¸Ã²ÎÊýÖ¸¶¨Æ¤·ôÎÄ¼þÊÇ´Ó*.ssk¶ÁÈ¡,»¹ÊÇ´ÓINIÎÄ¼þ¶ÁÈ¡.
SKINPPWTL_API BOOL  skinppLoadSkin(TCHAR* szSkinFile,BOOL bFromIni = FALSE);

SKINPPWTL_API BOOL skinppLoadSkinFromRes(HINSTANCE hInstance,LPCTSTR szResourceName,
										 LPCTSTR szResourceType,TCHAR* szSkinFileName);

//ÒÆ³ýÆ¤·ô
SKINPPWTL_API BOOL  skinppRemoveSkin();

//ÍË³ö½çÃæ¿â£¬×öÇåÀí¹¤×÷¡£
SKINPPWTL_API BOOL  skinppExitSkin();

//ÉèÖÃListBox¿Ø¼þµÄ×Ô»­ÐÅÏ¢
//hWnd			: ListBox¿Ø¼þµÄ¾ä±ú
//nIndex		: ItemÏîµÄË÷Òý
//pListBoxItem	: ItemÏî×Ô»­µÄ½á¹¹ÐÅÏ¢
SKINPPWTL_API void skinppSetListBoxItemDrawInfo(HWND hWnd,int nIndex,struct ListBoxItem* pListBoxItem);

//»ñµÃ»»·ôºóµÄÏµÍ³ÑÕÉ«
//nColorIndex	: Òª»ñÈ¡µÄÑÕÉ«ÀàÐÍ
SKINPPWTL_API COLORREF skinppGetSkinSysColor(int nColorIndex);

//»ñµÃWindowsÏµÍ³Ä¬ÈÏµÄÑÕÉ«
//nColorIndex	: Òª»ñÈ¡µÄÑÕÉ«ÀàÐÍ
SKINPPWTL_API COLORREF skinppGetDefaultSysColor(int nColorIndex);

//hWnd			: ¶Ô»°¿ò´°¿ÚµÄ¾ä±ú
//nResID		: ¶Ô»°¿ò×ÊÔ´ID
SKINPPWTL_API BOOL skinppSetWindowResID(HWND hWnd,int nResID);//[¶àÓïÑÔ]

SKINPPWTL_API BOOL skinppSetFreeDlgID(HWND hWnd,int nResID);

SKINPPWTL_API BOOL skinppSetSkinResID(HWND hWnd,int nResID);

//ÉèÖÃListHeader´°¿ÚµÄÅÅÐòÐÅÏ¢
//hWnd			: ListHeaderµÄ´°¿Ú¾ä±ú
//nSortColumn	: Òª¶ÔListHeaderÅÅÐòµÄÁÐµÄË÷Òý
//bSortAscending: ÊÇ·ñÎªÉýÐò
SKINPPWTL_API void skinppSetListHeaderSortInfo(HWND hWnd,int nSortColumn,BOOL bSortAscending = TRUE);

//ÔÚ¸ø¶¨µÄHDCÉÏ,Ö¸¶¨ÏàÓ¦µÄ»æÖÆÀàÐÍºÍ×´Ì¬,ÔÚÏàÓ¦µÄ¾ØÐÎÇøÓòÖÐ½øÐÐ»æÖÆ.
//hdc			:Ä¿±êDC
//rect			:»æÖÆÇøÓò
//eDrawType		:»æÖÆÀàÐÍ£¬Ä¿Ç°Ö§³ÖSPLITTERBARHORZ,SPLITTERBARVERT,SPLITTERBARBORDER
//nState		:Ñ¡Ôñ»æÖÆ×´Ì¬
SKINPPWTL_API void  skinppDrawSkinObject(HDC hdc,RECT rect,DRAWTYPE eDrawType,int nState);

//Í¨¹ý×ÊÔ´ID,»ñµÃÏàÓ¦ÀàÐÍµÄÆ¤·ô×ÊÔ´Î»Í¼¾ä±ú
//nSkinObjType	: Æ¤·ôÀàÐÍ,Ä¿Ç°Ö§³Ö	DIALOGTYPE,BUTTONTYPE,CHECKBOXTYPE,RADIOBOXTYPE
//nResID		: ×ÊÔ´ID
//nState		: ×´Ì¬,¶ÔBUTTONTYPE,CHECKBOXTYPE,RADIOBOXTYPEÓÐÐ§
SKINPPWTL_API HBITMAP skinppGetResFromID(SKINOBJTYPE nSkinObjType,int nResID,int nState =0 );

//ÉèÖÃÊÇ·ñ×Ô¼º»­¶Ô»°¿ò±³¾°,¸Ã·½·¨ÓÃÔÚÐèÒª×Ô¼º¶Ô±³¾°½øÐÐ´¦ÀíµÄÇé¿öÏÂ.
//hWnd			: ¶Ô»°¿òµÄ¾ä±ú
//bErase		: TRUE Îª×Ô¼º»­±³¾°,FALSE ÎªSkin++»­,Èç¹ûÃ»ÓÐµ÷ÓÃ¸Ã·½·¨,Skin++½«»­¶Ô»°¿ò±³¾°.
SKINPPWTL_API void skinppSetDialogEraseBkgnd(HWND hWnd,BOOL bErase);

//ÉèÖÃ¶Ô»°¿ò±³¾°ÊÇ·ñ¼ôÇÐ×Ó¿Ø¼þÇøÓò¡£
//hWnd			: ¶Ô»°¿ò¾ä±ú
//bNoClip		: TRUEÎª²»ÐèÒª¼ôÇÐ,FALSEÎªÐèÒª¼ôÇÐÇøÓò
//bAllChild		: TRUEÎª¸Ã´°ÌåµÄËùÓÐ×Ó¶Ô»°¿ò¶¼¼ôÇÐ.
SKINPPWTL_API void skinppSetDialogBkClipRgn(HWND hWnd,BOOL bClip,BOOL bAllChild = TRUE);

//Í¨¹ýÆ¤·ô×ÊÔ´Ãû³Æ»ñµÃÆ¤·ô×ÊÔ´ÖÐÎ»Í¼
//szName		: Æ¤·ô×ÊÔ´Ãû³Æ
//HBITMAP		: ·µ»Ø×ÊÔ´ÖÐµÄÎ»Í¼
SKINPPWTL_API HBITMAP skinppGetBitmapRes(LPCTSTR szName);

//Í¨¹ý×ÊÔ´Ãû³ÆÈ¡×ÊÔ´µÄÄÚ´æÖ¸Õë
//szName		: ×ÊÔ´Ãû³Æ
//nSize			: ×ÊÔ´´óÐ¡
//pByte			: ·µ»ØÖµ£¬³É¹¦·µ»Ø·ÇNULL£¬Ê§°Ü·µ»ØNULL
SKINPPWTL_API BYTE*  skinppGetSkinResource(LPCTSTR szName,int& nSize);

//Í¨¹ýÆ¤·ô×ÊÔ´µÄÃû³Æ»ñµÃÎ»Í¼²»±»À­ÉìµÄÇøÓòÖµ
//szName		: Æ¤·ô×ÊÔ´Ãû³Æ
//nTopHeight	: ·µ»Ø²»±»À­ÉìµÄ¶¥¸ß
//nBottomHeight : ·µ»Ø²»±»À­ÉìµÄµ×¸ß
//nLeftWidth	: ·µ»Ø²»±»À­ÉìµÄ×ó¿í
//nRightWidth	: ·µ»Ø²»±»À­ÉìµÄÓÒ¿í
SKINPPWTL_API BOOL skinppGetBitmapResRect(LPCTSTR szName,int& nTopHeight,int& nBottomHeight,
												   int& nLeftWidth,int& nRightWidth);

//ÉèÖÃ´°¿Ú×Ô»­ÊÇ·ñ×Ô¼ºÀ´´¦Àí,¸Ã·½·¨ÓÃÓÚ×Ô»­²¿·ÖÐèÒª×Ô¼º´¦ÀíµÄÇé¿öÏÂ
//hWnd			: Òª×Ô»­µÄ´°¿Ú¾ä±ú
//bCustomDraw	: TRUEÎª×Ô¼º´¦Àí×Ô»­,FALSEÎª½»¸øSkin++´¦Àí×Ô»­
SKINPPWTL_API void skinppSetCustomDraw(HWND hWnd,BOOL bCustomDraw);

//ÉèÖÃ²Ëµ¥µÄÆ¤·ô±êÊ¶
//hWnd			: ÓµÓÐ²Ëµ¥µÄ´°¿Ú¾ä±ú
//nSkinObjectID	: ²Ëµ¥Æ¤·ôµÄ±êÊ¶
SKINPPWTL_API void skinppSetMenuSkinObjectID(HWND hWnd,int nSkinObjectID);

//ÉèÖÃÊÇ·ñ¶Ô×Ô»­²Ëµ¥½øÐÐ»»·ô
//bSkin			: TRUEÎª»»·ô
SKINPPWTL_API void skinppSetSkinOwnerMenu(BOOL bSkin);

//¶Ô²Ëµ¥½øÐÐ»»·ô¿ØÖÆ
//hMenu   : Ïë»»·ôµÄ²Ëµ¥¾ä±ú
//bNoSkin : ÊÇ·ñ»»·ô,TRUEÎª²»»»·ô,FALSEÎª»»·ô
SKINPPWTL_API void  skinppSetDrawMenu(HMENU hMenu,BOOL bNoSkin);
	
//¶ÔÖ¸¶¨µÄ´°¿ÚÈ¥µôÆ¤·ô,²¢ÇÒ±£Ö¤²»»áÔÙ±»»»·ô,¼´Ê¹Ê¹ÓÃSetSkinHwndÒ²²»»á»»·ô.
//hWnd			: Ö¸¶¨µÄ´°¿Ú¾ä±ú
//bChildNoSkin	: ÊÇ·ñ¶Ô¸Ã´°¿ÚÖÐµÄ×Ó´°¿ÚÈ¥µôÆ¤·ô
SKINPPWTL_API void skinppSetNoSkinHwnd(HWND hWnd,BOOL bChildNoSkin = TRUE);

//¶ÔÖ¸¶¨µÄ´°¿Ú½øÐÐ»»·ô
//hWnd			: Ö¸¶¨µÄ´°¿Ú¾ä±ú
//szClassName	: Òª×ÓÀà»¯µÄSkinÀàÐÍ WC_DIALOGBOX/WC_CONTROLBARµÈ
SKINPPWTL_API void skinppSetSkinHwnd(HWND hWnd,LPCTSTR szClassName = NULL);

//¶ÔÖ¸¶¨µÄ´°¿ÚÁÙÊ±È¥µôÆ¤·ô,¿ÉÒÔÍ¨¹ýSetSkinHwnd½øÐÐÔÙ´Î»»·ô
SKINPPWTL_API void skinppRemoveSkinHwnd(HWND hWnd);

//ÊÇ¶ÔSetNoSkinHwndµÄ½øÒ»²½´¦Àí,¿ÉÒÔ½â¾öÊ¹ÓÃSetNoSkinHwndÒýÆðµÄDebug°æµÄ¶ÏÑÔ´í
#define SETNOSKINHWND(x) {\
    HWND w=(x).UnsubclassWindow();\
	skinppSetNoSkinHwnd(w);\
	(x).SubclassWindow(w);\
} 

//ÊÇ¶ÔRemoveSkinHwndµÄ½øÒ»²½´¦Àí,¿ÉÒÔ½â¾öÊ¹ÓÃRemoveSkinHwndÒýÆðµÄDebug°æµÄ¶ÏÑÔ´í
#define REMOVESKINHWND(x){\
    HWND w=(x).UnsubclassWindow();\
	skinppRemoveSkinHwnd(w);\
	(x).SubclassWindow(w);\
}

SKINPPWTL_API HGDIOBJ skinppGetResFromID(PRESINFO pResInfo);


#endif //_SKINPPWTL_H_

