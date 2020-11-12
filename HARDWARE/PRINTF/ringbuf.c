
#include "ringbuf.h"





RingBuff_t ringBuff;        //创建一个ringBuff的缓冲区

void RingBuff_Init(void)
{
   //初始化相关信息
   ringBuff.Head = 0;
   ringBuff.Tail = 0;
   ringBuff.Lenght = 0;
}

u8 Write_RingBuff(int data)
{
	if(ringBuff.Lenght >= RINGBUFF_LEN) //判断缓冲区是否已满
	{
		return 0;
	}
	
	ringBuff.Ring_Buff[ringBuff.Tail]=data;
	ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//防止越界非法访问
	ringBuff.Lenght++;
	return 1;
}

u8 Read_RingBuff(int *rData)
{
	if(ringBuff.Lenght == 0)//判断非空
	{
		return 0;
	}
	
	*rData = ringBuff.Ring_Buff[ringBuff.Head];//先进先出FIFO，从缓冲区头出

	ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//防止越界非法访问
	ringBuff.Lenght--;      //读出一个字节之后，数据长度-1
	return 1;
}



