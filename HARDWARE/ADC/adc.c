#include "adc.h"
#include "delay.h"

#include "ctrl.h"
#include "main.h"

void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                  //����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1,ENABLE);	
 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;		           //PC0  ADC1_IN0   DMA1_Channel1ͨ��1
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR)); //ADC��ַ  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_ADC_Buf;        //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //����(�����赽�ڴ�)    ��ת����adcֵ���浽�ڴ���
	DMA_InitStructure.DMA_BufferSize = 4;                               //�������ݵĴ�С  4��ͨ����ֵ  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //�ڴ��ַ������������һ�����֣�������Ҫ����4����ֵ��������Ҫ������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ;  //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;          //�ڴ����ݵ�λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ;       //DMAģʽ��ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_High ;    //���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            //��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);            //����DMA1_Channel1
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);         //DMAͨ��1��������ж�
	DMA_Cmd(DMA1_Channel1,ENABLE);
 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;      //����ADCģʽ
	ADC_InitStructure.ADC_ScanConvMode =  ENABLE;           //ɨ�跽ʽ����Ҫ����Զ���ͨ�����ԣ�Ҳ����˵���Ƿ��ж���ͨ����
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;     //��������ת��ģʽ 
	ADC_InitStructure.ADC_ExternalTrigConv =  ADC_ExternalTrigConv_None; //ʹ���ⲿ����ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //�ɼ������Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 4;                 //Ҫת����ͨ����Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//�����ǹ���ͨ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);//����ADC1ͨ��10Ϊ55.5����������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_28Cycles5);//����ADC1ͨ��11Ϊ55.5����������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_28Cycles5);//����ADC1ͨ��12Ϊ55.5����������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_28Cycles5);//����ADC1ͨ��13Ϊ55.5����������
//  ADC_DiscModeChannelCountConfig(ADC1, 1);
//	ADC_DiscModeCmd(ADC1, ENABLE);
	//������ע��ͨ��������ע��ͨ�����ó�TIM1��CCR4�¼�����     
//	ADC_InjectedSequencerLengthConfig(ADC1, 1);
//	ADC_InjectedChannelConfig(ADC1,ADC_Channel_TempSensor,1,ADC_SampleTime_7Cycles5);   //CURRENT  �¶�
//	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);         //�������ʹ��time1��CCR4�¼�����������
//	ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
	
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
 
	ADC_ResetCalibration(ADC1);//��λУ׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�У׼�Ĵ�����λ���
 
	ADC_StartCalibration(ADC1);//ADCУ׼
	while(ADC_GetCalibrationStatus(ADC1));//�ȴ�У׼���
}




