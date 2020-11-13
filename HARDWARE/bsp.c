#include "bsp.h"
#include "ctrl.h"
#include "adc.h"
#include "usart.h"
#include "pwrchk.h"
#include "pid_control.h"
#include "wdg.h"
#include "doublebuf.h"

void RCC_Configuration(void); 
void Init_GPIO(void);            //��ʼ��IO��
void Interrupt_Config(void);     //�ж����ȼ�����


void BSP_Init(void) 
{
	RCC_Configuration(); 
	delay_init();	             //��ʱ������ʼ��	
	
	Init_GPIO();
	BLDC_GPIOConfig();
	BLDC_TIM1Config();
	BLDC_TIM3Config();
	BLDC_TIM4Config();
	PIDInit();               //PID������ʼ��
	
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
	delay_ms(1000);       //��ʱ�ȴ���������ʼ�����,����ȴ�300ms 	
}

void RCC_Configuration(void)  
{  
	//----------ʹ���ⲿRC����-----------  
	RCC_DeInit();                    //��ʼ��Ϊȱʡֵ  
	RCC_HSEConfig(RCC_HSE_ON);       //ʹ���ⲿ�ĸ���ʱ��   
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET); //�ȴ��ⲿ����ʱ��ʹ�ܾ���  

	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);   //Enable Prefetch Buffer  //FLASHʱ���ӳټ������ڣ��ȴ�����ͬ�����������г����б����
	FLASH_SetLatency(FLASH_Latency_2);          //Flash 2 wait state  

	RCC_HCLKConfig(RCC_SYSCLK_Div1);            //HCLK = SYSCLK  ����AHBʱ��Ϊϵͳʱ��
	RCC_PCLK2Config(RCC_HCLK_Div1);             //PCLK2 =  HCLK  APB2����APBʱ�ӣ�PCLK2��Դ��AHBʱ�ӣ�HCLK��      72M
	RCC_PCLK1Config(RCC_HCLK_Div2);             //PCLK1 = HCLK/2 APB1����APBʱ�ӣ�PCLK1��Դ��AHBʱ��/2��HCLK/2��  36M
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); //PLLCLK = 8MHZ * 9 =72MHZ     �ⲿ����8MHz*9��Ƶ
	RCC_PLLCmd(ENABLE);                         //��PLL���໷ PLLCLK  

	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //�ȴ�PLL���໷����  
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //ѡ��PLL��Ϊϵͳʱ��
	while(RCC_GetSYSCLKSource()!=0x08);         //Wait till PLL is used as system clock source  
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO,ENABLE);    //ʹ������ʱ�� 
}  

void Init_GPIO(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;              //����һ���ṹ��
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����ٶ�

	GPIO_InitStructure.GPIO_Pin = LEDL_PIN;           //���ź�
	GPIO_Init(LEDL_GPIO,&GPIO_InitStructure);         //���ݶ�����������
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         //���źţ����Ƽ̵���
	GPIO_Init(GPIOD,&GPIO_InitStructure);             //���ݶ�����������
	GPIO_ResetBits(GPIOD,GPIO_InitStructure.GPIO_Pin);//��������Ϊ�͵�ƽ
	
	/**************����4��IO�ڣ������ⲿ����**************/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;         //���ź�
	GPIO_Init(GPIOC,&GPIO_InitStructure);             //���ݶ�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;         //���ź�
	GPIO_Init(GPIOC,&GPIO_InitStructure);             //���ݶ�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;         //���ź�
	GPIO_Init(GPIOB,&GPIO_InitStructure);             //���ݶ�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         //���ź�
	GPIO_Init(GPIOB,&GPIO_InitStructure);             //���ݶ�����������

	
//	GPIO_ResetBits(LED_GPIO,GPIO_InitStr.GPIO_Pin);//��������Ϊ�͵�ƽ
//  GPIO_SetBits(LED_GPIO,GPIO_InitStr.GPIO_Pin);  //��������Ϊ�ߵ�ƽ
//  GPIO_Write(GPIOA,0xff);
//  GPIO_WriteBit(LED_GPIO,GPIO_InitStr.GPIO_Pin,BitAction(1))  //��������Ϊ�ߵ�ƽ
}

void Interrupt_Config(void)   //NVIC �ж����ȼ����� 
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //2��ʾ2λ��ռ���ȼ�0-3    2:2λ��ռ0-3��2λ��Ӧ0-3

	//�ⲿ�ж���������
//	NVIC_InitStructure.NVIC_IRQChannel= EXTI0_IRQn;          //ѡ���ж�ͨ��0
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0; //��ռʽ�ж����ȼ�����Ϊ0
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0;        //��Ӧʽ�ж����ȼ�����Ϊ0
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;            //ʹ���ж�
//	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		    //��TIM3�ж�   �������ж�+��ʱ����ʱ�жϣ����������ٶ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;      //��TIM1�����ж�    ������PWM�е����  100usһ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel =	TIM1_BRK_IRQn; //��TIM1�����ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable DMA channel1 IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;      //adc1��DMAͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;      //��TIM4�����жϣ�ÿ��1ms�ж�һ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;      //���ڿ��Ź��ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

#ifdef USE_MAX_485 
	//Usart2 �ж���������
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;       			//�ж�������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 			//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		    		//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        		//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
//	/* Enable DMA channel6 IRQ Channel */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;      //����2���յ�DMAͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	/* Enable DMA channel7 IRQ Channel */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;      //����2���͵�DMAͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
#elif	defined USE_MAX_232
	//Usart3 �ж���������
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;       			//�ж�������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 			//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		    		//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        		//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
//	/* Enable DMA channel6 IRQ Channel */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;      //����2���յ�DMAͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	/* Enable DMA channel7 IRQ Channel */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;      //����2���͵�DMAͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
#endif
}






