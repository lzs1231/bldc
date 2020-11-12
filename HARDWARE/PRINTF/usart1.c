#include "sys.h"
#include "usart.h"	  
#include "cmd_queue.h"
#include "delay.h"

//#define DMA_NVIC

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif



u8     USART1_TX_Buf[400];          //DMAĿ���ַ,�ڴ��ַ    ���ͻ�����


void DMA1_Channel4_Send(u16 size);


/******************************************************************************/

/*!
*   \brief   ����3��ʼ��
*   \param   BaudRate-����������
*/
void uart1_init(u32 BaudRate)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//ʹ��USART1��GPIOBʱ��

	//USART1_TX   GPIOB.6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PORTB.6
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.6
   
    //USART1_RX	  GPIOB.7��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;           //PORTB.7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);              //��ʼ��GPIOB.7

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;              //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;         //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode =  USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);                       //��ʼ������1
	// ��������DMA����
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); 
	
	
	/* ʹ�ܴ���1 DMA ����DMA1ͨ��4*/
	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));  //ADC��ַ  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART1_TX_Buf;        //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                     //����(���ڴ浽����)    ��ת����adcֵ���浽�ڴ���
	DMA_InitStructure.DMA_BufferSize = 0;                       //�������ݵĴ�С  1��ͨ����ֵ  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                //�ڴ��ַ������������һ�����֣�������Ҫ����4����ֵ��������Ҫ������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;  //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;          //�ڴ����ݵ�λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal  ;          //DMAģʽ��ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;     //���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;               //��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);               //����DMA1_Channel4
	
	
//	DMA_Cmd(DMA1_Channel4,ENABLE);

	USART_ClearFlag(USART1,USART_FLAG_TC); 
	USART_Cmd(USART1, ENABLE);   
}



/*!
*   \brief  ����1���ֽ�
*   \param  t ���͵��ֽ�
*/
void  Send1Char(u8 t)        //�������ݣ���Ҫʹ��DMA����ͨ����������������Ϊ����ģʽ
{
	USART_SendData(USART1,t);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET); //�ȴ����ڷ��ͽ���	
}

/*!
*   \brief  ����1����
*   \param  t ���͵���
*/
void  Send1Int(int t)        //�������ݣ���Ҫʹ��DMA����ͨ����������������Ϊ����ģʽ
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
//����n���ֽ�����
//buff:�������׵�ַ
//len�����͵��ֽ���
void Test_SendData(u8 *buff,u16 len)
{
	memcpy(USART1_TX_Buf,buff,len);
	DMA1_Channel4_Send(len);
}

#ifndef DMA_NVIC 
/************����1 DMA��������(��ʹ��DMA�ж�)**************/
void DMA1_Channel4_Send(u16 size)
{	
	if(!size) return ;                                 // �жϳ����Ƿ���Ч
//    while (DMA_GetCurrDataCounter(DMA1_Channel4));     //���DMA����ͨ�����Ƿ�������,DMA �������
	
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel4, size);
	DMA_Cmd(DMA1_Channel4, ENABLE);        //�򿪴��ڷ���DMAͨ��
	
    while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);          //�ȴ����ͽ���
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET); //�ȴ����ڷ��ͽ���	
	USART_ClearFlag(USART1,USART_FLAG_TC);
	
    DMA_ClearFlag(DMA1_FLAG_TC4);//�����־λ
    DMA_Cmd(DMA1_Channel4,DISABLE);
}
#elif
/************����1 DMA��������(ʹ��DMA�ж�)**************/
void DMA1_Channel4_Send(u16 size)
{	
	if(!size) return ;                                 // �жϳ����Ƿ���Ч
//  while (DMA_GetCurrDataCounter(DMA1_Channel4));     //���DMA����ͨ�����Ƿ�������,DMA �������
	
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel4, size);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);   //��DMAͨ��1��������ж�
	DMA_Cmd(DMA1_Channel4, ENABLE);                   //�򿪴��ڷ���DMAͨ��
	
    while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);          //�ȴ����ͽ���
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET); //�ȴ����ڷ��ͽ���	
	USART_ClearFlag(USART1,USART_FLAG_TC);
	
    DMA_ClearFlag(DMA1_FLAG_TC4);//�����־λ
    DMA_Cmd(DMA1_Channel4,DISABLE);
}

/*************����1 DMA�����ж�***************/
void DMA1_Channel4_IRQHandler(void)           //�������DMA�жϣ��ر�DMA���������Ϊ����  
{
	OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL();   /* Tell uC/OS-II that we are starting an ISR   ���ж�  ����UC/OS-II������������ISR*/
	OSIntNesting++;         //�ж�Ƕ�ײ���
	OS_EXIT_CRITICAL();     //���ж�
	
    if (DMA_GetITStatus(DMA1_FLAG_TC4))
    {
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);  //�������ڷ�������ж�

        DMA_ClearITPendingBit(DMA1_IT_TC4);
		DMA_ClearITPendingBit(DMA1_IT_GL4); //���ȫ���жϱ�־

        DMA_Cmd(DMA1_Channel4, DISABLE);        //�ر�DMA
    }
	
	OSIntExit();
}

#endif
