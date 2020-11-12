#ifndef  __RINGBUF_H__
#define  __RINGBUF_H__

#include "sys.h"
#define RINGBUFF_LEN 240

typedef struct
{
	u16 Head;
	u16 Tail;
	u16 Lenght;    //��Ч���ݳ���
	int Ring_Buff[RINGBUFF_LEN];    //���λ���������
}RingBuff_t;

extern RingBuff_t ringBuff;        //����һ��ringBuff�Ļ�����


void RingBuff_Init(void);

u8 Write_RingBuff(int data);

u8 Read_RingBuff(int *rData);





#endif

