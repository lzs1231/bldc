#include "sys.h"


u8 WWDG_CNT=0x7f;   //窗口看门狗计数器初值

 
/**********独立看门狗*************/
void IWDG_Init(uint8_t prv ,uint16_t rlv)
{    
	//使能预分频寄存器PR和重装载寄存器RLR可写    	
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	IWDG_SetPrescaler( prv );     // 设置预分频器值      7表示256倍分频   减1花费6.4ms
	IWDG_SetReload( rlv );        // 设置重装载寄存器值   
	IWDG_ReloadCounter();         // 把重装载寄存器的值放到计数器中    
	IWDG_Enable();                // 使能 IWDG   
}

void IWDG_Feed(void)              // 喂狗
{
	// 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
	// 当计数器的值减到0的时候会产生系统复位
	IWDG_ReloadCounter();
}
 
/***********窗口看门狗*************/
void Wwdg_Init(u8 tr,u8 wr,u8 fprer)
{
	RCC->APB1ENR|=1<<11;   		 //使能WWDG时钟
	WWDG_CNT=tr&WWDG_CNT;  		 //初始化计数器值
	WWDG->CFR|=fprer<<7;   		 //设置频率时基
//  WWDG->CFR|=1<<9;       		 //使能窗口看门狗中断
	WWDG->CFR&=0xff80;      	 //窗口值清零
	WWDG->CFR|=wr;          	 //设定窗口值
	WWDG->CR|=(WWDG_CNT|1<<7);   //启动看门狗设置7位计数器初值
}

void Wwdg_Feed(u8 cnt)
{
	WWDG->CR|=(cnt&0x7f);        //喂狗值
}
 
void WWDG_IRQHandler(void)
{
   u8 tr,wr;
   wr=WWDG->CFR&0x7f;
   tr=WWDG->CR&0x7f; 
// Wwdg_Feed(0x7f);
   WWDG->SR&=0x00;                       //清楚提前唤醒标志位
}

//窗口看门狗的初始化
//void WWDG_Init(u8 tr,u8 wr,u32 fprer)
//{ 
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);  //   WWDG时钟使能
//	WWDG_CNT=tr&WWDG_CNT;   //初始化WWDG_CNT.   
//	WWDG_SetPrescaler(fprer);////设置IWDG预分频值
//	WWDG_SetWindowValue(wr);//设置窗口值
//	WWDG_Enable(WWDG_CNT);	 //使能看门狗 ,	设置 counter .                  
//	WWDG_ClearFlag();//清除提前唤醒中断标志位 
//	WWDG_NVIC_Init();//初始化窗口看门狗 NVIC
//	WWDG_EnableIT(); //开启窗口看门狗中断
//}  

//看门狗的中断函数
//void WWDG_IRQHandler(void)
//{
//	WWDG_SetCounter(WWDG_CNT);	  //当禁掉此句后,窗口看门狗将产生复位
//	WWDG_ClearFlag();	  		  //清除提前唤醒中断标志位	
//} 

