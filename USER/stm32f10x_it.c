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


static  int  PlaceOut,SpeedOut,MeasuSpeed;        //速度环输出到电流环    静态全局变量
u32 time0 = 2500000,time1 = 2500000,time2 = 2500000;
volatile bool   g_ADC_OK = FALSE;   			  //ADC采集成功标志


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
  /* Go to infinite loop when Hard Fault exception occurs 发生硬故障异常时转到无限循环*/
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
		DMA_ClearITPendingBit(DMA1_IT_GL1); //清除全部中断标志
		PwmOut(SpeedOut);	
		g_ADC_OK=TRUE;
	}
}

void TIM1_CC_IRQHandler(void)       //定时器1输出比较中断中采集电流
{ 
	TIM1->SR&=0;           //清中断标志
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	  调用ADC采集函数
}

void TIM1_BRK_IRQHandler(void)
{

}

void TIM3_IRQHandler(void)
{
	static u8  PhaseCnt;            //换相次数
	static u8  time_over; 			//定时器溢出计数，为什么要设置为静态static，因为中断一旦退出，局部变量会被释放
	if(TIM3->SR&0x0010)       		//产生比较/捕获事件４中断计数器溢出   TIM_IT_CC4捕获/比较4中断   10ms中断一次
	{
		TIM3->SR&=0xffef;	    	//清中断标志  CC4IF
		
		time_over = time_over>100?100:++time_over;/*溢出清零　堵转计数*/
		if(time_over%10 == 0)   bldc_dev.motor_state = STOP;
		switch(PhaseCnt%3)
		{
			case 0:    time0=((10000*time_over));  break;      //hall1到hall2换相时间
			case 1:    time1=((10000*time_over));  break;      //hall2到hall3换相时间
			case 2:    time2=((10000*time_over));  break;      //hall3到hall1换相时间    3个时间相加为电机转动45度的时间
			default:   break;
		}
		TIM3->CNT=0;                    //清TIM3的计数器 

//		GPIOB->ODR ^= (1<<12); 
	}
	if(TIM3->SR&0x0040)      		    //TIM_IT_Trigger触发中断标记     电机每次转过15度，触发一次捕获中断，计算出时间
	{
		TIM3->SR &= 0xffbf;        	    //清中断标志
		
		bldc_dev.motor_direction = huanxiang();          //调用换向函数，启动 ，返回速度的方向
		
		switch(PhaseCnt%3)
		{
			case 0:    time0=(TIM3->CCR1+(10000*time_over));  break;      //hall1到hall2换相时间    CCR1包含了由上一次输入捕获1事件(IC1)传输的计数器值。
			case 1:    time1=(TIM3->CCR1+(10000*time_over));  break;      //hall2到hall3换相时间
			case 2:    time2=(TIM3->CCR1+(10000*time_over));  break;      //hall3到hall1换相时间    3个时间相加为电机转动45度的时间
			default:   break;
		}
		PhaseCnt++;
		if(PhaseCnt == SERIES)   PhaseCnt = 0;
		
		time_over = 0;			    	//溢出计数清零
		TIM3->CNT=0;                    //清TIM3的计数器 
	}     		
}

void TIM4_IRQHandler(void)             //4ms中断1次
{
	static u8   iCnt;

	static u16  valueL_buf[4],valueR_buf[4];
	
	if(TIM4->SR&0x0001) 
	{
		TIM4->SR&=0xfffe;	    		//清中断标志  CC4IF
		
		SensorL_value = filter(valueL_buf,iCnt,g_ADC_Buf[1]);
		SensorR_value = filter(valueR_buf,iCnt,g_ADC_Buf[2]);
		iCnt++;
		if(iCnt == 4)   iCnt = 0;
			
		if(g_ADC_OK == TRUE)            //100us电流采集成功
		{
			g_ADC_OK = FALSE;   
			
			
			
//根据传感器计算位置环输出PlaceOut    位置环输出PlaceOut作为速度环输入，加减速控制
			PlaceOut = PlaceOutHandle(SensorL_value,SensorR_value);                           //反向信号，延时
			
			MeasuSpeed = (int)NUMBERTURN/(time0+time1+time2);  
			MeasuSpeed = bldc_dev.motor_direction==CCW?MeasuSpeed:-MeasuSpeed;  //判断速度方向
			
//不启动校验行程时,速度PID,100ms反馈一次，电机负载变动，如果不调整电流，会影响电机速度。校准时候，不使用速度pid		



			
			bldc_dev.order_state = PlaceOut==0?STOP:RUN;           //输出1，表示指令停止；输出0，表示指令启动
			SpeedOut = (TravelCal.CaliFlag==0||TravelCal.CaliStep==2)?SpeedPID(PlaceOut,MeasuSpeed):PlaceOut;
		}
	}
}



int filter(u16 *value_buf,u8 i,u16 ADC_Buf)                 //求滑动平均值
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
