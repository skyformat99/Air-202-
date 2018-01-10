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
 ALIENTEK战舰STM32开发板实验51
 UCOSII实验1-任务调度 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
//DHT11任务
//设置任务优先级
#define HC_SR501_TASK_PRIO       			7 
//设置任务堆栈大小
#define HC_SR501_STK_SIZE  		    		64
//任务堆栈	
OS_STK HC_SR501_TASK_STK[HC_SR501_STK_SIZE];
//任务函数
void HC_SR501_task(void *pdata);


//DHT11任务
//设置任务优先级
#define DHT11_TASK_PRIO       			8
//设置任务堆栈大小
#define DHT11_STK_SIZE  					64
//任务堆栈
OS_STK DHT11_TASK_STK[DHT11_STK_SIZE];
//任务函数
void DHT11_task(void *pdata);

//DRIVE任务
//设置任务优先级
#define DRIVE_TASK_PRIO       			6
//设置任务堆栈大小
#define DRIVE_STK_SIZE  					64
//任务堆栈
OS_STK DRIVE_TASK_STK[DRIVE_STK_SIZE];
//任务函数
void DRIVE_task(void *pdata);


//串口任务
//设置任务优先级
#define UART_SEND_TASK_PRIO       			5
//设置任务堆栈大小
#define UART_SEND_STK_SIZE  					64
//任务堆栈
OS_STK UART_SEND_TASK_STK[DRIVE_STK_SIZE];
//任务函数
void UART_SEND_task(void *pdata);






//powerLED任务
//设置任务优先级
#define Key_TASK_PRIO       			3
//设置任务堆栈大小
#define Key_STK_SIZE  					64
//任务堆栈
OS_STK Key_TASK_STK[Key_STK_SIZE];
//任务函数
void Key_task(void *pdata);



OS_EVENT * q_msg;			//消息队列
void * MsgGrp[100];			//消息队列存储地址,最大支持256个消息
char SendBuff[20] ;//DMA串口发送的数据

 int main(void)
 {	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	 delay_init();	    //延时函数初始化	  
	uart_init(115200);
	 uart2_init(115200);
	//MYDMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)SendBuff,20);//DMA串口发送数据的配置
	readBuf=OSSemCreate(1);
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	  	 
}
	  
//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	EXTIX_Init();							//脉冲引脚初始化
	OLED_Init();
	OLED_Clear();
	initCom(&Com);
	initUartBuff(&Buf);
  OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
	OSTaskCreate(pulse_task,(void *)0,(OS_STK*)&pulse_TASK_STK[pulse_STK_SIZE-1],pulse_TASK_PRIO);	
	//OSTaskCreate(agreement_task,(void *)0,(OS_STK*)&agreement_TASK_STK[agreement_STK_SIZE-1],agreement_TASK_PRIO);	
	OSTaskCreate(readbuf_task,(void *)0,(OS_STK*)&readbuf_TASK_STK[readbuf_STK_SIZE-1],readbuf_TASK_PRIO);	
	OSTaskCreate(OLED_task,(void *)0,(OS_STK*)&OLED_TASK_STK[OLED_STK_SIZE-1],OLED_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	printf("初始化完成\r\n");
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}




//后面任务没有用的

//人体红外任务
void HC_SR501_task(void *pdata)
{	 	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 1,0);
	};
}

//DHT11任务
void DHT11_task(void *pdata)
{
	while(1)
	{
	OSTimeDlyHMSM(0, 0, 0, 100);
	};
		
}

//驱动任务，初始化后挂起
void DRIVE_task(void *pdata)
{
	while(1)
	{

	 OSTaskSuspend(DRIVE_TASK_PRIO);
	};
		
}

//串口任务，初始化后挂起，串口发送数据
void UART_SEND_task(void *pdata)
{
	u8 err;
	u8* p;
	while(1)
	
		{
			p=OSQPend(q_msg,0,&err);
			sprintf(SendBuff,"%s",p);
			if(DMA_GetFlagStatus(DMA1_FLAG_TC4)!=RESET)	//判断通道4传输完成
				{
					DMA_ClearFlag(DMA1_FLAG_TC4);//清除通道4传输完成标志
					
		    }
				else if(err!=OS_ERR_NONE)
				{
				}
				else
				{
					USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //使能串口1的DMA发送      
					MYDMA_Enable(DMA1_Channel4);//开始一次DMA传输！
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






