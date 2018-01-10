#ifndef __AGREEMENT_H
#define __AGREEMENT_H
#include "includes.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "udp.h"
#include "sem.h"
#include "checkway.h"
union Tdata	//对一些数字转成字符串
{
	u32 u32number;
	u16 u16number;
	u8 	u8number;
	float Fnumber;
	u8 str[4];
};

extern u8 waterType;
extern u8 switchstatus;
extern u16 randomcode;
extern u32 cardNum;
extern float money;
extern float unitprice;
extern u8 everyL;
extern u16 sendTimes;
extern u8 CRC_checkout;
extern u8 RecTail[2];
extern u16 pulseTotal;

extern const u8 datahead[];
extern const u8 datatail[];
extern const u8 agreement[];

//pulse任务
//设置任务优先级
#define agreement_TASK_PRIO       			7
//设置任务堆栈大小
#define agreement_STK_SIZE  					256
//任务堆栈
extern OS_STK agreement_TASK_STK[agreement_STK_SIZE];
void agreement_task(void *pdata);
extern u8 agreementSend(void);
#endif
