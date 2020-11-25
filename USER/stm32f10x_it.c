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

#define PosNeg(a,b) ((a^b)<0?1:0)     //符号相同返回0

typedef enum {
	state_1a=1,
	state_1b,
	state_2a,
	state_2b,
	state_3a,
	state_3b,
	state_4a,
	state_4b,
	state_5a,
	state_5b,
}State;


typedef enum{
	event_1=1,
	event_2,
	event_3,
	event_4,
	event_5,
}EventID;

typedef int (*Action)(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);

typedef struct {
	State curState;		//当前状态
	EventID eventId;	//事件ID
	State nextState;	//下个状态
	Action action;		//具体表现
}StateTransform;

typedef struct{
	State state;    
	int transNum;
	StateTransform* transform;
}StateMachine;

EventID			inputEvent;
StateMachine	stateMachine;

static  int  PlaceOut,SpeedOut,MeasuSpeed;        //速度环输出到电流环    静态全局变量
u32 time0 = 500000,time1 = 500000,time2 = 500000;


int decspeed(int SpeedIn,u16 FlexDec);
int T_Mplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);
int T_Aplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);
int T_Cplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);
int filter(u16 *value_buf,u8 i,u16 ADC_Buf);
int reversal(int P,u8 TimeCnt);



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
		
		time_over = time_over>5?5:++time_over;/*溢出清零　堵转计数*/
		if(time_over == 5)   bldc_dev.motor_state = STOP;
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
	static int  LastPlaceOut,SpeedIn;
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
			switch(gWorkMode)
			{
				case 0:	
					/*当前状态为手动模式*/
					inputEvent = event_1;	    		 //事件1
					
					
					if(stateMachine.state != state_1b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
					{
						stateMachine.state = state_1a;
						SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
						LastPlaceOut = SpeedIn;
						if(SpeedIn == 0)		    	 //减速状态结束
						{
							stateMachine.state = state_1b;
						}
					}else{
						stateMachine.state = state_1b;    
						SpeedIn = T_Mplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
						LastPlaceOut = SpeedIn;
					}
				break;
				case 1:			
					if(LongPortFun[PusherLeft]!=0)    	  			//远程点动打开
					{
						/*当前状态为自动远程点动开*/
						inputEvent = event_2;			 //事件2
						
				
						if(stateMachine.state != state_2b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
						{
							stateMachine.state = state_2a;
							SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
							LastPlaceOut = SpeedIn;
							if(SpeedIn == 0)		    	 //减速状态结束
							{
								stateMachine.state = state_2b;
							}
						}else{
							stateMachine.state = state_2b; 
							SpeedIn = T_Mplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
							LastPlaceOut = SpeedIn;
						}
					}else{	
						/*当前状态为自动远程点动关*/
						inputEvent = event_3;			 //事件3
						
						
						if(stateMachine.state != state_3b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
						{
							stateMachine.state = state_3a;
							SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
							LastPlaceOut = SpeedIn;
							if(SpeedIn == 0)		    	 //减速状态结束
							{
								stateMachine.state = state_3b;
							}
						}else{
							stateMachine.state = state_3b; 
							SpeedIn = T_Aplan(reversal(PlaceOut,5), LastPlaceOut, gAFlexAcc, gAFlexDec);
							LastPlaceOut = SpeedIn;
						}
					}
				break;
				
				case 2:	
					if(LongPortFun[PusherLeft]!=0 || ClickButton!=0){    	//远程点动打开
						/*当前状态为中心点动开*/
						inputEvent = event_4;			 //事件4
						
						
						if(stateMachine.state != state_4b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
						{
							stateMachine.state = state_4a;
							SpeedIn = decspeed(LastPlaceOut,gMFlexDec<<1);
							LastPlaceOut = SpeedIn;
							if(SpeedIn == 0)		    	 //减速状态结束
							{
								stateMachine.state = state_4b;
							}
						}else{
							stateMachine.state = state_4b; 
							SpeedIn = T_Mplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
							LastPlaceOut = SpeedIn;
						}
					}else{	
						/*当前状态为中心点动关*/						
						inputEvent = event_5;			//事件5
						
						
						if(stateMachine.state != state_5b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
						{
							stateMachine.state = state_5a;
							SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
							LastPlaceOut = SpeedIn;
							if(SpeedIn == 0)		    	 //减速状态结束
							{
								stateMachine.state = state_5b;
							}
						}else{
							stateMachine.state = state_5b; 
							SpeedIn = T_Cplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
							LastPlaceOut = SpeedIn;
						}						
					}						
				break;
				default:		
				break;
			}

//			runStateMachine(&stateMachine, inputEvent);   //根据触发事件与当前状态，执行对应功能函数并切换下一个状态。
//				
//			runState(&stateMachine,inputEvent);
			
			bldc_dev.order_state = SpeedIn==0?STOP:RUN;           //输出1，表示指令停止；输出0，表示指令启动
			SpeedOut = (TravelCal.CaliFlag==0||TravelCal.CaliStep==2)?SpeedPID(SpeedIn,MeasuSpeed):PlaceOut;
		}
	}
}

int decspeed(int LastPlaceOut,u16 FlexDec)
{
	int SpeedIn;
	
	if(LastPlaceOut > 0)	
	{
		SpeedIn = LastPlaceOut-(FlexDec<<1);
		if(SpeedIn < 0)	SpeedIn = 0;
	}else if(LastPlaceOut < 0)	{
		SpeedIn = LastPlaceOut+(FlexDec<<1);
		if(SpeedIn > 0)	SpeedIn = 0;
	}
	return SpeedIn;
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

int T_Mplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec)
{
	int SpeedIn,PEk;
	
	PEk = PlaceOut-LastPlaceOut;
	if((PlaceOut>0)&&(PEk > 0))
	{	
		SpeedIn = LastPlaceOut+acc;
		if(SpeedIn>PlaceOut)   SpeedIn = PlaceOut;
	}else if((PlaceOut>=0)&&(PEk < 0)){
		SpeedIn = LastPlaceOut-dec;
		if(SpeedIn<PlaceOut)  SpeedIn = PlaceOut;
	}else if((PlaceOut<0)&&(PEk < 0)){
		SpeedIn = LastPlaceOut-acc;
		if(SpeedIn<PlaceOut)   SpeedIn = PlaceOut;
	}else if((PlaceOut<=0)&&(PEk > 0)){
		SpeedIn = LastPlaceOut+dec;
		if(SpeedIn>PlaceOut)  SpeedIn = PlaceOut;
	}else{
		SpeedIn = PlaceOut;
	}
	
	return SpeedIn;
}

int T_Aplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec)
{
	int SpeedIn,PEk;
	
	PEk = PlaceOut-LastPlaceOut;
	
	if((PlaceOut>0)&&(PEk > 0))
	{	
		SpeedIn = LastPlaceOut+acc;
		if(SpeedIn>PlaceOut)   SpeedIn = PlaceOut;
	}else if((PlaceOut<0)&&(PEk < 0)){
		SpeedIn = LastPlaceOut-acc;
		if(SpeedIn<PlaceOut)   SpeedIn = PlaceOut;
	}else{
		SpeedIn = PlaceOut;
	}
	
	return SpeedIn;
}

int T_Cplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec)
{
	int SpeedIn,PEk;
	
	PEk = PlaceOut-LastPlaceOut;
	
	if((PlaceOut>0)&&(PEk > 0))
	{	
		SpeedIn = LastPlaceOut+acc;
		if(SpeedIn>PlaceOut)   SpeedIn = PlaceOut;
	}else if((PlaceOut<0)&&(PEk < 0)){
		SpeedIn = LastPlaceOut-acc;
		if(SpeedIn<PlaceOut)   SpeedIn = PlaceOut;
	}else{
		SpeedIn = PlaceOut;
	}
	
	return SpeedIn;
}


//如果是反向的信号输入就先停止4MS输出再启动电机
int reversal(int P,u8 TimeCnt)
{
	int Dir_Deviation;                        //输出偏差的方向
	static int Last_Dir_Deviation;            //记录上一次输出偏差的方向,用于比较输出偏差是否改变方向
	static u8 Number_Pwm;
	
	//提取输入电压的方向
    Dir_Deviation = GetDirection(P);
	
	if(Dir_Deviation!=Last_Dir_Deviation) 		 //反向信号
	{
		if(Number_Pwm < TimeCnt)
		{
			Number_Pwm++;
			return 0;
		}else{
			Number_Pwm=0;
			Last_Dir_Deviation=Dir_Deviation;
			return P;
		}
	}else{
		Number_Pwm=0;				
		Last_Dir_Deviation=Dir_Deviation;
		return P;
	}
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
