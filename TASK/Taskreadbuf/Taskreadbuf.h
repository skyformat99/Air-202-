#ifndef __TASKREADBUF_H
#define __TASKREADBUF_H
#include "includes.h"
#include "usart2.h"
#include "udp.h"
#include "sem.h"
//pulse����
//�����������ȼ�
#define readbuf_TASK_PRIO       			9
//���������ջ��С
#define readbuf_STK_SIZE  					256
//�����ջ
extern OS_STK readbuf_TASK_STK[readbuf_STK_SIZE];

extern void readbuf_task(void *pdata);
#endif
