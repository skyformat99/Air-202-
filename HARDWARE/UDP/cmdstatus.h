#ifndef __CMDSTATUS_H
#define __CMDSTATUS_H
#include "sys.h" 
typedef struct
{
	u8 CmdStatus;			//0:��ָ����Ҫ��ȡ		1:����ȷָ����Ҫ��ȡ		2:�д����ָ��δ��ȡ
	u8 ifread;			// 0:����Ҫ��ȡ				1:��Ҫ��ȡ
	
}CmdStatus;
#endif
