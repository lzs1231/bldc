#include "sys.h"


u8 WWDG_CNT=0x7f;   //���ڿ��Ź���������ֵ

 
/**********�������Ź�*************/
void IWDG_Init(uint8_t prv ,uint16_t rlv)
{    
	//ʹ��Ԥ��Ƶ�Ĵ���PR����װ�ؼĴ���RLR��д    	
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	IWDG_SetPrescaler( prv );     // ����Ԥ��Ƶ��ֵ      7��ʾ256����Ƶ   ��1����6.4ms
	IWDG_SetReload( rlv );        // ������װ�ؼĴ���ֵ   
	IWDG_ReloadCounter();         // ����װ�ؼĴ�����ֵ�ŵ���������    
	IWDG_Enable();                // ʹ�� IWDG   
}

void IWDG_Feed(void)              // ι��
{
	// ����װ�ؼĴ�����ֵ�ŵ��������У�ι������ֹIWDG��λ
	// ����������ֵ����0��ʱ������ϵͳ��λ
	IWDG_ReloadCounter();
}
 
/***********���ڿ��Ź�*************/
void Wwdg_Init(u8 tr,u8 wr,u8 fprer)
{
	RCC->APB1ENR|=1<<11;   		 //ʹ��WWDGʱ��
	WWDG_CNT=tr&WWDG_CNT;  		 //��ʼ��������ֵ
	WWDG->CFR|=fprer<<7;   		 //����Ƶ��ʱ��
//  WWDG->CFR|=1<<9;       		 //ʹ�ܴ��ڿ��Ź��ж�
	WWDG->CFR&=0xff80;      	 //����ֵ����
	WWDG->CFR|=wr;          	 //�趨����ֵ
	WWDG->CR|=(WWDG_CNT|1<<7);   //�������Ź�����7λ��������ֵ
}

void Wwdg_Feed(u8 cnt)
{
	WWDG->CR|=(cnt&0x7f);        //ι��ֵ
}
 
void WWDG_IRQHandler(void)
{
   u8 tr,wr;
   wr=WWDG->CFR&0x7f;
   tr=WWDG->CR&0x7f; 
// Wwdg_Feed(0x7f);
   WWDG->SR&=0x00;                       //�����ǰ���ѱ�־λ
}

//���ڿ��Ź��ĳ�ʼ��
//void WWDG_Init(u8 tr,u8 wr,u32 fprer)
//{ 
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);  //   WWDGʱ��ʹ��
//	WWDG_CNT=tr&WWDG_CNT;   //��ʼ��WWDG_CNT.   
//	WWDG_SetPrescaler(fprer);////����IWDGԤ��Ƶֵ
//	WWDG_SetWindowValue(wr);//���ô���ֵ
//	WWDG_Enable(WWDG_CNT);	 //ʹ�ܿ��Ź� ,	���� counter .                  
//	WWDG_ClearFlag();//�����ǰ�����жϱ�־λ 
//	WWDG_NVIC_Init();//��ʼ�����ڿ��Ź� NVIC
//	WWDG_EnableIT(); //�������ڿ��Ź��ж�
//}  

//���Ź����жϺ���
//void WWDG_IRQHandler(void)
//{
//	WWDG_SetCounter(WWDG_CNT);	  //�������˾��,���ڿ��Ź���������λ
//	WWDG_ClearFlag();	  		  //�����ǰ�����жϱ�־λ	
//} 

