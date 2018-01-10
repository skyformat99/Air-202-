#ifndef __TASKOLED_H
#define __TASKOLED_H
#include "includes.h"
#include "oled.h"

//OLED任务
//设置任务优先级
#define OLED_TASK_PRIO       			4
//设置任务堆栈大小
#define OLED_STK_SIZE  					64
//任务堆栈
extern OS_STK OLED_TASK_STK[OLED_STK_SIZE];
//任务函数
extern void OLED_task(void *pdata);
#endif
