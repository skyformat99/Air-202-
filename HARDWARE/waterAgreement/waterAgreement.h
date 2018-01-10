#ifndef __WATERAGREEMENT_H
#define __WATERAGREEMENT_H	

#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "sys.h" 

typedef struct
{

	//Э��ͷ
	u8 head[6];	

	//�豸ID
	u8 deviceID[8];

	//��ˮ���� 0������	1��ˢ��	2���ֻ�
	u8 payType;

	//�̵���״̬	0���ر�		1������
	u8 status;

	//��Ƭ�������
	u8 nonceDevice[4];

	//�����������
	u8 nonceServer[4];

	//����
	u8 cardNo[8];

	//�������
	u8 counter[4];

	//�������
	u8 packIndex[8];

	//���(��)
	u8 balance[4];
	
	//����(��/��)
	u8 price[4];

	//������λ(�����ź�/��)
	u8 signalPerlitexs[4];

	//�ܾ�ԭ��
	u8 refuseReson;
	
	//md5ǩ��(ȥ��Э��ͷ��β����ǰǩ���ֶΣ���[������+key]����md5)
	u8 sign[16];
	
	//Э��β
	u8 tail[4];
}waterPack;

#endif
