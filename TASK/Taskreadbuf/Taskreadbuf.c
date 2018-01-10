#include "Taskreadbuf.h"
OS_STK readbuf_TASK_STK[readbuf_STK_SIZE];
void readbuf_task(void *pdata)
{
	u8 err;
	while(1)
	{
		OSSemPend(readBuf,0,&err);
		while (readLine(read, &Buf, Line, 200) != 0)
		{
			comparestr(read);
			printf("Ω” ’ƒ⁄»›:%s\r\n",read);
		}
		OSSemPost(readBuf);
		OSTimeDlyHMSM(0, 0, 0, 100);
	}
}
