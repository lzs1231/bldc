#include "includes.h"
#include "ctrl.h"
#include "delay.h"
#include "handle.h"

#define IR1LIN  PAout(8)          //IR2103_1�ĵͶ�����,�߶�ΪPWM����
#define IR2LIN  PAout(9) 		  //IR2103_2�ĵͶ�����,�߶�ΪPWM����
#define IR3LIN  PAout(10)         //IR2103_3�ĵͶ�����,�߶�ΪPWM����

MotorDev bldc_dev={1000,STOP,STOP,CW,CW,0,0};

u8 HallA_data = 0,HallB_data = 0,HallC_data = 0; 


/**********************************************************************
* Description    : �Զ�ʱ��1�Ͷ�ʱ��3��GPIO����
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void BLDC_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
          
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;  //TIM1���     GPIOA8ͨ��CH1  GPIOA9ͨ��CH2  GPIOA10ͨ��CH3  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //������� 	��ODR����                                         
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
                         
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//TIM1���     GPIOB13����ͨ��CH1N  GPIOB14����ͨ��CH2N  GPIOB15����ͨ��CH3N  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //�����������    ����ODR����                                       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;              //TIM3�Ļ�������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
        
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                         //TIM1_BKIN
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);         
}

/**********************************************************************
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void BLDC_TIM1Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;                       //�����ṹ���������
	TIM_OCInitTypeDef        TIM_OCInitStructure;                         //����ṹ���������
	TIM_BDTRInitTypeDef      TIM_BDTRInitStructure;                       //����ɲ���ṹ���������

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	TIM_DeInit(TIM1);
	/*TIM������ʼ��*/
	TIM_TimeBaseStructure.TIM_Prescaler = 3-1;                                //Ԥ��Ƶֵ   72/3=24MHz
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned2;   //����������ģʽ,����Ƚϱ�־λֻ���ڱȽ������ϼ��㱻����
	TIM_TimeBaseStructure.TIM_Period = 1200 - 1;                              //�Զ�����ֵARR  PWM 24M/(1200*2)=10KHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                              //ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                          //ѭ������     tim1�������䲻Ȼû��Ӧ	    
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                         //TIM���ͨ����ʼ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;            //�ر�OC�������Ӧ����  Ϊʲô�رգ�   �ȵ���Ҫ��ʱ���
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;          //�ر�OC�������        
	TIM_OCInitStructure.TIM_Pulse =600;                                      //CCR���PWMռ�ձ�1200/1800=2/3  _����_����_
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;         
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;          //TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;          
	TIM_OC1Init(TIM1,&TIM_OCInitStructure); 

	TIM_OCInitStructure.TIM_Pulse =600;
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse =600;
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;                    //TIM���ͨ��4��ʼ������������ADע�����
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;                   
	TIM_OCInitStructure.TIM_Pulse =10; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;           
	TIM_OC4Init(TIM1,&TIM_OCInitStructure); 

	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;        //����ɲ����ʼ��
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
	TIM_BDTRInitStructure.TIM_DeadTime = 120;
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;         //��򿪣������������״̬���ң�������
	TIM_BDTRInitStructure.TIM_BreakPolarity =  TIM_BreakPolarity_Low ;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);

	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);  //ʹ�ܲ���ȽϼĴ���Ԥװ�أ�ͨ��1��
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);  //ʹ�ܲ���ȽϼĴ���Ԥװ�أ�ͨ��2��
	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);  //ʹ�ܲ���ȽϼĴ���Ԥװ�أ�ͨ��3��
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);           //PWM���

	TIM_ClearITPendingBit( TIM1, TIM_IT_CC4);
	TIM_Cmd(TIM1,ENABLE);
	TIM_ITConfig(TIM1, TIM_IT_CC4 ,ENABLE);
}

void BLDC_TIM4Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;              //�����ṹ���������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	TIM_DeInit(TIM4);
    /*TIM������ʼ��*/
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;                    //Ԥ��Ƶֵ   72/72=1MHz
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 4000-1;                       //�Զ�����ֵARR  ����1us*1000=1ms
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);    
	
	TIM_ClearITPendingBit( TIM4, TIM_IT_Update);
	TIM_Cmd(TIM4,ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);////ʹ�ܻ���ʧ��ָ����TIM�ж�   TIM �����ж�Դ
}


/**********************************************************************
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void BLDC_TIM3Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;              //�����ṹ���������
	TIM_ICInitTypeDef  TIM_ICInitStructure;                      //����ṹ�����
	TIM_OCInitTypeDef  TIM_OCInitStructure;                      //����ṹ���������
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3 ,ENABLE ); 
	TIM_DeInit(TIM3);
    /*TIM������ʼ��*/
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;                    //Ԥ��Ƶֵ   72/72=1MHz
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 10000-1;                       //�Զ�����ֵARR  ����1us*10000=10ms
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);     

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;            //ѡ��ͨ��1       ͨ��������룬����3�������ź�
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //���������ز���  
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC;  //����ͨ��Ϊ���룬��ӳ�䵽���� ��ӳ�䵽TRC
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       //���벶��Ԥ��Ƶֵ
	TIM_ICInitStructure.TIM_ICFilter = 10;                      //�����˲�����������
	TIM_ICInit(TIM3, &TIM_ICInitStructure);                     //����ͨ������
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;                   //TIM���ͨ����ʼ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;             
	TIM_OCInitStructure.TIM_Pulse =10000;                       //�����Ƚϲ����жϣ�����������ֵ=10000ʱ�ͻᴥ���ж�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      
	TIM_OC4Init(TIM3,&TIM_OCInitStructure);

	TIM_SelectHallSensor(TIM3,ENABLE);                          //ʹ��TIMx�Ļ����������ӿ�,3��IO�ھͿ������ӵ�һ��ͨ����TIM_Channel_1�����в���

	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1F_ED);               //���봥��Դѡ��     TI1�ı��ؼ����

	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);             //��ģʽѡ��

	TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);//����ģʽѡ��        

	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_OC2Ref);       //ѡ���������ģʽ(TRGO��)  ͨ��OC2
	
	TIM3->SR &= 0xffef;	    		//���жϱ�־  CC4IF
	TIM3->SR &= 0xffbf;        	    //���жϱ�־
	TIM_Cmd(TIM3,ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Trigger|TIM_IT_CC4, ENABLE);   //����ʱ���ж�   TIM_IT_Trigger�жϼ�¼ʱ��   TIM_IT_CC4 �жϼ����ٶȻ���                     
}

void BLDCSwitchP(u8 HallData)        //��ת   ���
{
	switch(HallData)                                 //����ת��λ�ã�����CCER�����λ��ת����ƫ����
	{
		case 0x05:{                   
			IR2LIN=1;
			IR3LIN=1;
			TIM1->CCER=0x3040;     //3 2
			IR1LIN=0;	
		};break;
		
		case 0x04:{
			IR1LIN=1;
			IR2LIN=1;
			TIM1->CCER=0x3040;     //3 6  
			IR3LIN=0;
		};break;  
		case 0x06:{
			IR1LIN=1;
			IR2LIN=1;
			TIM1->CCER=0x3004;     //1 6
			IR3LIN=0;
		};break;
		
		case 0x02:{
			IR1LIN=1;
			IR3LIN=1;
			TIM1->CCER=0x3004;     //1 4  
			IR2LIN=0;
		};break;
		case 0x03:{
			IR1LIN=1;
			IR3LIN=1;	
			TIM1->CCER=0x3400;     //5 4
			IR2LIN=0;	
		};break;
		case 0x01:{
			IR2LIN=1;
			IR3LIN=1;
			TIM1->CCER=0x3400;     //5 2
			IR1LIN=0;
		};break;
		
		default:  TIM1->CCER=0x3000;   IR1LIN=1;IR2LIN=1;IR3LIN=1;break;
	}                    
}

void BLDCSwitchN(u8 HallData)        //��ת   ����
{
	switch(HallData)                 //����ת��λ�ã�����CCER�����λ��ת����ƫ����
	{
		case 0x05:{                   //101    
			IR1LIN=1;IR3LIN=1;   //�ȹر��¹����
			TIM1->CCER=0x3004;        //1,4     A+,B-  0x3034   ��Ȼ����Ϲ�PWM���
			IR2LIN=0;
		};break;
		
		case 0x04:{                   //100
			IR1LIN=1;IR3LIN=1;	
			TIM1->CCER=0x3400;        //4,5     C+,B-  0x3430    
			IR2LIN=0;			
		};break; 
		
		case 0x06:{                   //110
			IR2LIN=1;IR3LIN=1;
			TIM1->CCER=0x3400;        //2,5     C+,A-
			IR1LIN=0;	
		};break;
		
		case 0x02:{                   //010
			IR2LIN=1;IR3LIN=1;
			TIM1->CCER=0x3040;        //2,3     B+,A-   
			IR1LIN=0;					
		};break;
		
		case 0x03:{                   //011
			IR1LIN=1;IR2LIN=1;
			TIM1->CCER=0x3040;        //3,6     B+,C-   
			IR3LIN=0;	
		};break;
		
		case 0x01:{                   //001
			IR1LIN=1;IR2LIN=1;
			TIM1->CCER=0x3004;        //1,6     A+,C- 
			IR3LIN=0;	
		};break;
		
		default:TIM1->CCER=0x3000;   IR1LIN=1;IR2LIN=1;IR3LIN=1; break;   //����PWM��������¹�ȫ���ر�
	} 
}

/**********************************************************************
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
MOTOR_DIR huanxiang(void)
{
	char motor_dir;
	u8 HallData = 0; 
	HallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);      //��ת��λ��    �ϳ�GPIOC8|GPIOC7|GPIOC6 

	if(HallData>=1&&HallData<=6)
	{
		if(bldc_dev.order_direction==-1)
		{                                                     //��ת   ����
			BLDCSwitchN(HallData);
		}else if(bldc_dev.order_direction==1){                                                     //��ת   ���
			BLDCSwitchP(HallData); 
		}else{       //�������0��Ϊʲô������뻻�������Ϊ��Ϊ�ֶ���ת������Ӷ����벶���ж�
			
		}
		switch(HallData)
		{//ִ��һ����ת15�ȣ������1�������ź�������Ҫ���棬��¼�����ǰλ��      //ִ��һ�η�ת15�ȣ������1
			case 1: if(LastHallData==5){gCurrentPulseNum++;motor_dir=CCW;}  if(LastHallData==3){gCurrentPulseNum--;motor_dir=CW;}break;
			case 2: if(LastHallData==3){gCurrentPulseNum++;motor_dir=CCW;}  if(LastHallData==6){gCurrentPulseNum--;motor_dir=CW;}break;
			case 3: if(LastHallData==1){gCurrentPulseNum++;motor_dir=CCW;}  if(LastHallData==2){gCurrentPulseNum--;motor_dir=CW;}break;
			case 4: if(LastHallData==6){gCurrentPulseNum++;motor_dir=CCW;}  if(LastHallData==5){gCurrentPulseNum--;motor_dir=CW;}break;
			case 5: if(LastHallData==4){gCurrentPulseNum++;motor_dir=CCW;}  if(LastHallData==1){gCurrentPulseNum--;motor_dir=CW;}break;
			case 6: if(LastHallData==2){gCurrentPulseNum++;motor_dir=CCW;}  if(LastHallData==4){gCurrentPulseNum--;motor_dir=CW;}break;
			default:  break;
		}
		LastHallData = HallData;
	}
	return motor_dir;
}

/**************����******************/
void BLDC_Start(void)
{
//	TIM1->SMCR|=0x0006;         //��TIM1�����봥��        
//	TIM1->DIER|=0x0040;         //��TIM1�Ĵ����ж� ,���ܹر�ͨ��4�����жϣ���Ϊ���ж�����Ҫ�ɼ��������ź�
	
	u8 HallData = 0; 
	HallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);        //��ת��λ��    �ϳ�GPIOC8|GPIOC7|GPIOC6 

	if(HallData>=1&&HallData<=6)
	{
		 bldc_dev.motor_state = RUN;    //���ֹͣ	  //�������֮����1���Ժ��ڻ��������ж��н��л���
		
		if(bldc_dev.order_direction == CW) 		//��ת
		{                                                     
			BLDCSwitchN(HallData);	
		}else if(bldc_dev.order_direction == CCW){	//��ת                                                    
			BLDCSwitchP(HallData);	
		}
		TIM3->CNT=0;                //��TIM3�ļ�����   	
//		TIM3->DIER|=0x0050;         //��TIM3�ж�  ���������ж����ڲ�������źź�ͨ��4�����ж����ڼ�¼��ʱ
	}          
}

/**************ֹͣ******************/
void BLDC_Stop(void)
{
//	TIM1->SMCR&=0xfff8;         //�ر�TIM1�����봥��
	TIM1->CCER=0;               //�ر�TIM1����·���   �ر�PWM����ر��Ϲܣ������Ȼ����

//	PAout(8)=1;PAout(9)=1;PAout(10)=1;         //�¹ܹر�
//	PBout(13)=0;PBout(14)=0;PBout(15)=0;       //�Ϲܹر�   GPIO_Mode_AF_PP   ODR������Ч 
	
	__nop();__nop();           //��ʱ����������    ����һ��ʱ�䣬֮��
	PAout(8)=0;PAout(9)=0;PAout(10)=0;         //�¹ܴ򿪣������ܺ��ƶ�
	
//	TIM1->CCER=0;               //�ر�TIM1����·�������ɲ��      
	                           
/**��Ҫ�رն�ʱ��3�����ж�TIM_IT_Trigger����ֹ��Ϊ�ı���λ�ö��жϹر��޷������¼���λ��**/
}

void PwmOut(int SpeedOut)     //100us����һ�Σ�����pwm
{
	static int IOut;		  //���������
	int SetPwmValue;
	u16 IValue;
	static u16 LastIValue;
	
	IValue = (LastIValue>>1)+(g_ADC_Buf[0]>>1);
	LastIValue = IValue;
	
	IOut = CurrentProtection(SpeedOut,IValue,g_ADC_Buf[3]);   //��������,������

	if(bldc_dev.order_state == STOP){   //ָ��ֹͣ
		SetPwmValue=0;
		BLDC_Stop();          			//�ر�PWM����������Ȼ���٣�Ȼ��ɲ��
		time0=time1=time2 = 500000;		
		bldc_dev.motor_state = STOP;    //���ֹͣ	  
	}else{
		if(IOut>0)
		{
			SetPwmValue=IOut;
			if(SetPwmValue<100)      SetPwmValue=100;
			bldc_dev.order_direction = CCW;
		}else if(IOut<0){
			SetPwmValue=-IOut;
			if(SetPwmValue<100)      SetPwmValue=100;
			bldc_dev.order_direction = CW;
		}else{
			SetPwmValue=0;
			BLDC_Stop();          			//�ر�PWM����������Ȼ���٣�Ȼ��ɲ��
//			bldc_dev.motor_state = STOP;    //���ֹͣ	
		}
		
		TIM1->CCR1=SetPwmValue;	         	//�����TIM1��PWM�Ĵ���
		TIM1->CCR2=SetPwmValue;
		TIM1->CCR3=SetPwmValue; 
		if(bldc_dev.motor_state==STOP && SetPwmValue>=100) 	//��������ֹͣ״̬������Ҫ������������
		{
			BLDC_Start();
		}
	}
}



