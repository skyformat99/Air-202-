#include "sys.h" 	
#include "delay.h"	
#include "usart.h"
#include "dma.h"
#include "malloc.h"
#include "includes.h"

#include "timer.h"
#include "pulse.h"
#include "agreement.h"
#include "Taskreadbuf.h"
#include "TaskOLED.h"
#include "sem.h"

 

/************************************************
 ALIENTEKս��STM32������ʵ��51
 UCOSIIʵ��1-������� ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 			   
//DHT11����
//�����������ȼ�
#define HC_SR501_TASK_PRIO       			7 
//���������ջ��С
#define HC_SR501_STK_SIZE  		    		64
//�����ջ	
OS_STK HC_SR501_TASK_STK[HC_SR501_STK_SIZE];
//������
void HC_SR501_task(void *pdata);


//DHT11����
//�����������ȼ�
#define DHT11_TASK_PRIO       			8
//���������ջ��С
#define DHT11_STK_SIZE  					64
//�����ջ
OS_STK DHT11_TASK_STK[DHT11_STK_SIZE];
//������
void DHT11_task(void *pdata);

//DRIVE����
//�����������ȼ�
#define DRIVE_TASK_PRIO       			6
//���������ջ��С
#define DRIVE_STK_SIZE  					64
//�����ջ
OS_STK DRIVE_TASK_STK[DRIVE_STK_SIZE];
//������
void DRIVE_task(void *pdata);


//��������
//�����������ȼ�
#define UART_SEND_TASK_PRIO       			5
//���������ջ��С
#define UART_SEND_STK_SIZE  					64
//�����ջ
OS_STK UART_SEND_TASK_STK[DRIVE_STK_SIZE];
//������
void UART_SEND_task(void *pdata);






//powerLED����
//�����������ȼ�
#define Key_TASK_PRIO       			3
//���������ջ��С
#define Key_STK_SIZE  					64
//�����ջ
OS_STK Key_TASK_STK[Key_STK_SIZE];
//������
void Key_task(void *pdata);



OS_EVENT * q_msg;			//��Ϣ����
void * MsgGrp[100];			//��Ϣ���д洢��ַ,���֧��256����Ϣ
char SendBuff[20] ;//DMA���ڷ��͵�����

 int main(void)
 {	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	 delay_init();	    //��ʱ������ʼ��	  
	uart_init(115200);
	 uart2_init(115200);
	//MYDMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)SendBuff,20);//DMA���ڷ������ݵ�����
	readBuf=OSSemCreate(1);
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	  	 
}
	  
//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	EXTIX_Init();							//�������ų�ʼ��
	OLED_Init();
	OLED_Clear();
	initCom(&Com);
	initUartBuff(&Buf);
  OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
	OSTaskCreate(pulse_task,(void *)0,(OS_STK*)&pulse_TASK_STK[pulse_STK_SIZE-1],pulse_TASK_PRIO);	
	//OSTaskCreate(agreement_task,(void *)0,(OS_STK*)&agreement_TASK_STK[agreement_STK_SIZE-1],agreement_TASK_PRIO);	
	OSTaskCreate(readbuf_task,(void *)0,(OS_STK*)&readbuf_TASK_STK[readbuf_STK_SIZE-1],readbuf_TASK_PRIO);	
	OSTaskCreate(OLED_task,(void *)0,(OS_STK*)&OLED_TASK_STK[OLED_STK_SIZE-1],OLED_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	printf("��ʼ�����\r\n");
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}




//��������û���õ�

//�����������
void HC_SR501_task(void *pdata)
{	 	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 1,0);
	};
}

//DHT11����
void DHT11_task(void *pdata)
{
	while(1)
	{
	OSTimeDlyHMSM(0, 0, 0, 100);
	};
		
}

//�������񣬳�ʼ�������
void DRIVE_task(void *pdata)
{
	while(1)
	{

	 OSTaskSuspend(DRIVE_TASK_PRIO);
	};
		
}

//�������񣬳�ʼ������𣬴��ڷ�������
void UART_SEND_task(void *pdata)
{
	u8 err;
	u8* p;
	while(1)
	
		{
			p=OSQPend(q_msg,0,&err);
			sprintf(SendBuff,"%s",p);
			if(DMA_GetFlagStatus(DMA1_FLAG_TC4)!=RESET)	//�ж�ͨ��4�������
				{
					DMA_ClearFlag(DMA1_FLAG_TC4);//���ͨ��4������ɱ�־
					
		    }
				else if(err!=OS_ERR_NONE)
				{
				}
				else
				{
					USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ���1��DMA����      
					MYDMA_Enable(DMA1_Channel4);//��ʼһ��DMA���䣡
				}
	};
		
}






void Key_task(void *pdata)
{
   while(1) 
	{		
		OSTimeDlyHMSM(0, 0, 0, 50);
	}	 
}






