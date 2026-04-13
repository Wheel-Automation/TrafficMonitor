#ifndef __ERROR_H__
#define __ERROR_H__

///////////////////////////////////////////////////////////////////////////////
//Í¨ÓÃ´íÎó
#define LYF_ERR_GNL_SUCCESS				0x20000000			// ³É¹¦
#define	LYF_ERR_GNL_UNKNOW				0x20000001			// Î´Öª´íÎó
#define LYF_ERR_GNL_PARAM				0x20000002			// ²ÎÊý²»ÕýÈ·
#define LYF_ERR_GNL_PSD					0x20000004			// ÃÜÂë´íÎó
#define LYF_ERR_GNL_CET_THREAD			0x20000005			// ´´½¨Ïß³ÌÊ§°Ü
#define LYF_ERR_GNL_INV_HANDLE			0x20000006			// ÎÞÐ§µÄ¾ä±ú
#define LYF_ERR_GNL__TIME				0x20000007			// Ê±¼ä×ª»»³ö´í
#define LYF_ERR_GNL_NOINIT				0x20000008			// Î´³õÊ¼»¯
#define LYF_ERR_GNL_HASHCOUNT			0x20000009			// HASH´íÎó³¬´Î

///////////////////////////////////////////////////////////////////////////////
//¼ÓÃÜËø´íÎó
#define LYF_ERR_DOG_PSD_USR_PSD			0x20000103			// ÓÃ»§ÃÜÂë»ò¹ÜÀíÃÜÂë´íÎó
#define LYF_ERR_DOG_USR_PSD				0x20000104			// ÓÃ»§ÃÜÂë´íÎó
#define LYF_ERR_DOG_LMT					0x20000105			// ÆÚÏÞÃÜÂë´íÎó
#define LYF_ERR_DOG_NODOG				0x20000106			// ÕÒ²»µ½¼ÓÃÜ¹· 
#define LYF_ERR_DOG_PSD					0x20000107			// ¹ÜÀíÃÜÂë´íÎó
#define LYF_ERR_DOG_AXPSD				0x20000108			// ¶¯Ì¬ÃÜÂë´íÎó
#define LYF_ERR_DOG_HW					0x20000109			// µ×²ãÓ²¼þ²Ù×÷Ê§°Ü
#define LYF_ERR_DOG_ADDR				0x2000010A			// µØÖ·Ð£Ñé´íÎó

///////////////////////////////////////////////////////////////////////////////
//Ê±ÖÓËø´íÎó
#define LYF_ERR_8K_RESETTIME_ONCE_IN_ONE_DAY		0x20000201			// Ã¿ÌìÖ»ÄÜÖØÉèÒ»´ÎÊ±¼ä
#define LYF_ERR_8K_RESETTIME_3MIN_ONE_DAY		0x20000202			// Ã¿ÌìÔÊÐí×î´óÊ±¼äÎó²îÎª3·ÖÖÓ
#define LYF_ERR_8K_HW_SETTIME				0x20000203			// ÉèÖÃÊ±¼ä³ö´í
#define LYF_ERR_8K_HW_READTIME				0x20000204			// ¶ÁÈ¡Ê±¼ä³ö´í
#define LYF_ERR_8K_HW_UNKNOW_LMT_TYPE			0x20000205			// Î´ÖªµÄ¼ÆÊ±¼Æ´ÎÀàÐÍ

///////////////////////////////////////////////////////////////////////////////
#endif	//endif __ERROR_H__