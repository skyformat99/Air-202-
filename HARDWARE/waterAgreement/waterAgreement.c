#include "waterAgreement.h"

//将协议头打包
void PackHead(waterPack *Pack,u8 *Head,u8 Len)
{
	memcpy(Head,Pack->head,Len);
}

