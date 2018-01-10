#include "TaskOLED.h"
OS_STK OLED_TASK_STK[OLED_STK_SIZE];
void OLED_task(void *pdata)
{
	static unsigned char signal_num;
	signal_num=6;
	while(1) 
	{
		OSTimeDlyHMSM(0, 0, 0, 1000);
		signal_num--;
		if(signal_num==0)	signal_num=5;
		showsignal(0,0,signal_num);
		OLED_ShowCHinese(32,0,0);
		OLED_ShowCHinese(48,0,1);
		OLED_ShowCHinese(64,0,2);
		OLED_ShowCHinese(80,0,3);
	}	 
}
