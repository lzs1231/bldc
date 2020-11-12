/************************************版权申明********************************************
**                             广州大彩光电科技有限公司
**                             http://www.gz-dc.com
**-----------------------------------文件信息--------------------------------------------
** 文件名称:   hmi_user_uart.c
** 修改时间:   2018-05-18
** 文件说明:   用户MCU串口驱动函数库
** 技术支持：  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------
使用必读
hmi_user_uart.c中的串口发送接收函数共3个函数：串口初始化Uartinti()、发送1个字节SendChar()、
发送字符串SendStrings().若移植到其他平台，需要修改底层寄
存器设置,但禁止修改函数名称，否则无法与HMI驱动库(hmi_driver.c)匹配。
--------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------
1. 基于STM32平台串口驱动
----------------------------------------------------------------------------------------*/
#include "hmi_user_uart.h"


/*!
*   \brief   串口初始化
*   \param   BaudRate-波特率设置
*/
void UartInit(uint32 BaudRate)
{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    USART_InitTypeDef USART_InitStructure;
//    /* Enable GPIO clock */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
////    GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);     //串口1重映射到PB6/7
//    
//	/* Configure USART Tx as alternate function push-pull */
//    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    //  /* Configure USART Rx as input floating */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    USART_DeInit(USART1);                                           //复位USART1
//    USART_InitStructure.USART_BaudRate = BaudRate;
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//    USART_InitStructure.USART_StopBits = USART_StopBits_1;
//    USART_InitStructure.USART_Parity = USART_Parity_No;
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//    USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;

//    /* USART configuration */
//    USART_Init(USART1, &USART_InitStructure);
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                  //接收中断使能

//    /* Enable USART */
//    USART_Cmd(USART1, ENABLE);
//	
	
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = BaudRate;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);   
}

/*!
*   \brief  发送1个字节
*   \param  t 发送的字节
*/
void  SendChar(uchar t)
{
    USART_SendData(USART1,t);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET));//等待串口发送完毕
}
