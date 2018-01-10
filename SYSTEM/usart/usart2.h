#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
#define USART2_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收

#define BufLen	200
			
typedef struct
{
	u16 endlocation;	//当前指向位置
	u16 startLocation;	//Buf起始位置
	u16	surplus;		//Buf剩余的长度
	u8 Buf[BufLen+1];	//Buf长度
	u8 fillFlag;	//字符串是否已经填满

}uartBuff;


typedef struct
{
	u8 type;		//0:没有连接任何协议	1:TCP,2:UDP
	u8 isConnect;	//0:未连接	1:等待连接	2:已经连接
	u8 isTimeout;	//0:没有超时	1:超时
	u8 cmdStatus;	//0:Error	1:OK	2:cmd接收的命令部队
}com;


//如果想串口中断接收，请不要注释以下宏定义
void uart2_init(u32 bound);
void u2_printf(char* fmt,...);

extern u16 USART2_RX_STA;         		//接收状态标记	
extern void RX2_TX(void);
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 


extern void inputstr(const u8 str, uartBuff *Buf);
extern u8 getstr(u8 *str, uartBuff *Buf);
extern void scanstr(const u8 *str, uartBuff *Buf);
extern void readstr(u8 *str, uartBuff *Buf, int len);
extern u8 readLine(u8 *str, uartBuff *Buf, const u8 *Line, int len);
extern void initUartBuff(uartBuff *Buf);
extern void initCom(com *Com);
extern u8 comparestr(const u8 *str);
extern uartBuff Buf;

extern u8 getstring(u8 *str, uartBuff *Buff, u8 len);
extern void Myagreement(u8 *str,u8 len);
extern void scanstrLen(const u8 *str, uartBuff *Buf,u16 len);


#endif


