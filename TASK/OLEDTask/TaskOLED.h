#ifndef __TASKOLED_H
#define __TASKOLED_H
#include "includes.h"
#include "oled.h"

//OLED����
//�����������ȼ�
#define OLED_TASK_PRIO       			4
//���������ջ��С
#define OLED_STK_SIZE  					64
//�����ջ
extern OS_STK OLED_TASK_STK[OLED_STK_SIZE];
//������
extern void OLED_task(void *pdata);
#endif
