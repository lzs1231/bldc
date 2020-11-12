
#include "ringbuf.h"





RingBuff_t ringBuff;        //����һ��ringBuff�Ļ�����

void RingBuff_Init(void)
{
   //��ʼ�������Ϣ
   ringBuff.Head = 0;
   ringBuff.Tail = 0;
   ringBuff.Lenght = 0;
}

u8 Write_RingBuff(int data)
{
	if(ringBuff.Lenght >= RINGBUFF_LEN) //�жϻ������Ƿ�����
	{
		return 0;
	}
	
	ringBuff.Ring_Buff[ringBuff.Tail]=data;
	ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
	ringBuff.Lenght++;
	return 1;
}

u8 Read_RingBuff(int *rData)
{
	if(ringBuff.Lenght == 0)//�жϷǿ�
	{
		return 0;
	}
	
	*rData = ringBuff.Ring_Buff[ringBuff.Head];//�Ƚ��ȳ�FIFO���ӻ�����ͷ��

	ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
	ringBuff.Lenght--;      //����һ���ֽ�֮�����ݳ���-1
	return 1;
}



