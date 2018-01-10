#ifndef __PULSE_H
#define __PULSE_H
#include "includes.h"
#include "timer.h"
#include "usart2.h"
#include "UDP.h"
#include "agreement.h"
//pulse任务
//设置任务优先级
#define pulse_TASK_PRIO       			8
//设置任务堆栈大小
#define pulse_STK_SIZE  					256
//任务堆栈
extern OS_STK pulse_TASK_STK[pulse_STK_SIZE];
//任务函数
extern void pulse_task(void *pdata);
#endif
