#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 
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
#define USART2_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define BufLen	200
			
typedef struct
{
	u16 endlocation;	//��ǰָ��λ��
	u16 startLocation;	//Buf��ʼλ��
	u16	surplus;		//Bufʣ��ĳ���
	u8 Buf[BufLen+1];	//Buf����
	u8 fillFlag;	//�ַ����Ƿ��Ѿ�����

}uartBuff;


typedef struct
{
	u8 type;		//0:û�������κ�Э��	1:TCP,2:UDP
	u8 isConnect;	//0:δ����	1:�ȴ�����	2:�Ѿ�����
	u8 isTimeout;	//0:û�г�ʱ	1:��ʱ
	u8 cmdStatus;	//0:Error	1:OK	2:cmd���յ������
}com;


//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart2_init(u32 bound);
void u2_printf(char* fmt,...);

extern u16 USART2_RX_STA;         		//����״̬���	
extern void RX2_TX(void);
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 


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


