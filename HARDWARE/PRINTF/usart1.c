#include "sys.h"
#include "usart.h"	  
#include "cmd_queue.h"
#include "delay.h"

//#define DMA_NVIC

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif



u8     USART1_TX_Buf[400];          //DMA目标地址,内存地址    发送缓冲区


void DMA1_Channel4_Send(u16 size);


/******************************************************************************/

/*!
*   \brief   串口3初始化
*   \param   BaudRate-波特率设置
*/
void uart1_init(u32 BaudRate)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1，GPIOB时钟

	//USART1_TX   GPIOB.6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PORTB.6
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.6
   
    //USART1_RX	  GPIOB.7初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;           //PORTB.7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);              //初始化GPIOB.7

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = BaudRate;              //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;         //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode =  USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure);                       //初始化串口1
	// 开启串口DMA发送
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); 
	
	
	/* 使能串口1 DMA 发送DMA1通道4*/
	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));  //ADC地址  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART1_TX_Buf;        //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                     //方向(从内存到外设)    将转换的adc值保存到内存中
	DMA_InitStructure.DMA_BufferSize = 0;                       //传输内容的大小  1条通道的值  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                //内存地址增长，定义了一个数字，里面需要保存4个数值，所以需要增长；
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //外设数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //内存数据单位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal  ;          //DMA模式：循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;     //优先级：高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;               //禁止内存到内存的传输
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);               //配置DMA1_Channel4
	
	
//	DMA_Cmd(DMA1_Channel4,ENABLE);

	USART_ClearFlag(USART1,USART_FLAG_TC); 
	USART_Cmd(USART1, ENABLE);   
}



/*!
*   \brief  发送1个字节
*   \param  t 发送的字节
*/
void  Send1Char(u8 t)        //发送数据，需要使能DMA发送通道，并且设置引脚为发送模式
{
	USART_SendData(USART1,t);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET); //等待串口发送结束	
}

/*!
*   \brief  发送1个字
*   \param  t 发送的字
*/
void  Send1Int(int t)        //发送数据，需要使能DMA发送通道，并且设置引脚为发送模式
{
	u8 TX_Buf[5];
	
	if(t<0) t=-t;
	TX_Buf[0] = (t/1000)+0x30;
	TX_Buf[1] = (t/100)%10+0x30;
	TX_Buf[2] = (t/10)%10+0x30;
	TX_Buf[3] = (t/1)%10+0x30;
	TX_Buf[4] = 0x0a;

	memcpy(USART1_TX_Buf,TX_Buf,5);

	DMA1_Channel4_Send(5);
}

//////////////////////////////////////////////////////////////////////////////
//发送n个字节数据
//buff:发送区首地址
//len：发送的字节数
void Test_SendData(u8 *buff,u16 len)
{
	memcpy(USART1_TX_Buf,buff,len);
	DMA1_Channel4_Send(len);
}

#ifndef DMA_NVIC 
/************串口1 DMA发送数据(不使用DMA中断)**************/
void DMA1_Channel4_Send(u16 size)
{	
	if(!size) return ;                                 // 判断长度是否有效
//    while (DMA_GetCurrDataCounter(DMA1_Channel4));     //检查DMA发送通道内是否还有数据,DMA 发送完成
	
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel4, size);
	DMA_Cmd(DMA1_Channel4, ENABLE);        //打开串口发送DMA通道
	
    while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);          //等待发送结束
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET); //等待串口发送结束	
	USART_ClearFlag(USART1,USART_FLAG_TC);
	
    DMA_ClearFlag(DMA1_FLAG_TC4);//清除标志位
    DMA_Cmd(DMA1_Channel4,DISABLE);
}
#elif
/************串口1 DMA发送数据(使用DMA中断)**************/
void DMA1_Channel4_Send(u16 size)
{	
	if(!size) return ;                                 // 判断长度是否有效
//  while (DMA_GetCurrDataCounter(DMA1_Channel4));     //检查DMA发送通道内是否还有数据,DMA 发送完成
	
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel4, size);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);   //打开DMA通道1传输完成中断
	DMA_Cmd(DMA1_Channel4, ENABLE);                   //打开串口发送DMA通道
	
    while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);          //等待发送结束
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET); //等待串口发送结束	
	USART_ClearFlag(USART1,USART_FLAG_TC);
	
    DMA_ClearFlag(DMA1_FLAG_TC4);//清除标志位
    DMA_Cmd(DMA1_Channel4,DISABLE);
}

/*************串口1 DMA发送中断***************/
void DMA1_Channel4_IRQHandler(void)           //发送完成DMA中断，关闭DMA，将方向改为接收  
{
	OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL();   /* Tell uC/OS-II that we are starting an ISR   关中断  告诉UC/OS-II我们正在启动ISR*/
	OSIntNesting++;         //中断嵌套层数
	OS_EXIT_CRITICAL();     //开中断
	
    if (DMA_GetITStatus(DMA1_FLAG_TC4))
    {
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);  //开启串口发送完成中断

        DMA_ClearITPendingBit(DMA1_IT_TC4);
		DMA_ClearITPendingBit(DMA1_IT_GL4); //清除全部中断标志

        DMA_Cmd(DMA1_Channel4, DISABLE);        //关闭DMA
    }
	
	OSIntExit();
}

#endif
