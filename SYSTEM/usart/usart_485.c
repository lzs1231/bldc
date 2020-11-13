#include "sys.h"
#include "usart.h"	  
#include "cmd_queue.h"
#include "delay.h"
#include "main.h"
#include "modbus.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

#define CMD_MAX_SIZE 128           // ����ָ���С��������Ҫ�������������ô�һЩ

extern OS_EVENT *Uart1RecMbox;    

u8   RS485_RX_BUFF[CMD_MAX_SIZE];          //DMAĿ���ַ,�ڴ��ַ    ���ջ�����
u8   RS485_TX_BUFF[CMD_MAX_SIZE];           //DMAĿ���ַ,�ڴ��ַ    ���ͻ�����


void DMA1_Channel7_Send(u8 size);



void max485Enable_RX()
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);               //��������Ϊ�͵�ƽ����ʾ����ģʽ
}

void max485Enable_TX()
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);                //��������Ϊ�ߵ�ƽ����ʾ����ģʽ
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
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART1��GPIOAʱ��

	//USART1_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PORTA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
    //USART1_RX	  GPIOA.3��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;           //PORTA.3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);              //��ʼ��GPIOA.3
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;           //  RE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;              //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;         //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure);                       //��ʼ������1
	// ��������DMA����
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); 
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); 
	
	/* ʹ�ܴ���2DMA ����DMAͨ��6*/
	DMA_DeInit(DMA1_Channel6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->DR));   //ADC��ַ  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&RS485_RX_BUFF;        //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      //����(�����赽�ڴ�)    ��ת����adcֵ���浽�ڴ���
	DMA_InitStructure.DMA_BufferSize = CMD_MAX_SIZE;                        //�������ݵĴ�С  1��ͨ����ֵ  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //�ڴ��ַ������������һ�����֣�������Ҫ����4����ֵ��������Ҫ������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //�ڴ����ݵ�λ
	DMA_InitStructure.DMA_Mode =  DMA_Mode_Normal ;           //DMAģʽ��ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;    //���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;              //��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);              //����DMA1_Channel5
	
//	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);           //DMAͨ��1��������ж�
	DMA_Cmd(DMA1_Channel6,ENABLE);
	
	/* ʹ�ܴ���2 DMA ����DMA1ͨ��7*/
	DMA_DeInit(DMA1_Channel7);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->DR));  //ADC��ַ  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&RS485_TX_BUFF;       //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                     //����(���ڴ浽����)    ��ת����adcֵ���浽�ڴ���
	DMA_InitStructure.DMA_BufferSize = 0;                       //�������ݵĴ�С  1��ͨ����ֵ  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                //�ڴ��ַ������������һ�����֣�������Ҫ����4����ֵ��������Ҫ������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //�ڴ����ݵ�λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal  ;          //DMAģʽ��ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;     //���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;               //��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);               //����DMA1_Channel5
	
//	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);            //DMAͨ��1��������ж�
	DMA_Cmd(DMA1_Channel7,ENABLE);

//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //�������ڽ����жϣ�����һ���ֽڣ�RXNEλ����λһ�Σ�����һ���ж�
	
	max485Enable_RX();
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  //�������ڽ��ܿ����жϣ�������һ֡���ݣ���RXNEλ����λ�ҽ���ͣ�ٳ���һ�ֽ�ʱ�䣩
	
	USART_ClearFlag(USART2,USART_FLAG_TC);
	USART_Cmd(USART2, ENABLE);   
}

/*!
*   \brief  ����1���ֽ�
*   \param  t ���͵��ֽ�
*/
void  Send485Char(u8 t)        //�������ݣ���Ҫʹ��DMA����ͨ����������������Ϊ����ģʽ
{
	static u16 len=0;         //��Ҫ֪�����͵�λ��
	static u32 cmd_state;
	
	RS485_TX_BUFF[len++]=t;                       //����Ҫ���͵�ֵ��ֵ��DMA����Դ��ַ
//	SEGGER_RTT_printf(0,"%3d\t",t);	
	cmd_state = ((cmd_state<<8)|t);          //ƴ�����4���ֽڣ����һ��32λ����
	if(cmd_state==0XFFFCFFFF)                
	{
		DMA1_Channel7_Send(len);
		len=0;
	}
}

//////////////////////////////////////////////////////////////////////////////
//����n���ֽ�����
//buff:�������׵�ַ
//len�����͵��ֽ���
void RS485_SendData(u8 *buff,u8 len)
{
	memcpy(RS485_TX_BUFF,buff,len);
	DMA1_Channel7_Send(len);
}

/************����2 DMA��������**************/
void DMA1_Channel7_Send(u8 size)
{	
	while(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)!=RESET);   //˵�����ڽ��յ����ݴ�ʱ��Ҫ��������
	
	if(!size) return ;                                      // �жϳ����Ƿ���Ч
//	delay_us(10);
    while (DMA_GetCurrDataCounter(DMA1_Channel7));          //���DMA����ͨ�����Ƿ�������,DMA �������
	
	max485Enable_TX();                                      //����Ϊ���ͣ��ȴ����ڷ���
	
	DMA_Cmd(DMA1_Channel7, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel7, size);
	
//  USART_ITConfig(USART2, USART_IT_TC, ENABLE);  //�������ڷ�������ж�
	DMA_Cmd(DMA1_Channel7, ENABLE);               //�򿪴��ڷ���DMAͨ��
	
    while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);           //�ȴ����ͽ�������Ҫ���͵����������ֽ�ȫ�����͵����ڵķ������ݼĴ�����
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);  //�ȴ����ڷ��ͽ���	
	USART_ClearFlag(USART2,USART_FLAG_TC);
    DMA_ClearFlag(DMA1_FLAG_TC7);//�����־λ
	
    DMA_Cmd(DMA1_Channel7,DISABLE);
	
	max485Enable_RX();                                          //����Ϊ���գ��ȴ����ڽ���
}

/*************����2 DMA�����ж�***************/
void DMA1_Channel7_IRQHandler(void)           //�������DMA�жϣ��ر�DMA���������Ϊ����  
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   /* Tell uC/OS-II that we are starting an ISR   ���ж�  ����UC/OS-II������������ISR*/
	OSIntNesting++;         //�ж�Ƕ�ײ���
	OS_EXIT_CRITICAL();     //���ж�
	
    if (DMA_GetITStatus(DMA1_FLAG_TC7))
    {
		USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);  //�������ڷ�������ж�
		
		DMA_Cmd(DMA1_Channel7, DISABLE);         //�ر�DMA
        DMA_ClearITPendingBit(DMA1_IT_TC7);
		DMA_ClearITPendingBit(DMA1_IT_GL7);      //���ȫ���жϱ�־
    }
	OSIntExit();
}

void Receive_DataPack(void)
{
	u8 i;
	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);
	DMA_Cmd(DMA1_Channel6, DISABLE);    /* ��ʱ�ر�dma��������δ���� �ر�DMA ����ֹ���� */
	DMA_ClearFlag( DMA1_FLAG_TC6 ); 	/* ��DMA��־λ */
//	DMA_ClearITPendingBit(DMA1_IT_GL6); //���ȫ���жϱ�־
	modbus.recount = CMD_MAX_SIZE-DMA_GetCurrDataCounter(DMA1_Channel6); 
	

	for(i=0;i<modbus.recount;i++)
	{
		modbus.rcbuf[i] = RS485_RX_BUFF[i];
		#ifdef _TEST_DMA
		printf("%d\t%d\n",modbus.rcbuf[i],i);
		#endif
	}
	
	DMA1_Channel6->CNDTR = CMD_MAX_SIZE;    /* ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ */
	DMA_Cmd(DMA1_Channel6, ENABLE);         /* �˴�Ӧ���ڴ����������ٴ򿪣����� DataPack_Process() ��*/
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
}

void USART2_IRQHandler(void)          //����Һ����ʾ�����ز���
{//����һ��֡���ݣ��ж�һ��
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();      /* Tell uC/OS-II that we are starting an ISR   ���ж�  ����UC/OS-II������������ISR*/
	OSIntNesting++;           //�ж�Ƕ�ײ���
	OS_EXIT_CRITICAL();       //���ж�

	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)//����һ���жϣ�˵�����յ�һָ֡��
	{
		(void)USART2->SR;     //��USART_DR�Ķ��������Խ������жϱ�־λ����
		(void)USART2->DR;     //�ȶ�SR�ٶ�DR�Ĵ���
		Receive_DataPack();
		modbus.reflag = 1;
	}
	
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//����һ���жϣ�˵�����յ�һָ֡��
	{
		uint8_t data = USART_ReceiveData(USART2);
//      queue_push(data);
	}  
	
	if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)  //��������ж�   ��ʧ
    {
        USART_ClearITPendingBit(USART2, USART_IT_TC);
	    
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);     //ת��Ϊ����ģʽ����Ҫ��ʱ�رմ��ڷ�������ж�   
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    }

	OSIntExit();
}


