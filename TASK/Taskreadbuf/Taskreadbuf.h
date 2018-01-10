#ifndef __TASKREADBUF_H
#define __TASKREADBUF_H
#include "includes.h"
#include "usart2.h"
#include "udp.h"
#include "sem.h"
//pulse任务
//设置任务优先级
#define readbuf_TASK_PRIO       			9
//设置任务堆栈大小
#define readbuf_STK_SIZE  					256
//任务堆栈
extern OS_STK readbuf_TASK_STK[readbuf_STK_SIZE];

extern void readbuf_task(void *pdata);
#endif
