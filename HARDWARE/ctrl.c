#include "includes.h"
#include "ctrl.h"
#include "delay.h"
#include "handle.h"

#define IR1LIN  PAout(8)          //IR2103_1的低端输入,高端为PWM输入
#define IR2LIN  PAout(9) 		  //IR2103_2的低端输入,高端为PWM输入
#define IR3LIN  PAout(10)         //IR2103_3的低端输入,高端为PWM输入

MotorDev bldc_dev={1000,STOP,STOP,CW,CW,0,0};

u8 HallA_data = 0,HallB_data = 0,HallC_data = 0; 


/**********************************************************************
* Description    : 对定时器1和定时器3的GPIO定义
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void BLDC_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
          
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;  //TIM1输出     GPIOA8通道CH1  GPIOA9通道CH2  GPIOA10通道CH3  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出 	受ODR控制                                         
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
                         
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//TIM1输出     GPIOB13互补通道CH1N  GPIOB14互补通道CH2N  GPIOB15互补通道CH3N  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用推挽输出    不受ODR控制                                       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;              //TIM3的霍尔输入
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
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;                       //基本结构体变量定义
	TIM_OCInitTypeDef        TIM_OCInitStructure;                         //输出结构体变量定义
	TIM_BDTRInitTypeDef      TIM_BDTRInitStructure;                       //死区刹车结构体变量定义

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	TIM_DeInit(TIM1);
	/*TIM基本初始化*/
	TIM_TimeBaseStructure.TIM_Prescaler = 3-1;                                //预分频值   72/3=24MHz
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned2;   //中央对齐计数模式,输出比较标志位只有在比较器向上计算被设置
	TIM_TimeBaseStructure.TIM_Period = 1200 - 1;                              //自动重载值ARR  PWM 24M/(1200*2)=10KHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                              //时钟分割
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                          //循环次数     tim1必须加这句不然没反应	    
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                         //TIM输出通道初始化
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;            //关闭OC输出到对应引脚  为什么关闭？   等到需要的时候打开
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;          //关闭OC互补输出        
	TIM_OCInitStructure.TIM_Pulse =600;                                      //CCR输出PWM占空比1200/1800=2/3  _——_——_
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;         
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;          //TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;          
	TIM_OC1Init(TIM1,&TIM_OCInitStructure); 

	TIM_OCInitStructure.TIM_Pulse =600;
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse =600;
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;                    //TIM输出通道4初始化，用来触发AD注入采样
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;                   
	TIM_OCInitStructure.TIM_Pulse =10; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;           
	TIM_OC4Init(TIM1,&TIM_OCInitStructure); 

	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;        //死区刹车初始化
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
	TIM_BDTRInitStructure.TIM_DeadTime = 120;
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;         //如打开，开机无输出且状态紊乱？？？？
	TIM_BDTRInitStructure.TIM_BreakPolarity =  TIM_BreakPolarity_Low ;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);

	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);  //使能捕获比较寄存器预装载（通道1）
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);  //使能捕获比较寄存器预装载（通道2）
	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);  //使能捕获比较寄存器预装载（通道3）
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);           //PWM输出

	TIM_ClearITPendingBit( TIM1, TIM_IT_CC4);
	TIM_Cmd(TIM1,ENABLE);
	TIM_ITConfig(TIM1, TIM_IT_CC4 ,ENABLE);
}

void BLDC_TIM4Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;              //基本结构体变量定义
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	TIM_DeInit(TIM4);
    /*TIM基本初始化*/
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;                    //预分频值   72/72=1MHz
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 4000-1;                       //自动重载值ARR  周期1us*1000=1ms
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);    
	
	TIM_ClearITPendingBit( TIM4, TIM_IT_Update);
	TIM_Cmd(TIM4,ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);////使能或者失能指定的TIM中断   TIM 更新中断源
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
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;              //基本结构体变量定义
	TIM_ICInitTypeDef  TIM_ICInitStructure;                      //定义结构体变量
	TIM_OCInitTypeDef  TIM_OCInitStructure;                      //输出结构体变量定义
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3 ,ENABLE ); 
	TIM_DeInit(TIM3);
    /*TIM基本初始化*/
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;                    //预分频值   72/72=1MHz
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 10000-1;                       //自动重载值ARR  周期1us*10000=10ms
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);     

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;            //选择通道1       通过异或输入，捕获3个霍尔信号
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //输入上升沿捕获  
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC;  //配置通道为输入，并映射到哪里 ，映射到TRC
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       //输入捕获预分频值
	TIM_ICInitStructure.TIM_ICFilter = 10;                      //输入滤波器带宽设置
	TIM_ICInit(TIM3, &TIM_ICInitStructure);                     //输入通道配置
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;                   //TIM输出通道初始化
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;             
	TIM_OCInitStructure.TIM_Pulse =10000;                       //开启比较捕获中断，当计数器的值=10000时就会触发中断
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      
	TIM_OC4Init(TIM3,&TIM_OCInitStructure);

	TIM_SelectHallSensor(TIM3,ENABLE);                          //使能TIMx的霍尔传感器接口,3个IO口就可以连接到一个通道（TIM_Channel_1）进行捕获

	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1F_ED);               //输入触发源选择     TI1的边沿检测器

	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);             //从模式选择

	TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);//主从模式选择        

	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_OC2Ref);       //选择输出触发模式(TRGO端)  通道OC2
	
	TIM3->SR &= 0xffef;	    		//清中断标志  CC4IF
	TIM3->SR &= 0xffbf;        	    //清中断标志
	TIM_Cmd(TIM3,ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Trigger|TIM_IT_CC4, ENABLE);   //开定时器中断   TIM_IT_Trigger中断记录时间   TIM_IT_CC4 中断计算速度换相                     
}

void BLDCSwitchP(u8 HallData)        //正转   伸出
{
	switch(HallData)                                 //根据转子位置，决定CCER输出相位和转子字偏移量
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

void BLDCSwitchN(u8 HallData)        //反转   缩进
{
	switch(HallData)                 //根据转子位置，决定CCER输出相位和转子字偏移量
	{
		case 0x05:{                   //101    
			IR1LIN=1;IR3LIN=1;   //先关闭下管输出
			TIM1->CCER=0x3004;        //1,4     A+,B-  0x3034   ，然后打开上管PWM输出
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
		
		default:TIM1->CCER=0x3000;   IR1LIN=1;IR2LIN=1;IR3LIN=1; break;   //出错，PWM不输出，下管全部关闭
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
	HallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);      //读转子位置    合成GPIOC8|GPIOC7|GPIOC6 

	if(HallData>=1&&HallData<=6)
	{
		if(bldc_dev.order_direction==-1)
		{                                                     //反转   缩进
			BLDCSwitchN(HallData);
		}else if(bldc_dev.order_direction==1){                                                     //正转   伸出
			BLDCSwitchP(HallData); 
		}else{       //方向等于0，为什么还会进入换相程序，因为人为手动旋转电机，从而进入捕获中断
			
		}
		switch(HallData)
		{//执行一次正转15度，脉冲加1，霍尔信号脉冲需要保存，记录电机当前位置      //执行一次反转15度，脉冲减1
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

/**************启动******************/
void BLDC_Start(void)
{
//	TIM1->SMCR|=0x0006;         //开TIM1的输入触发        
//	TIM1->DIER|=0x0040;         //开TIM1的触发中断 ,不能关闭通道4捕获中断，因为在中断中需要采集传感器信号
	
	u8 HallData = 0; 
	HallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);        //读转子位置    合成GPIOC8|GPIOC7|GPIOC6 

	if(HallData>=1&&HallData<=6)
	{
		 bldc_dev.motor_state = RUN;    //电机停止	  //电机启动之后置1，以后都在霍尔捕获中断中进行换相
		
		if(bldc_dev.order_direction == CW) 		//反转
		{                                                     
			BLDCSwitchN(HallData);	
		}else if(bldc_dev.order_direction == CCW){	//正转                                                    
			BLDCSwitchP(HallData);	
		}
		TIM3->CNT=0;                //清TIM3的计数器   	
//		TIM3->DIER|=0x0050;         //开TIM3中断  开启触发中断用于捕获霍尔信号和通道4捕获中断用于记录超时
	}          
}

/**************停止******************/
void BLDC_Stop(void)
{
//	TIM1->SMCR&=0xfff8;         //关闭TIM1的输入触发
	TIM1->CCER=0;               //关闭TIM1的六路输出   关闭PWM输出关闭上管，电机自然减速

//	PAout(8)=1;PAout(9)=1;PAout(10)=1;         //下管关闭
//	PBout(13)=0;PBout(14)=0;PBout(15)=0;       //上管关闭   GPIO_Mode_AF_PP   ODR控制无效 
	
	__nop();__nop();           //延时（加死区）    减速一定时间，之后
	PAout(8)=0;PAout(9)=0;PAout(10)=0;         //下管打开，进行能耗制动
	
//	TIM1->CCER=0;               //关闭TIM1的六路输出，关刹车      
	                           
/**不要关闭定时器3触发中断TIM_IT_Trigger，防止人为改变电机位置而中断关闭无法捕获记录电机位置**/
}

void PwmOut(int SpeedOut)     //100us调用一次，更新pwm
{
	static int IOut;		  //电流环输出
	int SetPwmValue;
	u16 IValue;
	static u16 LastIValue;
	
	IValue = (LastIValue>>1)+(g_ADC_Buf[0]>>1);
	LastIValue = IValue;
	
	IOut = CurrentProtection(SpeedOut,IValue,g_ADC_Buf[3]);   //电流保护,掉电检测

	if(bldc_dev.order_state == STOP){   //指令停止
		SetPwmValue=0;
		BLDC_Stop();          			//关闭PWM输出，电机自然减速，然后刹车
		time0=time1=time2 = 500000;		
		bldc_dev.motor_state = STOP;    //电机停止	  
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
			BLDC_Stop();          			//关闭PWM输出，电机自然减速，然后刹车
//			bldc_dev.motor_state = STOP;    //电机停止	
		}
		
		TIM1->CCR1=SetPwmValue;	         	//输出到TIM1的PWM寄存器
		TIM1->CCR2=SetPwmValue;
		TIM1->CCR3=SetPwmValue; 
		if(bldc_dev.motor_state==STOP && SetPwmValue>=100) 	//如果电机是停止状态，则需要调用启动程序
		{
			BLDC_Start();
		}
	}
}



