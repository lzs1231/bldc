
void UART4_IRQHandler(void)
{
	uint16_t i;
	uint16_t Data_Len;
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)  //���Ϊ�����ж�
	{ 
		DMA_Cmd(DMA2_Channel3, DISABLE);
		
		Data_Len=512-DMA_GetCurrDataCounter(DMA2_Channel3);
		
		USART_PutStr(USART1,UART4_Rx_buffer,Data_Len);				
		UART4_Rx_num=0;
		DMA_ClearFlag(DMA2_FLAG_GL3 | DMA2_FLAG_TC3 | DMA2_FLAG_TE3 | DMA2_FLAG_HT3);     //���־
		DMA2_Channel3->CNDTR = 512;          //��װ��
		DMA_Cmd(DMA2_Channel3, ENABLE);      //������,�ؿ�DMA
		
		i = UART4->SR;                       //��SR���DR���IDLE
		i = UART4->DR;
		if(i)			i=0;
		if(Data_Len)	Data_Len=0; 
	}
	if(USART_GetITStatus(UART4, USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)       //����
	{ 
		USART_ClearITPendingBit(UART4, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
	USART_ClearITPendingBit(UART4, USART_IT_TC);
	USART_ClearITPendingBit(UART4, USART_IT_IDLE);
}



void DMA2_Channel3_IRQHandler(void)
{
	USART_PutStr(USART1," DMA23:\r\n",9);
	
	DMA_ClearITPendingBit(DMA2_IT_TC3);
	DMA_ClearITPendingBit(DMA2_IT_TE3);
	
	DMA_Cmd(DMA2_Channel3, DISABLE);     //�ر�DMA,��ֹ�������������
	DMA2_Channel3->CNDTR = 512;       //��װ��
	DMA_Cmd(DMA2_Channel3, ENABLE);     //������,�ؿ�DMA
}

