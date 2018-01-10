#include "agreement.h"
__align(8) OS_STK agreement_TASK_STK[agreement_STK_SIZE];

const u8 datahead[] = {0x0D,0x0A,0x0F,0xF0,0x0D,0x0A};
const u8 datatail[] = {0x0D,0x0A,0xF0,0x0F,0x0D,0x0A};
const u8 agreement[] = { 0x0D,0x0A,0x0F,0xF0,0x0D,0x0A,0x14,0x02,0x00,0x12,0x34,0x16,0x78,0x90,0x1b,0x33,0x33,0x33,0x41,0x33,0x33,0x33,0x41,0x01,0x02,0x11,0x12,0x0D,0x0A,0xF0,0x0F,0x0D,0x0A };

u8 waterType = 0;
u8 switchstatus = 0;
u16 randomcode = 0;
u32 cardNum = 0;
float money = 0;
float unitprice = 0;
u16 pulseTotal = 0;
u8 everyL = 0;
u16 sendTimes = 0;
u8 CRC_checkout = 0;

u8 RecTail[2];

/*******************************************************************
���ܣ���Ƭ������Э����������
˵����ʹ�û����ź������漰ȫ�ֱ�������Ҫ��ֹ�����޸�ȫ�ֱ���ͬʱ��ȡ���������
*******************************************************************/
void agreement_task(void *pdata)
{
	union Tdata Num;
	u8 num;
	u8 err;
	u8 check[12];
	u16 ThisSendTimes = 0;
	while(1)
	{
		OSSemPend(readBuf,0,&err);
		
		UartSend(USART1,(u8 *)datahead,6);		//����ͷ
		
		Num.u8number = waterType;	//��ˮ����
		UartSend(USART1,Num.str,1);
		memcpy(&check[0],Num.str,1);
		
		
		Num.u8number = switchstatus;	//�̵���״̬
		UartSend(USART1,Num.str,1);
		memcpy(&check[1],Num.str,1);
		
		Num.u16number = randomcode;		//�����
		UartSend(USART1,Num.str,2);
		memcpy(&check[2],Num.str,2);
		
		Num.u32number = cardNum;			//����
		UartSend(USART1,Num.str,4);
		memcpy(&check[4],Num.str,4);

		Num.u16number = pulseTotal;		//�������
		UartSend(USART1,Num.str,2);
		memcpy(&check[8],Num.str,2);

		
		Num.u16number = ThisSendTimes;	//��ǰ���ʹ���
		UartSend(USART1,Num.str,2);
		memcpy(&check[10],Num.str,2);
		
		
		Num.u8number = BCC_CheckOut(check,12);		//BCCУ��
		UartSend(USART1,Num.str,1);
		
		UartSend(USART1,(u8 *)datatail,6);		//����β

		OSSemPost(readBuf);
		ThisSendTimes ++;
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}
