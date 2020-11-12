#include "sys.h"
#include "stmflash.h"
#include "pwrchk.h"



//使用PC0作为外部中断进行掉电检测
void EXTI_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0; 				//选择引脚0
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING; 	//选择输入模式为浮空输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 		//输出频率最大50MHz
	GPIO_Init(GPIOC,&GPIO_InitStructure); 					//设置PC.0
}

//外部中断配置
void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	EXTI_ClearITPendingBit(EXTI_Line0);    //清空中断标志

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0);  	//选择中断管脚PC.0

	EXTI_InitStructure.EXTI_Line= EXTI_Line0; 						//选择中断线路0
	EXTI_InitStructure.EXTI_Mode= EXTI_Mode_Interrupt; 				//设置为中断请求，非事件请求
	EXTI_InitStructure.EXTI_Trigger= EXTI_Trigger_Rising; 	        //设置中断触发方式为上升沿触发，表示系统掉电，在中断中保存参数
	EXTI_InitStructure.EXTI_LineCmd=ENABLE; 						//外部中断使能
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line0)!=RESET)//判断是否置位
	{
		//Dosomething();执行中断事件，保存掉电数据
		GPIO_SetBits(GPIOD,GPIO_Pin_6);			//点亮LED灯
		
		SavePara();

		EXTI_ClearFlag(EXTI_Line0);			//清零LINE上标志位
	}
	
}

//void EXTI0_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line0)!=RESET)  //判断是否置位      先判断中断是否使能，在判断中断标志
//	{
//		Dosomething();                        //执行中断事件
//		EXTI_ClearITPendingBit(EXTI_Line0)；  //清空中断标志位，防止持续进入中断
//	}
//}









