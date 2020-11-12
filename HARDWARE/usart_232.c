#include "sys.h"
#include "usart.h"	  
#include "cmd_queue.h"
#include "delay.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif


extern OS_EVENT *Uart1RecMbox;    

u8     USART3_RX_Buf[CMD_MAX_SIZE];          //DMA目标地址,内存地址    接收缓冲区
u8     USART3_TX_Buf[CMD_MAX_SIZE];          //DMA目标地址,内存地址    发送缓冲区

void DMA1_Channel2_Send(u8 size);


/******************************************************************************/

/*!
*   \brief   串口3初始化
*   \param   BaudRate-波特率设置
*/
void uart3_init(u32 BaudRate)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3，GPIOB时钟

	//USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PORTB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.10
   
    //USART3_RX	  GPIOB.11初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;           //PORTB.11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);              //初始化GPIOB.11  

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = BaudRate;              //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;         //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure);                       //初始化串口1
	// 开启串口DMA接收
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); 
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE); 
	
	/* 使能串口3 DMA 接收DMA通道3*/
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART3->DR));   //ADC地址  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART3_RX_Buf;           //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      //方向(从外设到内存)    将转换的adc值保存到内存中
	DMA_InitStructure.DMA_BufferSize = CMD_MAX_SIZE;                        //传输内容的大小  1条通道的值  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //内存地址增长，定义了一个数字，里面需要保存4个数值，所以需要增长；
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //外设数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //内存数据单位
	DMA_InitStructure.DMA_Mode =  DMA_Mode_Normal ;           //DMA模式：循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;    //优先级：高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;              //禁止内存到内存的传输
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);              //配置DMA1_Channel5
	
//	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);           //DMA通道1传输完成中断
	DMA_Cmd(DMA1_Channel3,ENABLE);
	
	/* 使能串口3 DMA 发送DMA1通道2*/
	DMA_DeInit(DMA1_Channel2);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART3->DR));  //ADC地址  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART3_TX_Buf;        //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                     //方向(从内存到外设)    将转换的adc值保存到内存中
	DMA_InitStructure.DMA_BufferSize = 0;                       //传输内容的大小  1条通道的值  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                //内存地址增长，定义了一个数字，里面需要保存4个数值，所以需要增长；
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //外设数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //内存数据单位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal  ;          //DMA模式：循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;     //优先级：高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;               //禁止内存到内存的传输
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);               //配置DMA1_Channel5
	
//	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);            //DMA通道1传输完成中断
	DMA_Cmd(DMA1_Channel2,ENABLE);


//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //开启串口接受中断，接收一个字节，RXNE位被置位一次，产生一次中断

	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  //开启串口接受空闲中断，接收完一帧数据，（RXNE位被置位且接收停顿超过一字节时间）
	
	USART_ClearFlag(USART3,USART_FLAG_TC);
	USART_Cmd(USART3, ENABLE);   
}

/*!
*   \brief  发送1个字节
*   \param  t 发送的字节
*/
void  Send232Char(u8 t)        //发送数据，需要使能DMA发送通道，并且设置引脚为发送模式
{
	static u16 i=0;         //需要知道发送的位数
	static u32 cmd_state;
	
	USART3_TX_Buf[i++]=t;                       //将需要发送的值赋值到DMA发送源地址
	cmd_state = ((cmd_state<<8)|t);          //拼接最后4个字节，组成一个32位整数
	if(cmd_state==0XFFFCFFFF)                
	{
		DMA1_Channel2_Send(i);
		i=0;
	}
}

/************串口3 DMA发送数据**************/
void DMA1_Channel2_Send(u8 size)
{	
	while(USART_GetFlagStatus(USART3,USART_FLAG_RXNE)!=RESET);   //说明串口接收到数据此时不要发送数据
	
	if(!size) return ;                                 // 判断长度是否有效
	delay_us(10);
    while (DMA_GetCurrDataCounter(DMA1_Channel2));     //检查DMA发送通道内是否还有数据,DMA 发送完成
	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel2, size);
	
//  USART_ITConfig(USART3, USART_IT_TC, ENABLE);  //开启串口发送完成中断
	DMA_Cmd(DMA1_Channel2, ENABLE);        //打开串口发送DMA通道
    while(DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);//等待发送结束
    while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);//等待串口发送结束	
	USART_ClearFlag(USART3,USART_FLAG_TC);
    DMA_ClearFlag(DMA1_FLAG_TC2);//清除标志位
    DMA_Cmd(DMA1_Channel2,DISABLE);
}

/*************串口3 DMA发送中断***************/
void DMA1_Channel2_IRQHandler(void)           //发送完成DMA中断，关闭DMA，将方向改为接收  
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   /* Tell uC/OS-II that we are starting an ISR   关中断  告诉UC/OS-II我们正在启动ISR*/
	OSIntNesting++;         //中断嵌套层数
	OS_EXIT_CRITICAL();     //开中断
	
    if (DMA_GetITStatus(DMA1_FLAG_TC2))
    {
		USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);

		USART_ITConfig(USART3, USART_IT_TC, ENABLE);  //开启串口发送完成中断
		
        DMA_ClearITPendingBit(DMA1_IT_TC2);
		
		DMA_ClearITPendingBit(DMA1_IT_GL2); //清除全部中断标志

        DMA_Cmd(DMA1_Channel2, DISABLE);        //关闭DMA
    }
	
	OSIntExit();
}

void Receive3_DataPack(void)
{
	u8 i;
	static u32 cmd_state;

	uint32_t buff_length;	 /* 接收的数据长度 */
	
	USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);
	DMA_Cmd(DMA1_Channel3, DISABLE);  /* 暂时关闭dma，数据尚未处理， 关闭DMA ，防止干扰 */
	DMA_ClearFlag( DMA1_FLAG_TC3 ); 	/* 清DMA标志位 */
//	DMA_ClearITPendingBit(DMA1_IT_GL3); //清除全部中断标志
	buff_length = CMD_MAX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);        //设置DMA接收大小（一旦接收数据超过CMD_MAX_SIZE，就产生中断）-获取剩余字节空间 = 本次接收数据的大小
    
	
	if(USART3_RX_Buf[0] != 0xee)   return;
	for(i=4; i>0; i--)
	{
		cmd_state = ((cmd_state<<8)|USART3_RX_Buf[buff_length-i]);
	}
	
	if(cmd_state != 0XFFFCFFFF)   return;

	for(i=0;i<buff_length;i++)
	{
		queue_push(USART3_RX_Buf[i]);
	}
}

void USART3_IRQHandler(void)          //接收液晶显示屏返回参数
{//接收一个帧数据，中断一次
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   /* Tell uC/OS-II that we are starting an ISR   关中断  告诉UC/OS-II我们正在启动ISR*/
	OSIntNesting++;         //中断嵌套层数
	OS_EXIT_CRITICAL();     //开中断

	if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)//进入一次中断，说明接收到一帧指令
	{
		(void)USART3->SR;     //对USART_DR的读操作可以将接收中断标志位清零
		(void)USART3->DR;   //先读SR再读DR寄存器
		Receive3_DataPack();
		
		DMA1_Channel3->CNDTR = CMD_MAX_SIZE;    /* 重新赋值计数值，必须大于等于最大可能接收到的数据帧数目 */
		DMA_Cmd(DMA1_Channel3, ENABLE);  		 /* 此处应该在处理完数据再打开，如在 DataPack_Process() 打开*/
		USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	}
	
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//进入一次中断，说明接收到一帧指令
	{
		uint8_t data = USART_ReceiveData(USART3);
        queue_push(data);
	}  
	
	if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)       //发送完成中断   丢失
    {
        USART_ClearITPendingBit(USART3, USART_IT_TC);
	    
		USART_ITConfig(USART3, USART_IT_TC, DISABLE);  //转换为接收模式，需要暂时关闭串口发送完成中断   
		USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
    }

	OSIntExit();
}

