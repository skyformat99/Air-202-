#ifndef __WATERAGREEMENT_H
#define __WATERAGREEMENT_H	

#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "sys.h" 

typedef struct
{

	//协议头
	u8 head[6];	

	//设备ID
	u8 deviceID[8];

	//供水类型 0：下线	1：刷卡	2：手机
	u8 payType;

	//继电器状态	0：关闭		1：开启
	u8 status;

	//单片机随机码
	u8 nonceDevice[4];

	//服务器随机码
	u8 nonceServer[4];

	//卡号
	u8 cardNo[8];

	//脉冲次数
	u8 counter[4];

	//发送序号
	u8 packIndex[8];

	//余额(分)
	u8 balance[4];
	
	//单价(分/升)
	u8 price[4];

	//计量单位(脉冲信号/升)
	u8 signalPerlitexs[4];

	//拒绝原因
	u8 refuseReson;
	
	//md5签名(去除协议头和尾及当前签名字段，对[其他书+key]进行md5)
	u8 sign[16];
	
	//协议尾
	u8 tail[4];
}waterPack;

#endif
