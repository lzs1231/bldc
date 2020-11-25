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

#define PosNeg(a,b) ((a^b)<0?1:0)     //������ͬ����0

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
	State curState;		//��ǰ״̬
	EventID eventId;	//�¼�ID
	State nextState;	//�¸�״̬
	Action action;		//�������
}StateTransform;

typedef struct{
	State state;    
	int transNum;
	StateTransform* transform;
}StateMachine;

EventID			inputEvent;
StateMachine	stateMachine;

static  int  PlaceOut,SpeedOut,MeasuSpeed;        //�ٶȻ������������    ��̬ȫ�ֱ���
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
		
		time_over = time_over>5?5:++time_over;/*������㡡��ת����*/
		if(time_over == 5)   bldc_dev.motor_state = STOP;
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
	static int  LastPlaceOut,SpeedIn;
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
			switch(gWorkMode)
			{
				case 0:	
					/*��ǰ״̬Ϊ�ֶ�ģʽ*/
					inputEvent = event_1;	    		 //�¼�1
					
					
					if(stateMachine.state != state_1b && PosNeg(LastPlaceOut,PlaceOut))                 	 //����״̬
					{
						stateMachine.state = state_1a;
						SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
						LastPlaceOut = SpeedIn;
						if(SpeedIn == 0)		    	 //����״̬����
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
					if(LongPortFun[PusherLeft]!=0)    	  			//Զ�̵㶯��
					{
						/*��ǰ״̬Ϊ�Զ�Զ�̵㶯��*/
						inputEvent = event_2;			 //�¼�2
						
				
						if(stateMachine.state != state_2b && PosNeg(LastPlaceOut,PlaceOut))                 	 //����״̬
						{
							stateMachine.state = state_2a;
							SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
							LastPlaceOut = SpeedIn;
							if(SpeedIn == 0)		    	 //����״̬����
							{
								stateMachine.state = state_2b;
							}
						}else{
							stateMachine.state = state_2b; 
							SpeedIn = T_Mplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
							LastPlaceOut = SpeedIn;
						}
					}else{	
						/*��ǰ״̬Ϊ�Զ�Զ�̵㶯��*/
						inputEvent = event_3;			 //�¼�3
						
						
						if(stateMachine.state != state_3b && PosNeg(LastPlaceOut,PlaceOut))                 	 //����״̬
						{
							stateMachine.state = state_3a;
							SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
							LastPlaceOut = SpeedIn;
							if(SpeedIn == 0)		    	 //����״̬����
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
					if(LongPortFun[PusherLeft]!=0 || ClickButton!=0){    	//Զ�̵㶯��
						/*��ǰ״̬Ϊ���ĵ㶯��*/
						inputEvent = event_4;			 //�¼�4
						
						
						if(stateMachine.state != state_4b && PosNeg(LastPlaceOut,PlaceOut))                 	 //����״̬
						{
							stateMachine.state = state_4a;
							SpeedIn = decspeed(LastPlaceOut,gMFlexDec<<1);
							LastPlaceOut = SpeedIn;
							if(SpeedIn == 0)		    	 //����״̬����
							{
								stateMachine.state = state_4b;
							}
						}else{
							stateMachine.state = state_4b; 
							SpeedIn = T_Mplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
							LastPlaceOut = SpeedIn;
						}
					}else{	
						/*��ǰ״̬Ϊ���ĵ㶯��*/						
						inputEvent = event_5;			//�¼�5
						
						
						if(stateMachine.state != state_5b && PosNeg(LastPlaceOut,PlaceOut))                 	 //����״̬
						{
							stateMachine.state = state_5a;
							SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
							LastPlaceOut = SpeedIn;
							if(SpeedIn == 0)		    	 //����״̬����
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

//			runStateMachine(&stateMachine, inputEvent);   //���ݴ����¼��뵱ǰ״̬��ִ�ж�Ӧ���ܺ������л���һ��״̬��
//				
//			runState(&stateMachine,inputEvent);
			
			bldc_dev.order_state = SpeedIn==0?STOP:RUN;           //���1����ʾָ��ֹͣ�����0����ʾָ������
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

int filter(u16 *value_buf,u8 i,u16 ADC_Buf)                 //�󻬶�ƽ��ֵ
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


//����Ƿ�����ź��������ֹͣ4MS������������
int reversal(int P,u8 TimeCnt)
{
	int Dir_Deviation;                        //���ƫ��ķ���
	static int Last_Dir_Deviation;            //��¼��һ�����ƫ��ķ���,���ڱȽ����ƫ���Ƿ�ı䷽��
	static u8 Number_Pwm;
	
	//��ȡ�����ѹ�ķ���
    Dir_Deviation = GetDirection(P);
	
	if(Dir_Deviation!=Last_Dir_Deviation) 		 //�����ź�
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
