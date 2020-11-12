#include "sys.h"
#include "usart.h"	  
#include "cmd_queue.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

extern u8  cmd_buffer[CMD_MAX_SIZE];  
extern u8 g_Receive_success_flag;
extern OS_EVENT *Uart1RecMbox;          
u8         USART_Buf[QUEUE_MAX_SIZE];          //DMA目标地址,内存地址
u8          buff_length;
u8          Usart_Rx_Sta;

typedef struct {
	uint8_t* buffer;	//FIFO数据
	uint16_t in;			//入口下标
	uint16_t out;			//出口下标
	uint16_t size;		//FIFO大小
}FIFO_Type

FIFO_Type Uart2_Rx_Fifo;

void Fifo_Init(FIFO_Type* fifo, uint8_t* buffer, uint16_t size)
{
	fifo->buffer = buffer;
	fifo->in = 0;
	fifo->out = 0;
	fifo->size = size;
}

uint16_t Fifo_Get(FIFO_Type* fifo, uint8_t* buffer, uint16_t len)
{
	uint16_t lenght;
	uint16_t in = fifo->in;	
	uint16_t i;
	lenght = (in + fifo->size - fifo->out)%fifo->size;
	if(lenght > len)
		lenght = len;
	for(i = 0; i < lenght; i++)
	{
		buffer[i] = fifo->buffer[(fifo->out + i)%fifo->size];
	}
	fifo->out = (fifo->out + lenght)%fifo->size;
	return lenght;
}

uint16_t Fifo_Status(FIFO_Type* fifo)
{
	uint16_t lenght;
	lenght = (fifo->in + fifo->size - fifo->out)%fifo->size;
	return lenght;
}

/*!
*   \brief   串口2初始化
*   \param   BaudRate-波特率设置
*/
void uart2_init(u32 BaudRate)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
		
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART1，GPIOA时钟

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;     //  RE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//复位串口2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//停止复位
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = BaudRate;             //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;        //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure);       //初始化串口1
	// 开启串口DMA接收
//	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); 
	
	/* 使能串口2DMA 接收DMA通道6*/
	DMA_DeInit(DMA1_Channel6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR)); //ADC地址  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&cmd_buffer;          //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //方向(从外设到内存)    将转换的adc值保存到内存中
	DMA_InitStructure.DMA_BufferSize = CMD_MAX_SIZE;                    //传输内容的大小  1条通道的值  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //内存地址增长，定义了一个数字，里面需要保存4个数值，所以需要增长；
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //外设数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //内存数据单位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal  ;       //DMA模式：循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;    //优先级：高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            //禁止内存到内存的传输
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);            //配置DMA1_Channel5
	
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);         //DMA通道1传输完成中断
	DMA_Cmd(DMA1_Channel6,ENABLE);
	
	
	/* 使能串口2 DMA 发送DMA1通道7*/
	DMA_DeInit(DMA1_Channel7);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR)); //ADC地址  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&cmd_buffer;          //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //方向(从外设到内存)    将转换的adc值保存到内存中
	DMA_InitStructure.DMA_BufferSize = CMD_MAX_SIZE;                    //传输内容的大小  1条通道的值  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //内存地址增长，定义了一个数字，里面需要保存4个数值，所以需要增长；
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //外设数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //内存数据单位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ;       //DMA模式：循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;    //优先级：高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            //禁止内存到内存的传输
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);            //配置DMA1_Channel5
	
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);         //DMA通道1传输完成中断
	DMA_Cmd(DMA1_Channel7,ENABLE);

//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //开启串口接受中断，接收一个字节，RXNE位被置位一次，产生一次中断
//	USART_ITConfig(USART1, USART_IT_ORE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  //开启串口接受空闲中断，接收完一帧数据，（RXNE位被置位且接收停顿超过一字节时间）
	USART_Cmd(USART2, ENABLE);   
	GPIO_SetBits(GPIOB,GPIO_Pin_2);  //该引脚置为低电平，表示接收模式
	
	Fifo_Init(&Uart2_Rx_Fifo,cmd_buffer,CMD_MAX_SIZE);
}

/*!
*   \brief  发送1个字节
*   \param  t 发送的字节
*/
void  send2_char(u8 t)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_2);  //该引脚置为高电平，表示发送模式
    USART_SendData(USART2,t);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET));//等待串口发送完毕
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);  //该引脚置为高电平，表示发送模式
}

void Receive_DataPack(void)
{
	/* 接收的数据长度 */
	uint32_t buff_length;
	/* 关闭DMA ，防止干扰 */
	DMA_Cmd(DMA1_Channel6, DISABLE);  /* 暂时关闭dma，数据尚未处理 */ 
	/* 清DMA标志位 */
	DMA_ClearFlag( DMA1_FLAG_TC6 ); 
	/* 获取接收到的数据长度 单位为字节*/
	buff_length = CMD_MAX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);        //设置DMA接收大小（一旦接收数据超过CMD_MAX_SIZE，就产生中断）-获取剩余字节空间 = 本次接收数据的大小
    /* 获取数据长度 */
    Usart_Rx_Sta = buff_length;
	/* 重新赋值计数值，必须大于等于最大可能接收到的数据帧数目 */
	DMA1_Channel6->CNDTR = CMD_MAX_SIZE;    
  
	/* 此处应该在处理完数据再打开，如在 DataPack_Process() 打开*/
	DMA_Cmd(DMA1_Channel6, ENABLE);   
}

void USART2_IRQHandler(void)          //接收液晶显示屏返回参数
{//接收一个帧数据，中断一次
	OS_CPU_SR  cpu_sr;
	u8 i;
	
	OS_ENTER_CRITICAL();   /* Tell uC/OS-II that we are starting an ISR   关中断  告诉UC/OS-II我们正在启动ISR*/
	OSIntNesting++;         //中断嵌套层数
	OS_EXIT_CRITICAL();     //开中断

	
//	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) != RESET)
//	{
//		(void)USART1->SR;
//		(void)USART1->DR; //先读SR再读DR寄存器
//	}
	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		Receive_DataPack();
		//进入一次中断，说明接收到一帧指令
		g_Receive_success_flag=1;
		USART_ReceiveData(USART2);    //对USART_DR的读操作可以将接收中断标志位清零
		for(i=0;i<;i++)
		{
			queue_push(cmd_buffer[i]);
		}
	}
	  
//	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
//    {
//        uint8_t data = USART_ReceiveData(USART2);
//        queue_push(data);
//    }

	OSIntExit();
}

