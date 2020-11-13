#include "bsp.h"
#include "ctrl.h"
#include "adc.h"
#include "usart.h"
#include "pwrchk.h"
#include "pid_control.h"
#include "wdg.h"
#include "doublebuf.h"

void RCC_Configuration(void); 
void Init_GPIO(void);            //初始化IO口
void Interrupt_Config(void);     //中断优先级配置


void BSP_Init(void) 
{
	RCC_Configuration(); 
	delay_init();	             //延时函数初始化	
	
	Init_GPIO();
	BLDC_GPIOConfig();
	BLDC_TIM1Config();
	BLDC_TIM3Config();
	BLDC_TIM4Config();
	PIDInit();               //PID参数初始化
	
#ifdef SPEED_PPRINTF
//	create_double_buf(p_double_buf,100);
	uart1_init(115200);
#endif
#ifdef  USE_MAX_485
	uart2_init(115200);
#elif  defined USE_MAX_232
	uart3_init(115200);
#endif
	Adc_Init();  	
	delay_ms(1000);       //延时等待串口屏初始化完毕,必须等待300ms 	
}

void RCC_Configuration(void)  
{  
	//----------使用外部RC晶振-----------  
	RCC_DeInit();                    //初始化为缺省值  
	RCC_HSEConfig(RCC_HSE_ON);       //使能外部的高速时钟   
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET); //等待外部高速时钟使能就绪  

	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);   //Enable Prefetch Buffer  //FLASH时序延迟几个周期，等待总线同步操作。所有程序中必须的
	FLASH_SetLatency(FLASH_Latency_2);          //Flash 2 wait state  

	RCC_HCLKConfig(RCC_SYSCLK_Div1);            //HCLK = SYSCLK  设置AHB时钟为系统时钟
	RCC_PCLK2Config(RCC_HCLK_Div1);             //PCLK2 =  HCLK  APB2高速APB时钟（PCLK2）源自AHB时钟（HCLK）      72M
	RCC_PCLK1Config(RCC_HCLK_Div2);             //PCLK1 = HCLK/2 APB1低速APB时钟（PCLK1）源自AHB时钟/2（HCLK/2）  36M
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); //PLLCLK = 8MHZ * 9 =72MHZ     外部晶振8MHz*9倍频
	RCC_PLLCmd(ENABLE);                         //打开PLL锁相环 PLLCLK  

	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //等待PLL锁相环工作  
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //选择PLL作为系统时钟
	while(RCC_GetSYSCLKSource()!=0x08);         //Wait till PLL is used as system clock source  
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO,ENABLE);    //使能外设时钟 
}  

void Init_GPIO(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;              //定义一个结构体
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //引脚模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //输出速度

	GPIO_InitStructure.GPIO_Pin = LEDL_PIN;           //引脚号
	GPIO_Init(LEDL_GPIO,&GPIO_InitStructure);         //根据定义设置引脚
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         //引脚号，控制继电器
	GPIO_Init(GPIOD,&GPIO_InitStructure);             //根据定义设置引脚
	GPIO_ResetBits(GPIOD,GPIO_InitStructure.GPIO_Pin);//该引脚置为低电平
	
	/**************定义4个IO口，用于外部输入**************/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //引脚模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;         //引脚号
	GPIO_Init(GPIOC,&GPIO_InitStructure);             //根据定义设置引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;         //引脚号
	GPIO_Init(GPIOC,&GPIO_InitStructure);             //根据定义设置引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;         //引脚号
	GPIO_Init(GPIOB,&GPIO_InitStructure);             //根据定义设置引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         //引脚号
	GPIO_Init(GPIOB,&GPIO_InitStructure);             //根据定义设置引脚

	
//	GPIO_ResetBits(LED_GPIO,GPIO_InitStr.GPIO_Pin);//该引脚置为低电平
//  GPIO_SetBits(LED_GPIO,GPIO_InitStr.GPIO_Pin);  //该引脚置为高电平
//  GPIO_Write(GPIOA,0xff);
//  GPIO_WriteBit(LED_GPIO,GPIO_InitStr.GPIO_Pin,BitAction(1))  //该引脚置为高电平
}

void Interrupt_Config(void)   //NVIC 中断优先级配置 
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //2表示2位抢占优先级0-3    2:2位抢占0-3，2位响应0-3

	//外部中断向量配置
//	NVIC_InitStructure.NVIC_IRQChannel= EXTI0_IRQn;          //选择中断通道0
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0; //抢占式中断优先级设置为0
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0;        //响应式中断优先级设置为0
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;            //使能中断
//	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		    //开TIM3中断   ，捕获中断+定时器计时中断，测量换相速度
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;      //开TIM1捕获中断    ，测量PWM中点电流  100us一次中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel =	TIM1_BRK_IRQn; //开TIM1触发中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable DMA channel1 IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;      //adc1的DMA通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;      //开TIM4更新中断，每隔1ms中断一次
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;      //窗口看门狗中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

#ifdef USE_MAX_485 
	//Usart2 中断向量配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;       			//中断向量表
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 			//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		    		//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                        		//根据指定的参数初始化VIC寄存器
	
//	/* Enable DMA channel6 IRQ Channel */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;      //串口2接收的DMA通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	/* Enable DMA channel7 IRQ Channel */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;      //串口2发送的DMA通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
#elif	defined USE_MAX_232
	//Usart3 中断向量配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;       			//中断向量表
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 			//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		    		//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                        		//根据指定的参数初始化VIC寄存器
	
//	/* Enable DMA channel6 IRQ Channel */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;      //串口2接收的DMA通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	/* Enable DMA channel7 IRQ Channel */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;      //串口2发送的DMA通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
#endif
}






