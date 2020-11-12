#include "sys.h"
#include "usart.h"	  
#include "cmd_queue.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

extern u8  cmd_buffer[CMD_MAX_SIZE];  
extern u8 g_Receive_success_flag;
extern OS_EVENT *Uart1RecMbox;          
u8         USART_Buf[QUEUE_MAX_SIZE];          //DMAĿ���ַ,�ڴ��ַ
u8          buff_length;
u8          Usart_Rx_Sta;

typedef struct {
	uint8_t* buffer;	//FIFO����
	uint16_t in;			//����±�
	uint16_t out;			//�����±�
	uint16_t size;		//FIFO��С
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
*   \brief   ����2��ʼ��
*   \param   BaudRate-����������
*/
void uart2_init(u32 BaudRate)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
		
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART1��GPIOAʱ��

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
    //USART1_RX	  GPIOA.10��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;     //  RE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//��λ����2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//ֹͣ��λ
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;             //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;        //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure);       //��ʼ������1
	// ��������DMA����
//	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); 
	
	/* ʹ�ܴ���2DMA ����DMAͨ��6*/
	DMA_DeInit(DMA1_Channel6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR)); //ADC��ַ  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&cmd_buffer;          //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //����(�����赽�ڴ�)    ��ת����adcֵ���浽�ڴ���
	DMA_InitStructure.DMA_BufferSize = CMD_MAX_SIZE;                    //�������ݵĴ�С  1��ͨ����ֵ  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //�ڴ��ַ������������һ�����֣�������Ҫ����4����ֵ��������Ҫ������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //�ڴ����ݵ�λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal  ;       //DMAģʽ��ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;    //���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            //��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);            //����DMA1_Channel5
	
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);         //DMAͨ��1��������ж�
	DMA_Cmd(DMA1_Channel6,ENABLE);
	
	
	/* ʹ�ܴ���2 DMA ����DMA1ͨ��7*/
	DMA_DeInit(DMA1_Channel7);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR)); //ADC��ַ  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&cmd_buffer;          //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //����(�����赽�ڴ�)    ��ת����adcֵ���浽�ڴ���
	DMA_InitStructure.DMA_BufferSize = CMD_MAX_SIZE;                    //�������ݵĴ�С  1��ͨ����ֵ  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //�ڴ��ַ������������һ�����֣�������Ҫ����4����ֵ��������Ҫ������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //�ڴ����ݵ�λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ;       //DMAģʽ��ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;    //���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            //��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);            //����DMA1_Channel5
	
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);         //DMAͨ��1��������ж�
	DMA_Cmd(DMA1_Channel7,ENABLE);

//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //�������ڽ����жϣ�����һ���ֽڣ�RXNEλ����λһ�Σ�����һ���ж�
//	USART_ITConfig(USART1, USART_IT_ORE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  //�������ڽ��ܿ����жϣ�������һ֡���ݣ���RXNEλ����λ�ҽ���ͣ�ٳ���һ�ֽ�ʱ�䣩
	USART_Cmd(USART2, ENABLE);   
	GPIO_SetBits(GPIOB,GPIO_Pin_2);  //��������Ϊ�͵�ƽ����ʾ����ģʽ
	
	Fifo_Init(&Uart2_Rx_Fifo,cmd_buffer,CMD_MAX_SIZE);
}

/*!
*   \brief  ����1���ֽ�
*   \param  t ���͵��ֽ�
*/
void  send2_char(u8 t)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_2);  //��������Ϊ�ߵ�ƽ����ʾ����ģʽ
    USART_SendData(USART2,t);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET));//�ȴ����ڷ������
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);  //��������Ϊ�ߵ�ƽ����ʾ����ģʽ
}

void Receive_DataPack(void)
{
	/* ���յ����ݳ��� */
	uint32_t buff_length;
	/* �ر�DMA ����ֹ���� */
	DMA_Cmd(DMA1_Channel6, DISABLE);  /* ��ʱ�ر�dma��������δ���� */ 
	/* ��DMA��־λ */
	DMA_ClearFlag( DMA1_FLAG_TC6 ); 
	/* ��ȡ���յ������ݳ��� ��λΪ�ֽ�*/
	buff_length = CMD_MAX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);        //����DMA���մ�С��һ���������ݳ���CMD_MAX_SIZE���Ͳ����жϣ�-��ȡʣ���ֽڿռ� = ���ν������ݵĴ�С
    /* ��ȡ���ݳ��� */
    Usart_Rx_Sta = buff_length;
	/* ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ */
	DMA1_Channel6->CNDTR = CMD_MAX_SIZE;    
  
	/* �˴�Ӧ���ڴ����������ٴ򿪣����� DataPack_Process() ��*/
	DMA_Cmd(DMA1_Channel6, ENABLE);   
}

void USART2_IRQHandler(void)          //����Һ����ʾ�����ز���
{//����һ��֡���ݣ��ж�һ��
	OS_CPU_SR  cpu_sr;
	u8 i;
	
	OS_ENTER_CRITICAL();   /* Tell uC/OS-II that we are starting an ISR   ���ж�  ����UC/OS-II������������ISR*/
	OSIntNesting++;         //�ж�Ƕ�ײ���
	OS_EXIT_CRITICAL();     //���ж�

	
//	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) != RESET)
//	{
//		(void)USART1->SR;
//		(void)USART1->DR; //�ȶ�SR�ٶ�DR�Ĵ���
//	}
	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		Receive_DataPack();
		//����һ���жϣ�˵�����յ�һָ֡��
		g_Receive_success_flag=1;
		USART_ReceiveData(USART2);    //��USART_DR�Ķ��������Խ������жϱ�־λ����
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

