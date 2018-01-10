#include "sys.h"
#include "usart2.h"
#include "stdio.h"	 	 
#include "string.h"	
#include "stdarg.h"	
#include "udp.h"
#include "agreement.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

__align(8) u8 USART2_TX_BUF[200]; 
#define USART_REC_LEN  			200  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 



/*ʹ��microLib�ķ���*/
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
 
#if EN_USART2_RX   //���ʹ���˽���
u16 USART2_RX_STA=0;       //����״̬���+����  
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.


uartBuff Buf;

 void execute2(u16 t)//���ڽ������ݺ�ִ�еĺ�������void RX_TX(void)����ִ��
 {
	 	USART_SendData(USART1, USART2_RX_BUF[t]);//�򴮿�1��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
 }

  
void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 

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

void USART2_IRQHandler(void)                	//����1�жϷ������
	{
		u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
		
		
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
			Res =USART_ReceiveData(USART2);	//��ȡ���յ�������
			//while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
			//USART_SendData(USART1, Res);//�򴮿�1��������
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
		
		 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
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
				return 0;		//û������
			}
			else
			{
				return 2;		//������������
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
	if ((waterType == 0 || waterType == 2) && num.u8number == 2)		//�������ֻ��˿�������
	{
		printf("�ֻ���:\n");
		printf("��ˮ����:%d\n", num.u8number);
		waterType = num.u8number;
		str++;

		memcpy(num.str, str, 1);
		printf("�̵���״̬:%d\n", num.u8number);
		switchstatus = num.u8number;		//���Ƽ̵�������
		str++;

		memcpy(num.str, str, 2);
		printf("�����:%d\n", num.u16number);
		randomcode = num.u16number;	
		str += 2;

		memcpy(num.str, str, 4);
		printf("����:%d\n", num.u32number);
		cardNum = num.u32number;		
		str += 4;

		memcpy(num.str, str, 4);
		printf("���:%f\n", num.Fnumber);
		money = num.Fnumber;
		str += 4;

		memcpy(num.str, str, 4);
		printf("����:%f\n", num.Fnumber);
		unitprice = num.Fnumber;
		str += 4;

		memcpy(num.str, str, 1);
		printf("1��ˮ������:%d\n", num.u8number);
		everyL = num.u8number;
		str++;

		memcpy(num.str, str, 2);
		printf("���ʹ���:%d\n", num.u16number);
		sendTimes = num.u16number;
		str += 2;

		memcpy(num.str, str, 1);
		printf("CRCУ��%d\n", num.u8number);
		CRC_checkout = num.u8number;

	}
	else if(waterType == 2 && num.u8number ==0)			//�������ֻ��˿�������
	{
		printf("�ֻ����Ѿ�����\r\n");
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
	else if (waterType == 1 && num.u8number == 1)		//ˢ����,�õ������
	{
		printf("ˢ����:\n");
		printf("��ˮ����:%d\n", num.u8number);
		waterType = num.u8number;
		str++;

		memcpy(num.str, str, 1);
		printf("�̵���״̬:%d\n", num.u8number);
		switchstatus = num.u8number;
		str++;

		memcpy(num.str, str, 2);
		printf("�����:%d\n", num.u16number);
		randomcode = num.u16number;
		str += 2;

		memcpy(num.str, str, 4);
		printf("����:%d\n", num.u32number);
		cardNum = num.u32number;
		str += 4;

		memcpy(num.str, str, 4);
		printf("���:%f\n", num.Fnumber);
		money = num.Fnumber;
		str += 4;

		memcpy(num.str, str, 4);
		printf("����:%f\n", num.Fnumber);
		unitprice = num.Fnumber;
		str += 4;

		memcpy(num.str, str, 1);
		printf("1��ˮ������:%d\n", num.u8number);
		everyL = num.u8number;
		str++;

		memcpy(num.str, str, 2);
		printf("���ʹ���:%d\n", num.u16number);
		sendTimes = num.u16number;
		str += 2;

		memcpy(num.str, str, 1);
		printf("CRCУ��%d\n", num.u8number);
		CRC_checkout = num.u8number;
	}
	else if (waterType == 0 && num.u8number == 0)
	{
		printf("���豸����\r\n");
	}
	else
	{
		printf("�������뵥Ƭ���˲�ͬ��\n");
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
						printf("���յ�����������\r\n\r\n");
						return i;
					}break;
				case 14:
					if (strcmp((char *)str, (char *)Restart) == 0)
					{
						printf("GSMģ���Ѿ�����\r\n");
						return i;
					}
				case 15:
					if (strcmp((char *)str, (char *)head) == 0)		//���������͵�����
					{
					getstr(&len, &Buf);
					printf("���ݳ���Ϊ%d\n", len);
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


