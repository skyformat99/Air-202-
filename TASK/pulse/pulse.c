#include "pulse.h"
__align(8) OS_STK pulse_TASK_STK[pulse_STK_SIZE];


void pulse_task(void *pdata)
{
	//OS_CPU_SR cpu_sr=0;
	u8 num;
	
	while(1) 
	{	
		if(CIPSTART_CMD.CmdStatus == 0)
		{
			num = isSign();
			if(num == 1)
			{
				printf("��������\r\n");
				u2_OutCmd(&CIPSTART_CMD,UDP);
				ReadResponseTimeOut(&CIPSTART_CMD,50);
			}
			else
			{
				printf("��������\r\n");
			}
		}
		
		else if(CIPSTART_CMD.CmdStatus == 1)
		{
			printf("���ڷ�������\r\n");
			udpSendMsg(times_mai);
		}
		OSTimeDlyHMSM(0, 0, 1, 0);
	}	 
}
