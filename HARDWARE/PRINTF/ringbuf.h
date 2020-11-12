#ifndef  __RINGBUF_H__
#define  __RINGBUF_H__

#include "sys.h"
#define RINGBUFF_LEN 240

typedef struct
{
	u16 Head;
	u16 Tail;
	u16 Lenght;    //有效数据长度
	int Ring_Buff[RINGBUFF_LEN];    //环形缓冲区长度
}RingBuff_t;

extern RingBuff_t ringBuff;        //创建一个ringBuff的缓冲区


void RingBuff_Init(void);

u8 Write_RingBuff(int data);

u8 Read_RingBuff(int *rData);





#endif

