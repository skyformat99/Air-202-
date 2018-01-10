#ifndef __CMDSTATUS_H
#define __CMDSTATUS_H
#include "sys.h" 
typedef struct
{
	u8 CmdStatus;			//0:无指令需要读取		1:有正确指令需要读取		2:有错误的指令未读取
	u8 ifread;			// 0:不需要读取				1:需要读取
	
}CmdStatus;
#endif
