#include "UDP.h"

const u8 Line[] = "\r\n";
const u8 certain[]="";



const u8 AT[] = "AT\r";
const u8 UDP[] = "AT+CIPSTART=\"UDP\",\"112.91.100.219\",60000\r";
const u8 connectOK[] = "CONNECT OK";
const u8 UDPSend[] = "AT+CIPSEND=25\r";
const u8 SendFlag[] = "> ";
const u8 SendResult[] = "SEND OK";
const u8 UDPClose[] = "AT+CIPCLOSE\r";
const u8 CloseResult[] = "CLOSE OK";
const u8 OK[] = "OK";
const u8 Error[] = "Error";
const u8 AConnect[] = "ALREADY CONNECT";
const u8 ATCREG[] = "AT+CREG?\r";
const u8 CREGResult[] = "+CREG: 0,1";
const u8 RecServer[] = "+server:123";
const u8 Restart[] = "+CPIN: READY";


const u8 head[3] = { 0x0f,0xf0 };
const u8 tail[3] = { 0xf0,0x0f };
com Com;
CmdStatus AT_CMD;
CmdStatus CIPSTART_CMD;
CmdStatus CIPSEND_CMD;
CmdStatus CIPCLOSE_CMD;
CmdStatus CREG_CMD;
CmdStatus SendResult_CMD;

u8 read[200] = { 0 };


u8 udpConnect(const u8 *CMD)
{
	u8 num;
	u2_printf("%s%s",CMD,certain);
	delay_ms(5000);
	while(readLine(read, &Buf, Line, 200) !=0)
	{
		if(strlen((char *)read) !=0)
		{
			num = comparestr(read);
			if(num == 2)
			{
					while(strlen((char *)read) ==0)
					{
						readLine(read, &Buf, Line, 200);
					}
					num = comparestr(read);
					if(num == 2)
					{
						printf("连接成功\r\n");
					}
				return 1;
			}
		}
	}
		
		
	if(Com.isConnect == 0)
	{
		u2_printf("%s%s",UDPClose,certain);
	}
	return 0;
}

u8 udpSendMsg(signed int MaiChong)
{
	u8 num;
	u8 next = 0;
	u2_OutCmd(&CIPSEND_CMD,UDPSend);
	next = ReadResponseTimeOut(&CIPSEND_CMD,10);
	//u2_printf("脉冲数为%d",MaiChong);
	agreementSend();
	return 0;
}

u8 agreementSend(void)
{
	union Tdata Num;
	u8 num;
	u8 err;
	u8 check[12];
	static u16 ThisSendTimes = 0;
	OSSemPend(readBuf,0,&err);
		
	UartSend(USART2,(u8 *)datahead,6);		//数据头
		
	Num.u8number = waterType;	//供水类型
	UartSend(USART2,Num.str,1);
	memcpy(&check[0],Num.str,1);
		
		
	Num.u8number = switchstatus;	//继电器状态
	UartSend(USART2,Num.str,1);
	memcpy(&check[1],Num.str,1);
		
	Num.u16number = randomcode;		//随机码
	UartSend(USART2,Num.str,2);
	memcpy(&check[2],Num.str,2);
		
	Num.u32number = cardNum;			//卡号
	UartSend(USART2,Num.str,4);
	memcpy(&check[4],Num.str,4);

	Num.u16number = pulseTotal;		//脉冲次数
	UartSend(USART2,Num.str,2);
	memcpy(&check[8],Num.str,2);

		
	Num.u16number = ThisSendTimes;	//当前发送次数
	UartSend(USART2,Num.str,2);
	memcpy(&check[10],Num.str,2);
		
		
	Num.u8number = BCC_CheckOut(check,12);		//BCC校验
	UartSend(USART2,Num.str,1);
		
	UartSend(USART2,(u8 *)datatail,6);		//数据尾

	OSSemPost(readBuf);
	ThisSendTimes ++;
	return 0;
}

u8 isSign(void)
{
	u8 next = 0;
	u2_OutCmd(&AT_CMD,AT);
	next = ReadResponseTimeOut(&AT_CMD,10);
	if(next == 1)
	{
		u2_OutCmd(&CREG_CMD,ATCREG);
		next = ReadResponseTimeOut(&CREG_CMD,50);
		if(next == 1)
		{
			return 1;
		}
	}
		return 0;
}

void initCmd(void)
{
	AT_CMD.ifread = 0;
	CIPSTART_CMD.ifread = 0;
	CIPSEND_CMD.ifread = 1;
	CIPCLOSE_CMD.ifread = 0;
	CREG_CMD.ifread = 0;
	SendResult_CMD.ifread = 0;
	
	AT_CMD.CmdStatus = 0;
	CIPSTART_CMD.CmdStatus= 0;
	CIPSEND_CMD.CmdStatus = 0;
	CIPCLOSE_CMD.CmdStatus = 0;
	CREG_CMD.CmdStatus = 0;
	SendResult_CMD.CmdStatus = 0;
}

u8 ReadResponseTimeOut(CmdStatus *CMD,u16 timeout)
{
	u16 time = 0;
	while(time++ < timeout)
	{
		if(CMD->CmdStatus == 1)
		{
			return 1;
		}
		else if(CMD->CmdStatus == 2)
		{
			return 2;
		}
		delay_ms(100);
	}
	return 0;
}

void u2_OutCmd(CmdStatus *CMD,const u8 *Air202CMD)
{
	u8 err = 0;
	CMD->ifread = 1;
	CMD->CmdStatus = 0;
	OSSemPend(readBuf,0,&err);
	u2_printf("%s%s",Air202CMD,certain);
	OSSemPost(readBuf);
	
}





