#include "adc.h"
#include "delay.h"

#include "ctrl.h"
#include "main.h"

void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                  //配置ADC时钟，为PCLK2的6分频，即12MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1,ENABLE);	
 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;		           //PC0  ADC1_IN0   DMA1_Channel1通道1
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR)); //ADC地址  (uint32_t)(&(ADC1->DR))
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_ADC_Buf;        //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //方向(从外设到内存)    将转换的adc值保存到内存中
	DMA_InitStructure.DMA_BufferSize = 4;                               //传输内容的大小  4条通道的值  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //内存地址增长，定义了一个数字，里面需要保存4个数值，所以需要增长；
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ;  //外设数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;          //内存数据单位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ;       //DMA模式：循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High ;    //优先级：高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            //禁止内存到内存的传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);            //配置DMA1_Channel1
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);         //DMA通道1传输完成中断
	DMA_Cmd(DMA1_Channel1,ENABLE);
 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;      //独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode =  ENABLE;           //扫描方式，主要是针对多条通道而言，也就是说你是否有多条通道。
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;     //开启连续转换模式 
	ADC_InitStructure.ADC_ExternalTrigConv =  ADC_ExternalTrigConv_None; //使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 4;                 //要转换的通道数目
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//下面是规则通道
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);//配置ADC1通道10为55.5个采样周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_28Cycles5);//配置ADC1通道11为55.5个采样周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_28Cycles5);//配置ADC1通道12为55.5个采样周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_28Cycles5);//配置ADC1通道13为55.5个采样周期
//  ADC_DiscModeChannelCountConfig(ADC1, 1);
//	ADC_DiscModeCmd(ADC1, ENABLE);
	//下面是注入通道，这里注入通道设置成TIM1的CCR4事件触发     
//	ADC_InjectedSequencerLengthConfig(ADC1, 1);
//	ADC_InjectedChannelConfig(ADC1,ADC_Channel_TempSensor,1,ADC_SampleTime_7Cycles5);   //CURRENT  温度
//	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);         //这个可以使用time1的CCR4事件来触发采样
//	ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
	
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
 
	ADC_ResetCalibration(ADC1);//复位校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//等待校准寄存器复位完成
 
	ADC_StartCalibration(ADC1);//ADC校准
	while(ADC_GetCalibrationStatus(ADC1));//等待校准完成
}




