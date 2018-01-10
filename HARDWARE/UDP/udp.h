#ifndef __UDP_H
#define __UDP_H
#include <string.h>
#include "delay.h"
#include "usart2.h"
#include "cmdstatus.h"

#include "includes.h"
#include "sem.h"
#include "agreement.h"




extern const u8 Line[];
extern const u8 certain[];

extern const u8 AT[];
extern const u8 UDP[];
extern const u8 connectOK[];
extern const u8 UDPSend[];
extern const u8 SendFlag[];
extern const u8 SendResult[];
extern const u8 UDPClose[];
extern const u8 CloseResult[];
extern const u8 OK[];
extern const u8 Error[];
extern const u8 AConnect[];
extern const u8 ATCREG[];
extern const u8 CREGResult[];
extern const u8 RecServer[];
extern const u8 Restart[];

extern const u8 tail[3];
extern const u8 head[3];


extern com Com;
extern CmdStatus AT_CMD;
extern CmdStatus CIPSTART_CMD;
extern CmdStatus CIPSEND_CMD;
extern CmdStatus CIPCLOSE_CMD;
extern CmdStatus CREG_CMD;
extern CmdStatus SendResult_CMD;
extern u8 read[200];


extern u8 udpSendMsg(signed int MaiChong);
extern u8 udpConnect(const u8 *CMD);
extern u8 isSign(void);
extern void initCmd(void);
extern u8 ReadResponseTimeOut(CmdStatus *CMD,u16 timeout);
extern void u2_OutCmd(CmdStatus *CMD,const u8 *Air202CMD);








#endif
