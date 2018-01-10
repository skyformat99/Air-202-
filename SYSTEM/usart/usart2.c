#include "sys.h"
#include "usart2.h"
#include "stdio.h"	 	 
#include "string.h"	
#include "stdarg.h"	
#include "udp.h"
#include "agreement.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

__align(8) u8 USART2_TX_BUF[200]; 
#define USART_REC_LEN  			200  
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
////////////////////////////////////////////////////////////////////////////////// 	  
 



/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART2_RX   //如果使能了接收
u16 USART2_RX_STA=0;       //接收状态标记+长度  
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.


uartBuff Buf;

 void execute2(u16 t)//串口接收数据后执行的函数，在void RX_TX(void)里面执行
 {
	 	USART_SendData(USART1, USART2_RX_BUF[t]);//向串口1发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
 }

  
void uart2_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//使能USART1，GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口1 

}


void u2_printf(char* fmt,...)  
{  
	int t,i=0;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	t=strlen((const char*)USART2_TX_BUF);
	while(t--)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		USART_SendData(USART2, USART2_TX_BUF[i++]);
	}
}

void USART2_IRQHandler(void)                	//串口1中断服务程序
	{
		u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
		
		
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			Res =USART_ReceiveData(USART2);	//读取接收到的数据
			//while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
			//USART_SendData(USART1, Res);//向串口1发送数据
			inputstr(Res,&Buf);
			/*
		if((USART2_RX_STA&(1<<15))==0)
		{ 
			if(USART2_RX_STA<USART_REC_LEN)	
			{
				TIM_SetCounter(TIM1,0);
				if(USART2_RX_STA==0) 			
				{
					TIM_Cmd(TIM1,ENABLE);
				}
				USART2_RX_BUF[USART2_RX_STA++]=Res;	
			}else 
			{
				USART2_RX_STA|=1<<15;	
			} 
		}
		
				*/
     } 
		
		 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	


void inputstr(const u8 str, uartBuff *Buf)
{
	if (Buf->surplus != 0)
	{
		memcpy((Buf->Buf + Buf->endlocation), &str, 1);
		if (Buf->endlocation != BufLen)
		{
			Buf->endlocation += 1;
		}
		else
		{
			Buf->endlocation = 0;
		}
		Buf->surplus -= 1;
	}
}

u8 getstr(u8 *str, uartBuff *Buf)
{
	if (Buf->surplus != (BufLen+1))
	{
		memcpy(str, (Buf->Buf + Buf->startLocation), 1);
		if (Buf->startLocation != BufLen)
		{
			Buf->startLocation += 1;
		}
		else
		{
			Buf->startLocation = 0;
		}
		Buf->surplus += 1;
		return 1;
	}
	return 0;
}


void scanstr(const u8 *str, uartBuff *Buf)
{
	int len = 0;
	len = strlen((char *)str);
	for (;len > 0;len--)
	{
		inputstr(*str, Buf);
		str++;
	}
}

void readstr(u8 *str, uartBuff *Buf ,int len)
{
	memset(str, 0, len);
	for (;len > 0;len--)
	{
		getstr(str, Buf);
		str++;
	}
}

u8 readLine(u8 *str, uartBuff *Buf, const u8 *Line, int len)
{
	int i = 0;
	int j = 0;
	memset(str, 0, len);
	for (;j<=len;j++)
	{
		if (getstr(str, Buf) == 0)
		{
			if(j == 0)
			{
				return 0;		//没有数据
			}
			else
			{
				return 2;		//读完所有数据
			}
		}
		if (*str == *(Line+i))
		{
			i++;
			if (*(Line + i) == 0)
			{
				*str-- = 0;
				*str = 0;
				break;
			}
		}
		else
		{
			if(i > 0)
			{
				i = 0;
				if (*str == *(Line+i))
				{
					i++;
				}
			}
		}
		str++;
	}
	return 1;
}


u8 getstring(u8 *str, uartBuff *Buff, u8 len)
{
	for (;len > 0;len--)
	{
		getstr(str, &Buf);
		str++;
	}
	return 0;
}


void Myagreement(u8 *str,u8 len)
{
	union Tdata num;
	memcpy(num.str, str, 1);
	if ((waterType == 0 || waterType == 2) && num.u8number == 2)		//服务器手机端控制上线
	{
		printf("手机端:\n");
		printf("供水类型:%d\n", num.u8number);
		waterType = num.u8number;
		str++;

		memcpy(num.str, str, 1);
		printf("继电器状态:%d\n", num.u8number);
		switchstatus = num.u8number;		//控制继电器代码
		str++;

		memcpy(num.str, str, 2);
		printf("随机码:%d\n", num.u16number);
		randomcode = num.u16number;	
		str += 2;

		memcpy(num.str, str, 4);
		printf("卡号:%d\n", num.u32number);
		cardNum = num.u32number;		
		str += 4;

		memcpy(num.str, str, 4);
		printf("余额:%f\n", num.Fnumber);
		money = num.Fnumber;
		str += 4;

		memcpy(num.str, str, 4);
		printf("单价:%f\n", num.Fnumber);
		unitprice = num.Fnumber;
		str += 4;

		memcpy(num.str, str, 1);
		printf("1升水脉冲数:%d\n", num.u8number);
		everyL = num.u8number;
		str++;

		memcpy(num.str, str, 2);
		printf("发送次数:%d\n", num.u16number);
		sendTimes = num.u16number;
		str += 2;

		memcpy(num.str, str, 1);
		printf("CRC校验%d\n", num.u8number);
		CRC_checkout = num.u8number;

	}
	else if(waterType == 2 && num.u8number ==0)			//服务器手机端控制下线
	{
		printf("手机端已经离线\r\n");
		waterType = 0;
		switchstatus = 0;
		randomcode = 0;
		cardNum = 0;
		money = 0;
		unitprice = 0;
		everyL = 0;
		sendTimes = 0;
		CRC_checkout = 0;
	}
	else if (waterType == 1 && num.u8number == 1)		//刷卡端,得到随机码
	{
		printf("刷卡端:\n");
		printf("供水类型:%d\n", num.u8number);
		waterType = num.u8number;
		str++;

		memcpy(num.str, str, 1);
		printf("继电器状态:%d\n", num.u8number);
		switchstatus = num.u8number;
		str++;

		memcpy(num.str, str, 2);
		printf("随机码:%d\n", num.u16number);
		randomcode = num.u16number;
		str += 2;

		memcpy(num.str, str, 4);
		printf("卡号:%d\n", num.u32number);
		cardNum = num.u32number;
		str += 4;

		memcpy(num.str, str, 4);
		printf("余额:%f\n", num.Fnumber);
		money = num.Fnumber;
		str += 4;

		memcpy(num.str, str, 4);
		printf("单价:%f\n", num.Fnumber);
		unitprice = num.Fnumber;
		str += 4;

		memcpy(num.str, str, 1);
		printf("1升水脉冲数:%d\n", num.u8number);
		everyL = num.u8number;
		str++;

		memcpy(num.str, str, 2);
		printf("发送次数:%d\n", num.u16number);
		sendTimes = num.u16number;
		str += 2;

		memcpy(num.str, str, 1);
		printf("CRC校验%d\n", num.u8number);
		CRC_checkout = num.u8number;
	}
	else if (waterType == 0 && num.u8number == 0)
	{
		printf("无设备连接\r\n");
	}
	else
	{
		printf("服务器与单片机端不同步\n");
	}


}


u8 comparestr(const u8 *str)
{
	int i = 1;
	u8 len;
	for (;;i++)
	{
		switch (i)
			{
				case 1:
					if (strcmp((char *)str, (char *)AT) == 0 && AT_CMD.ifread == 1) 
					{
						readLine(read, &Buf, Line, 200);
						while(strlen((char *)read) == 0)
						{
							if(readLine(read, &Buf, Line, 200) == 0 )
							{
								AT_CMD.CmdStatus = 2;
								return 0;
							}
						}
						if (strcmp((char *)str, (char *)OK) == 0)
						{
							AT_CMD.CmdStatus = 1;
							return i;
						}
						else if (strcmp((char *)str, (char *)Error) == 0)
						{
							AT_CMD.CmdStatus = 2;
							return 0;
						}
					}break;
				case 2:
					if (strcmp((char *)str, (char *)UDP) == 0)
					{
						printf("%s",str);
						readLine(read, &Buf, Line, 200);
						while(strlen((char *)read) == 0)
						{
							if(readLine(read, &Buf, Line, 200) == 0)
							{
								return 0;
							}
						}
						if (strcmp((char *)str, (char *)OK) == 0)
						{
							Com.type = 2;
							Com.cmdStatus = 1;
						}
						else if (strcmp((char *)str, (char *)Error) == 0)
						{
								Com.type = 0;
								Com.cmdStatus = 0;
						}
						else
						{
							Com.type = 0;
							Com.cmdStatus = 2;
						}
						return i;
					}break;
				case 3:
					if (strcmp((char *)str, (char *)UDPSend) == 0 && CIPSEND_CMD.ifread == 1) 
					{
						printf("%s",read);
						
						while(readLine(read, &Buf, Line, 200) !=0)
						{
							if(strlen((char *)read) == 0)
							{
							}
							else
							{
								if (strcmp((char *)str, (char *)SendFlag) == 0)
								{
									CIPSEND_CMD.CmdStatus = 1;
									return i;
								}
								else
								{
									CIPSEND_CMD.CmdStatus = 2;
									CIPSTART_CMD.CmdStatus = 0;
									Com.isConnect = 0;
									return 0;
								}
							}
						}
						return 0;
					}break;
				case 4:
					if (strcmp((char *)str, (char *)UDPClose) == 0 && CIPCLOSE_CMD.ifread == 1) 
					{
						CIPCLOSE_CMD.CmdStatus = 1;
						return i;
					}break;
				case 5:if (strcmp((char *)str, (char *)SendFlag) == 0) return i;break;
				case 6:
					if (strcmp((char *)str, (char *)SendResult) == 0 && SendResult_CMD.ifread == 1) 
					{
						SendResult_CMD.CmdStatus = 1;
						return i;
					}break;
				case 7:
					if (strcmp((char *)str, (char *)connectOK) == 0 && CIPSTART_CMD.ifread == 1)
					{		
							CIPSTART_CMD.CmdStatus = 1;
							Com.isConnect = 2;
						return i;
					}break;
				case 8:if (strcmp((char *)str, (char *)CloseResult) == 0) return i;break;
				case 9:if (strcmp((char *)str, (char *)OK) == 0) return i;break;
				case 10:if (strcmp((char *)str, (char *)Error) == 0) return i;break;
				case 11:if (strcmp((char *)str, (char *)AConnect) == 0 && CIPSTART_CMD.ifread == 1) 
									{
										CIPSTART_CMD.CmdStatus = 1;
										Com.isConnect = 2;
										return i;
									}
										break;
				case 12:
					if (strcmp((char *)str, (char *)ATCREG) == 0 && CREG_CMD.ifread == 1) 
					{
						readLine(read, &Buf, Line, 200);
						while(strlen((char *)read) == 0)
						{
							if(readLine(read, &Buf, Line, 200) == 0 )
							{
								CREG_CMD.CmdStatus = 2;
								return 0;
							}
						}
						if (strcmp((char *)str, (char *)CREGResult) == 0) 
						{
							readLine(read, &Buf, Line, 200);
							while(strlen((char *)read) == 0)
							{
								if(readLine(read, &Buf, Line, 200) == 0 )
								{
									CREG_CMD.CmdStatus = 2;
									return 0;
								}
							}	
							if (strcmp((char *)str, (char *)OK) == 0)
							{
								CREG_CMD.CmdStatus = 1;
								return i;
							}
						}
					}
					break;
				case 13:
					if (strcmp((char *)str, (char *)RecServer) == 0)
					{
						printf("接收到服务器数据\r\n\r\n");
						return i;
					}break;
				case 14:
					if (strcmp((char *)str, (char *)Restart) == 0)
					{
						printf("GSM模块已经重启\r\n");
						return i;
					}
				case 15:
					if (strcmp((char *)str, (char *)head) == 0)		//服务器发送的数据
					{
					getstr(&len, &Buf);
					printf("内容长度为%d\n", len);
					memset(read, 0, len);
					getstring(read,&Buf,len);
					getstring(RecTail, &Buf, 2);
					Myagreement(read, len);
					if (strcmp((char *)RecTail, (char *)tail) == 0)
					{
						return i;
					}
					else
					{
						return 0;
					}
				}break;
				default:
					return 100;
			}
	}
}

void initUartBuff(uartBuff *Buf)
{
	Buf->endlocation = 0;
	Buf->startLocation = 0;
	Buf->fillFlag = 0;
	Buf->surplus = BufLen+1;
	memset(Buf->Buf, 0, BufLen+1);
}

void initCom(com *Com)
{
	Com->cmdStatus = 0;
	Com->isConnect = 0;
	Com->isTimeout = 0;
	Com->type = 0;
}

void scanstrLen(const u8 *str, uartBuff *Buf,u16 len)
{
	for (;len > 0;len--)
	{
		inputstr(*str, Buf);
		str++;
	}
}


