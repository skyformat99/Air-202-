#ifndef __PULSE_H
#define __PULSE_H
#include "includes.h"
#include "timer.h"
#include "usart2.h"
#include "UDP.h"
#include "agreement.h"
//pulse����
//�����������ȼ�
#define pulse_TASK_PRIO       			8
//���������ջ��С
#define pulse_STK_SIZE  					256
//�����ջ
extern OS_STK pulse_TASK_STK[pulse_STK_SIZE];
//������
extern void pulse_task(void *pdata);
#endif
