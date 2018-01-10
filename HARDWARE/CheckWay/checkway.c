#include "checkway.h"

u8 BCC_CheckOut(const u8 *str,u16 len)
{
	u8 checksum = 0;
	for(;len>0;len--)
	{
		checksum ^= *str++;
	}
	return checksum;
}
