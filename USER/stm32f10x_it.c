/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "sys.h"
#include "ctrl.h"
#include "main.h"
#include "handle.h"
#include "lcdtest.h"
#include "pid_control.h"
#include "wdg.h"


static  int  PlaceOut,SpeedOut,MeasuSpeed;        //�ٶȻ������������    ��̬ȫ�ֱ���
u32 time0 = 2500000,time1 = 2500000,time2 = 2500000;
volatile bool   g_ADC_OK = FALSE;   			  //ADC�ɼ��ɹ���־


int filter(u16 *value_buf,u8 i,u16 ADC_Buf);


void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
	if (CoreDebug->DHCSR & 1)
   {
      //check C_DEBUGEN == 1 -> Debugger Connected
      __breakpoint(0);  // halt program execution here
   }
  /* Go to infinite loop when Hard Fault exception occurs ����Ӳ�����쳣ʱת������ѭ��*/
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}
 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL1); //���ȫ���жϱ�־
		PwmOut(SpeedOut);	
		g_ADC_OK=TRUE;
	}
}

void TIM1_CC_IRQHandler(void)       //��ʱ��1����Ƚ��ж��вɼ�����
{ 
	TIM1->SR&=0;           //���жϱ�־
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	  ����ADC�ɼ�����
}

void TIM1_BRK_IRQHandler(void)
{

}

void TIM3_IRQHandler(void)
{
	static u8  PhaseCnt;            //�������
	static u8  time_over; 			//��ʱ�����������ΪʲôҪ����Ϊ��̬static����Ϊ�ж�һ���˳����ֲ������ᱻ�ͷ�
	if(TIM3->SR&0x0010)       		//�����Ƚ�/�����¼����жϼ��������   TIM_IT_CC4����/�Ƚ�4�ж�   10ms�ж�һ��
	{
		TIM3->SR&=0xffef;	    	//���жϱ�־  CC4IF
		
		time_over = time_over>100?100:++time_over;/*������㡡��ת����*/
		if(time_over%10 == 0)   bldc_dev.motor_state = STOP;
		switch(PhaseCnt%3)
		{
			case 0:    time0=((10000*time_over));  break;      //hall1��hall2����ʱ��
			case 1:    time1=((10000*time_over));  break;      //hall2��hall3����ʱ��
			case 2:    time2=((10000*time_over));  break;      //hall3��hall1����ʱ��    3��ʱ�����Ϊ���ת��45�ȵ�ʱ��
			default:   break;
		}
		TIM3->CNT=0;                    //��TIM3�ļ����� 

//		GPIOB->ODR ^= (1<<12); 
	}
	if(TIM3->SR&0x0040)      		    //TIM_IT_Trigger�����жϱ��     ���ÿ��ת��15�ȣ�����һ�β����жϣ������ʱ��
	{
		TIM3->SR &= 0xffbf;        	    //���жϱ�־
		
		bldc_dev.motor_direction = huanxiang();          //���û����������� �������ٶȵķ���
		
		switch(PhaseCnt%3)
		{
			case 0:    time0=(TIM3->CCR1+(10000*time_over));  break;      //hall1��hall2����ʱ��    CCR1����������һ�����벶��1�¼�(IC1)����ļ�����ֵ��
			case 1:    time1=(TIM3->CCR1+(10000*time_over));  break;      //hall2��hall3����ʱ��
			case 2:    time2=(TIM3->CCR1+(10000*time_over));  break;      //hall3��hall1����ʱ��    3��ʱ�����Ϊ���ת��45�ȵ�ʱ��
			default:   break;
		}
		PhaseCnt++;
		if(PhaseCnt == SERIES)   PhaseCnt = 0;
		
		time_over = 0;			    	//�����������
		TIM3->CNT=0;                    //��TIM3�ļ����� 
	}     		
}

void TIM4_IRQHandler(void)             //4ms�ж�1��
{
	static u8   iCnt;

	static u16  valueL_buf[4],valueR_buf[4];
	
	if(TIM4->SR&0x0001) 
	{
		TIM4->SR&=0xfffe;	    		//���жϱ�־  CC4IF
		
		SensorL_value = filter(valueL_buf,iCnt,g_ADC_Buf[1]);
		SensorR_value = filter(valueR_buf,iCnt,g_ADC_Buf[2]);
		iCnt++;
		if(iCnt == 4)   iCnt = 0;
			
		if(g_ADC_OK == TRUE)            //100us�����ɼ��ɹ�
		{
			g_ADC_OK = FALSE;   
			
			
			
//���ݴ���������λ�û����PlaceOut    λ�û����PlaceOut��Ϊ�ٶȻ����룬�Ӽ��ٿ���
			PlaceOut = PlaceOutHandle(SensorL_value,SensorR_value);                           //�����źţ���ʱ
			
			MeasuSpeed = (int)NUMBERTURN/(time0+time1+time2);  
			MeasuSpeed = bldc_dev.motor_direction==CCW?MeasuSpeed:-MeasuSpeed;  //�ж��ٶȷ���
			
//������У���г�ʱ,�ٶ�PID,100ms����һ�Σ�������ر䶯�������������������Ӱ�����ٶȡ�У׼ʱ�򣬲�ʹ���ٶ�pid		



			
			bldc_dev.order_state = PlaceOut==0?STOP:RUN;           //���1����ʾָ��ֹͣ�����0����ʾָ������
			SpeedOut = (TravelCal.CaliFlag==0||TravelCal.CaliStep==2)?SpeedPID(PlaceOut,MeasuSpeed):PlaceOut;
		}
	}
}



int filter(u16 *value_buf,u8 i,u16 ADC_Buf)                 //�󻬶�ƽ��ֵ
{
	char count;
	int  sum=0;

	value_buf[i] = ADC_Buf;
	
	for(count=0;count<4;count++)
		sum += value_buf[count];
	
	return (int)(sum/4);
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
