#include "sys.h"
#include "usart.h"	  
#include "cmd_queue.h"
#include "delay.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif


extern OS_EVENT *Uart1RecMbox;    

u8     USART3_RX_Buf[CMD_MAX_SIZE];          //DMAĿ���ַ,�ڴ��ַ    ���ջ�����
u8     USART3_TX_Buf[CMD_MAX_SIZE];          //DMAĿ���ַ,�ڴ��ַ    ���ͻ�����

void DMA1_Channel2_Send(u8 size);


/******************************************************************************/

/*!
*   \brief   ����3��ʼ��
*   \param   BaudRate-����������
*/
void uart3_init(u32 BaudRate)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3��GPIOBʱ��

	//USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PORTB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.10
   
    //USART3_RX	  GPIOB.11��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;           //PORTB.11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);              //��ʼ��GPIOB.11  

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;              //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;         //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART3, &USART_InitStructure);                       //��ʼ������1
	// ��������DMA����
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); 
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE); 
	
	/* ʹ�ܴ���3 DMA ����DMAͨ��3*/
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART3->DR));   //ADC��ַ  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART3_RX_Buf;           //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      //����(�����赽�ڴ�)    ��ת����adcֵ���浽�ڴ���
	DMA_InitStructure.DMA_BufferSize = CMD_MAX_SIZE;                        //�������ݵĴ�С  1��ͨ����ֵ  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //�ڴ��ַ������������һ�����֣�������Ҫ����4����ֵ��������Ҫ������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //�ڴ����ݵ�λ
	DMA_InitStructure.DMA_Mode =  DMA_Mode_Normal ;           //DMAģʽ��ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;    //���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;              //��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);              //����DMA1_Channel5
	
//	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);           //DMAͨ��1��������ж�
	DMA_Cmd(DMA1_Channel3,ENABLE);
	
	/* ʹ�ܴ���3 DMA ����DMA1ͨ��2*/
	DMA_DeInit(DMA1_Channel2);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART3->DR));  //ADC��ַ  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART3_TX_Buf;        //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                     //����(���ڴ浽����)    ��ת����adcֵ���浽�ڴ���
	DMA_InitStructure.DMA_BufferSize = 0;                       //�������ݵĴ�С  1��ͨ����ֵ  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                //�ڴ��ַ������������һ�����֣�������Ҫ����4����ֵ��������Ҫ������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //�ڴ����ݵ�λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal  ;          //DMAģʽ��ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;     //���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;               //��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);               //����DMA1_Channel5
	
//	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);            //DMAͨ��1��������ж�
	DMA_Cmd(DMA1_Channel2,ENABLE);


//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //�������ڽ����жϣ�����һ���ֽڣ�RXNEλ����λһ�Σ�����һ���ж�

	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  //�������ڽ��ܿ����жϣ�������һ֡���ݣ���RXNEλ����λ�ҽ���ͣ�ٳ���һ�ֽ�ʱ�䣩
	
	USART_ClearFlag(USART3,USART_FLAG_TC);
	USART_Cmd(USART3, ENABLE);   
}

/*!
*   \brief  ����1���ֽ�
*   \param  t ���͵��ֽ�
*/
void  Send232Char(u8 t)        //�������ݣ���Ҫʹ��DMA����ͨ����������������Ϊ����ģʽ
{
	static u16 i=0;         //��Ҫ֪�����͵�λ��
	static u32 cmd_state;
	
	USART3_TX_Buf[i++]=t;                       //����Ҫ���͵�ֵ��ֵ��DMA����Դ��ַ
	cmd_state = ((cmd_state<<8)|t);          //ƴ�����4���ֽڣ����һ��32λ����
	if(cmd_state==0XFFFCFFFF)                
	{
		DMA1_Channel2_Send(i);
		i=0;
	}
}

/************����3 DMA��������**************/
void DMA1_Channel2_Send(u8 size)
{	
	while(USART_GetFlagStatus(USART3,USART_FLAG_RXNE)!=RESET);   //˵�����ڽ��յ����ݴ�ʱ��Ҫ��������
	
	if(!size) return ;                                 // �жϳ����Ƿ���Ч
	delay_us(10);
    while (DMA_GetCurrDataCounter(DMA1_Channel2));     //���DMA����ͨ�����Ƿ�������,DMA �������
	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel2, size);
	
//  USART_ITConfig(USART3, USART_IT_TC, ENABLE);  //�������ڷ�������ж�
	DMA_Cmd(DMA1_Channel2, ENABLE);        //�򿪴��ڷ���DMAͨ��
    while(DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);//�ȴ����ͽ���
    while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);//�ȴ����ڷ��ͽ���	
	USART_ClearFlag(USART3,USART_FLAG_TC);
    DMA_ClearFlag(DMA1_FLAG_TC2);//�����־λ
    DMA_Cmd(DMA1_Channel2,DISABLE);
}

/*************����3 DMA�����ж�***************/
void DMA1_Channel2_IRQHandler(void)           //�������DMA�жϣ��ر�DMA���������Ϊ����  
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   /* Tell uC/OS-II that we are starting an ISR   ���ж�  ����UC/OS-II������������ISR*/
	OSIntNesting++;         //�ж�Ƕ�ײ���
	OS_EXIT_CRITICAL();     //���ж�
	
    if (DMA_GetITStatus(DMA1_FLAG_TC2))
    {
		USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);

		USART_ITConfig(USART3, USART_IT_TC, ENABLE);  //�������ڷ�������ж�
		
        DMA_ClearITPendingBit(DMA1_IT_TC2);
		
		DMA_ClearITPendingBit(DMA1_IT_GL2); //���ȫ���жϱ�־

        DMA_Cmd(DMA1_Channel2, DISABLE);        //�ر�DMA
    }
	
	OSIntExit();
}

void Receive3_DataPack(void)
{
	u8 i;
	static u32 cmd_state;

	uint32_t buff_length;	 /* ���յ����ݳ��� */
	
	USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);
	DMA_Cmd(DMA1_Channel3, DISABLE);  /* ��ʱ�ر�dma��������δ���� �ر�DMA ����ֹ���� */
	DMA_ClearFlag( DMA1_FLAG_TC3 ); 	/* ��DMA��־λ */
//	DMA_ClearITPendingBit(DMA1_IT_GL3); //���ȫ���жϱ�־
	buff_length = CMD_MAX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);        //����DMA���մ�С��һ���������ݳ���CMD_MAX_SIZE���Ͳ����жϣ�-��ȡʣ���ֽڿռ� = ���ν������ݵĴ�С
    
	
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

void USART3_IRQHandler(void)          //����Һ����ʾ�����ز���
{//����һ��֡���ݣ��ж�һ��
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   /* Tell uC/OS-II that we are starting an ISR   ���ж�  ����UC/OS-II������������ISR*/
	OSIntNesting++;         //�ж�Ƕ�ײ���
	OS_EXIT_CRITICAL();     //���ж�

	if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)//����һ���жϣ�˵�����յ�һָ֡��
	{
		(void)USART3->SR;     //��USART_DR�Ķ��������Խ������жϱ�־λ����
		(void)USART3->DR;   //�ȶ�SR�ٶ�DR�Ĵ���
		Receive3_DataPack();
		
		DMA1_Channel3->CNDTR = CMD_MAX_SIZE;    /* ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ */
		DMA_Cmd(DMA1_Channel3, ENABLE);  		 /* �˴�Ӧ���ڴ����������ٴ򿪣����� DataPack_Process() ��*/
		USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	}
	
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//����һ���жϣ�˵�����յ�һָ֡��
	{
		uint8_t data = USART_ReceiveData(USART3);
        queue_push(data);
	}  
	
	if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)       //��������ж�   ��ʧ
    {
        USART_ClearITPendingBit(USART3, USART_IT_TC);
	    
		USART_ITConfig(USART3, USART_IT_TC, DISABLE);  //ת��Ϊ����ģʽ����Ҫ��ʱ�رմ��ڷ�������ж�   
		USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
    }

	OSIntExit();
}

