#include "bsp.h"
#include "stmflash.h"
#include "lcdtest.h"
#include "main.h"
#include "wdg.h"
#include "modbus.h"


#define get4bit(a,b)  ((a>>(b*4))&0x000f)

//以下是要存储的参数
__noinit__ u16   gCheckFlag;		       //校验标志
__noinit__ u16   gIwdgFlag;                //看门狗复位标志
__noinit__ u16   gBackupFlag;              //备份标志     0表示无备份，1表示有备份

__noinit__ u16   gWorkMode;                //0=手动   1=自动  2=对中
__noinit__ u16   gSensorMode;              //纠偏模式，0=左，1=右，2=左+右，3=SPC  ，EPC1超声波传感器1工作，EPC2超声波传感器2工作，CPC两个传感器一起工作，SPC
__noinit__ u16   gSensorSignal;

__noinit__ u16   gAutoPolar;               //自动极性，1=负极，0=正极;
__noinit__ u16   gManuPolar;               //手动极性，1=负极，0=正极;
__noinit__ u16   gMotorType;               //电机类型   0无刷电机4    1无刷电机5  2有刷电机
__noinit__ u16   gPowerOnMode;             //开机工作模式，,0=手动，1=自动，2=中心，3=上次
__noinit__ u16   gCurMatNum;               //默认材料

__noinit__ u16   gGainData;                //系统调整灵敏度;
__noinit__ u16   gDeadZone;                //盲区设置，传感器识别盲区，当材料边沿有缺口时，防止抖动
__noinit__ u16   gFineTune;                //微调=补偿，当需要微调材料位置时候，需要改变传感器位置，通过设置微调参数，避免调整传感器位置

__noinit__ u16   gAutoSpeed;               //自动速度
__noinit__ u16   gManuSpeed;               //手动速度
__noinit__ u16   gCentSpeed;               //对中速度

__noinit__ u16   gCaliTorque;              //校准转矩
__noinit__ u16   gFuncTorque;              //正常支行时转矩限制，最大为10A
__noinit__ s16   gCurrentPulseNum;         //当前所在位置脉冲数
__noinit__ s16   gMAXPulseNum;             //行程最大脉冲数

__noinit__ u16   gLimitMode;               //限位方式   0为内部限位  1为外部限位开关限位
__noinit__ u16   gExtendLimit;             //前限位点设置
__noinit__ u16   gCenterLimit;		       //中心限位点设置
__noinit__ u16   gIndentLimit;			   //后限位点设置

__noinit__ u16   gSPCMode;                 //蛇形纠偏模式  0：内部编码器  1：外部传感器
__noinit__ u16   gSPCStopTime;             //spc停止时间=0.2秒
__noinit__ u16   gSPCExtendLimit;          //spc伸出停止点
__noinit__ u16   gSPCIndentLimit;           //spc缩进停止点

__noinit__ u16   gNoWaitEN;                //无料等待功能  0=使能,1=禁止
__noinit__ u16   gNoDetectValve;           //无料检测阀值  
__noinit__ u16   gNoDetectTime;            //无料检测时间=0.1秒
__noinit__ u16   gNoWaitTime;              //无料等待时间=1.5秒  

__noinit__ u16	 gLongIo0Mode;             //远程输入口1状态 
__noinit__ u16   gLongIo1Mode;             //远程输入口2状态
__noinit__ u16   gLongIo2Mode;             //远程输入口3状态
__noinit__ u16   gLongIo3Mode;             //远程输入口4状态 

__noinit__ u16   gAFlexAcc;                //自动柔性加速
__noinit__ u16   gAFlexDec;                //自动柔性减速
__noinit__ u16   gMFlexAcc;                //手动柔性加速
__noinit__ u16   gMFlexDec;                //手动柔性减速
__noinit__ u16   gAlarmSwitch;
__noinit__ u16   gRelay;
__noinit__ u16   gKeepWait;

__noinit__ u16   g_Mat0_SensorL_H;      //左传感器的高信号值
__noinit__ u16   g_Mat0_SensorL_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat0_SensorR_H;      //右传感器的高信号值
__noinit__ u16   g_Mat0_SensorR_L;      //右传感器的低信号值 
__noinit__ u16    Mat0EPC12;

__noinit__ u16   g_Mat1_SensorL_H;      //左传感器的高信号值
__noinit__ u16   g_Mat1_SensorL_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat1_SensorR_H;      //右传感器的高信号值
__noinit__ u16   g_Mat1_SensorR_L;      //右传感器的低信号值 
__noinit__ u16    Mat1EPC12;

__noinit__ u16   g_Mat2_SensorL_H;      //左传感器的高信号值
__noinit__ u16   g_Mat2_SensorL_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat2_SensorR_H;      //右传感器的高信号值
__noinit__ u16   g_Mat2_SensorR_L;      //右传感器的低信号值 
__noinit__ u16    Mat2EPC12;

__noinit__ u16   g_Mat3_SensorL_H;      //左传感器的高信号值
__noinit__ u16   g_Mat3_SensorL_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat3_SensorR_H;      //右传感器的高信号值
__noinit__ u16   g_Mat3_SensorR_L;      //右传感器的低信号值 
__noinit__ u16    Mat3EPC12;

__noinit__ u16   g_Mat4_SensorL_H;      //左传感器的高信号值
__noinit__ u16   g_Mat4_SensorL_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat4_SensorR_H;      //右传感器的高信号值
__noinit__ u16   g_Mat4_SensorR_L;      //右传感器的低信号值 
__noinit__ u16    Mat4EPC12;

__noinit__ u16   g_Mat5_SensorL_H;      //左传感器的高信号值
__noinit__ u16   g_Mat5_SensorL_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat5_SensorR_H;      //右传感器的高信号值
__noinit__ u16   g_Mat5_SensorR_L;      //右传感器的低信号值 
__noinit__ u16    Mat5EPC12;

__noinit__ u16   g_Mat6_SensorL_H;      //左传感器的高信号值
__noinit__ u16   g_Mat6_SensorL_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat6_SensorR_H;      //右传感器的高信号值
__noinit__ u16   g_Mat6_SensorR_L;      //右传感器的低信号值 
__noinit__ u16    Mat6EPC12;

__noinit__ u16   g_Mat7_SensorL_H;      //左传感器的高信号值
__noinit__ u16   g_Mat7_SensorL_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat7_SensorR_H;      //右传感器的高信号值
__noinit__ u16   g_Mat7_SensorR_L;      //右传感器的低信号值 
__noinit__ u16    Mat7EPC12;


u16  LongPortFun[FunNum];      	  	 /*******远程IO控制******/
u16  ClickButton;                    //点动按键属性
u8 	 Warm[WarmNum];               	 /*******报警标志******/

volatile u16    g_ADC_Buf[4];        //DMA目标地址


u16  SensorL_value;                  //传感器1的值
u16  SensorR_value;                  //传感器2的值                      

u32  NUMBERTURN;                     //3次换相时间
u8   SERIES;                         //电机极对数

u8   LastHallData;


OS_EVENT *iMbox;                    //接收邮箱 
OS_EVENT *sem_p;
OS_EVENT *sem_v;
OS_EVENT *mutex;            		//互斥信号量

//start任务
#define	START_TASK_PRIO				22      //任务创建优先级
#define	START_STK_SIZE				32
OS_STK  START_TASK_STK[START_STK_SIZE];
void    start_task(void *pdata);

//READ_IO任务
#define	READ_IO_TASK_PRIO				5     //任务创建优先级
#define	READ_IO_STK_SIZE				64
OS_STK  READ_IO_TASK_STK[READ_IO_STK_SIZE];
void    readio_task(void *pdata);

//LCD1任务
#define	MODBUS_TASK_PRIO			6      //任务创建优先级
#define	MODBUS_STK_SIZE				64
OS_STK  MODBUS_TASK_STK[MODBUS_STK_SIZE];
void    modbus_task(void *pdata);

//LCD1任务
#define	LCD_TASK_PRIO				7      //任务创建优先级
#define	LCD_STK_SIZE				64
OS_STK  LCD_TASK_STK[LCD_STK_SIZE];
void    lcd_task(void *pdata);

//LED任务
#define	LED_TASK_PRIO				20      //任务创建优先级
#define	LED_STK_SIZE				32
OS_STK  LED_TASK_STK[LED_STK_SIZE];
void    led_task(void *pdata);

void SensorInit(void)
{
	g_Mat0_SensorL_H=3930;      //左传感器的高信号值
	g_Mat0_SensorL_L=165;      //左传感器的低信号值 
	g_Mat0_SensorR_H=3930;      //右传感器的高信号值
	g_Mat0_SensorR_L=165;      //右传感器的低信号值 
	Mat0EPC12=3;

	g_Mat1_SensorL_H=3930;      //左传感器的高信号值
	g_Mat1_SensorL_L=165;      //左传感器的低信号值 
	g_Mat1_SensorR_H=3930;      //右传感器的高信号值
	g_Mat1_SensorR_L=165;      //右传感器的低信号值 
	Mat1EPC12=0;

	g_Mat2_SensorL_H=3930;      //左传感器的高信号值
	g_Mat2_SensorL_L=165;      //左传感器的低信号值 
	g_Mat2_SensorR_H=3930;      //右传感器的高信号值
	g_Mat2_SensorR_L=165;      //右传感器的低信号值 
	Mat2EPC12=0;

	g_Mat3_SensorL_H=3930;      //左传感器的高信号值
	g_Mat3_SensorL_L=165;      //左传感器的低信号值 
	g_Mat3_SensorR_H=3930;      //右传感器的高信号值
	g_Mat3_SensorR_L=165;      //右传感器的低信号值 
	Mat3EPC12=0;

	g_Mat4_SensorL_H=3930;      //左传感器的高信号值
	g_Mat4_SensorL_L=165;      //左传感器的低信号值 
	g_Mat4_SensorR_H=3930;      //右传感器的高信号值
	g_Mat4_SensorR_L=165;      //右传感器的低信号值 
	Mat4EPC12=0;
	
	g_Mat5_SensorL_H=3930;      //左传感器的高信号值
	g_Mat5_SensorL_L=165;      //左传感器的低信号值 
	g_Mat5_SensorR_H=3930;      //右传感器的高信号值
	g_Mat5_SensorR_L=165;      //右传感器的低信号值 
	Mat5EPC12=0;

	g_Mat6_SensorL_H=3930;      //左传感器的高信号值
	g_Mat6_SensorL_L=165;      //左传感器的低信号值 
	g_Mat6_SensorR_H=3930;      //右传感器的高信号值
	g_Mat6_SensorR_L=165;      //右传感器的低信号值 
	Mat6EPC12=0;

	g_Mat7_SensorL_H=3930;      //左传感器的高信号值
	g_Mat7_SensorL_L=165;      //左传感器的低信号值 
	g_Mat7_SensorR_H=3930;      //右传感器的高信号值
	g_Mat7_SensorR_L=165;      //右传感器的低信号值 
	Mat7EPC12=0;
}

void SetDataInit()                 //设置参数初始化
{
	gCheckFlag = 0xee55;		 //校验标志
	gIwdgFlag = 0;               //触发独立看门狗计数
	gBackupFlag = 0;             //是否有备份，1表示有备份，0表示没有备份
	
	gWorkMode=0;                 //0=手动   1=自动  2=对中
	gSensorMode=0;               //纠偏模式，0=左，1=右，2=左+右，3=SPC  ，EPC1超声波传感器1工作，EPC2超声波传感器2工作，CPC两个传感器一起工作，SPC
	gSensorSignal = 1;
	
	gAutoPolar=0;                //自动极性，0=负极，1=正极;
	gManuPolar=0;                //手动极性，0=负极，1=正极;
	gMotorType = 0;              //电机类型   0无刷电机4   1无刷电机5   2有刷电机
	gPowerOnMode  =0;            //开机工作模式，,0=手动，1=自动，2=中心，3=上次 
	gCurMatNum=0;                //默认材料

	gGainData = 40;              //系统调整灵敏度;
	gDeadZone = 1;               //盲区设置，传感器识别盲区，当材料边沿有缺口时，防止抖动
	gFineTune = 80;              //微调=补偿，当需要微调材料位置时候，需要改变传感器位置，通过设置微调参数，避免调整传感器位置

	gAutoSpeed = 800;            //自动速度
	gManuSpeed = 600;            //手动速度
	gCentSpeed = 600;            //对中速度
	
	gCaliTorque=1;
	gFuncTorque = 5;             //正常支行时转矩限制，最大为10A
	gCurrentPulseNum=200;        //当前所在位置脉冲数
	gMAXPulseNum=500;            //行程最大脉冲数

	gLimitMode = 0;              //限位方式   0为内部限位  1为外部限位开关限位   2内部加外部
	gExtendLimit=95;             //伸出限位点设置
	gCenterLimit=50;		     //中心限位点设置
	gIndentLimit=5;			     //缩进限位点设置

	gNoWaitEN=2;                 //无料等待功能，0回到中心;1电机停止;2无操作;
	gNoDetectValve=95;           //无料检测阀值  
	gNoDetectTime=1;             //无料检测时间=0.1秒
	gNoWaitTime=50;              //无料等待时间=5秒  

	gSPCMode = 0;                //蛇形纠偏模式  0：内部编码器  1：外部传感器
	gSPCStopTime=2;              //spc停止时间=0.2秒
	gSPCExtendLimit=90;          //spc伸出停止点
	gSPCIndentLimit=10;          //spc缩进停止点

	gLongIo0Mode=0;              //远程输入口1状态 
	gLongIo1Mode=0;              //远程输入口2状态
	gLongIo2Mode=0;              //远程输入口3状态
	gLongIo3Mode=0;              //远程输入口4状态

	gAFlexAcc=AFlexAcc;          //自动柔性加速
	gAFlexDec=AFlexDec;          //自动柔性减速
	gMFlexAcc=MFlexAcc;          //手动柔性加速
	gMFlexDec=MFlexDec;          //手动柔性减速	
	gAlarmSwitch  = 0;
	gSensorSignal = 1;
	gRelay		  = 0;
	gKeepWait	  = 0;
}

void ParameterInit()             //参数出厂设置
{
	SetDataInit();               //设置参数初始化
	SensorInit();                //传感器参数初始化
	Modbus_Init();
}

int main(void)
{
	OSInit();    //创建空闲任务和统计任务
	OSTaskCreate(start_task,(void*)0,&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart();   //当调用 OSStart()时，OSStart()从任务就绪表中找出那个用户建立的优先级最高任务的任务控制块，运行最高优先级任务
}

/**-----------------开始任务------------------**/
void start_task(void *pdata) 
{	
	OS_CPU_SR cpu_sr=0;

	pdata=pdata;
	
    BSP_Init();     
  
	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)!= RESET)         //判断是否发生了独立看门狗复位
	{
		RCC->CSR = 1<<24;    	/*Clear reset flag*/
		gIwdgFlag++;          	//发生复位，__noinit__修饰的全局变量会保持复位之前的数据，不会清零
//		PBout(12)=1;	
	}else{                      //上电复位
		ReadPara();           	//读取flash中保存的参数
		ReadSensor();
//		PBout(12)=0;
	}
	
	if(gCheckFlag!=0xee55)  			SetDataInit();  	 //如果参数读取错误，就重新初始化参数
	if(g_Mat0_SensorL_H == 0xffff)  	SensorInit();		 //如果参数读取错误，就重新初始化传感器参数
	
	switch(gPowerOnMode)                    		 //查询开机时的工作状态
	{
	   case 0: gWorkMode=0;          	break;      //开机手动
	   case 1: gWorkMode=1;          	break;      //开机时为自动状态
	   case 2: gWorkMode=2;             break;      //开机时为中心状态
	   case 3: 						 	break;      //恢复关机的状态
	   default: SetDataInit();    		break;      //系统初始化          
	}
	*pHWorkMode = gWorkMode;			//0=手动   1=自动  2=对中
	
	switch(gMotorType)                  //查询电机类型
    {
        case BrushlessMotor4:           //如果选择的无刷电机就开CAP,和外部中断作为霍尔检测    4对级
			NUMBERTURN = 7500000;       //3次换相转过角度45度   （45/(t1+t2+t3)）*60/360  乘以60表示1分钟转过角度
		    SERIES     =   24;
		break;                   
        case BrushlessMotor5:                    //5对级
			NUMBERTURN = 6000000;
		    SERIES     =   30;
		break; 
        default:SetDataInit();          break;   //系统初始化  
    }
	
	/***********霍尔位置读取***********/
	LastHallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);   //读转子位置    合成GPIOC8|GPIOC7|GPIOC6 	                                                            
	
	Modbus_Init();       			//根据读取的全局变量更新保持寄存器
#if (OS_TASK_STAT_EN > 0)   		//允许统计任务
	OSStatInit();     
#endif	
	OS_ENTER_CRITICAL();             //进入临界区(开中断)
	//OSTaskCreate();                创建任务
	OSTaskCreate(readio_task, (void*)0, &READ_IO_TASK_STK[READ_IO_STK_SIZE-1],READ_IO_TASK_PRIO);
	OSTaskCreate(modbus_task, (void*)0, &MODBUS_TASK_STK[MODBUS_STK_SIZE-1],MODBUS_TASK_PRIO);
	OSTaskCreate(lcd_task, (void*)0, &LCD_TASK_STK[LCD_STK_SIZE-1],LCD_TASK_PRIO);
	OSTaskCreate(led_task, (void*)0, &LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);  //挂起开始任务
	OS_EXIT_CRITICAL();              //退出临界区(开中断)
}

/**-----------------readio_task任务---------------------**/
void readio_task(void *pdata)
{
	static u16 LastSwitch1EPC,LastSwitch2EPC;
	
//  如果定义为全局变量，psi_pid_task()优先级高于si_pid_task()优先级,SetPwmValue可能被高优先级的任务更改，从而出错            
//	Wwdg_Init(0x7f,0x7d,3);
	IWDG_Init(7,312);   		  //7表示256分频，256/40K=6.4ms,6.4*312=2s表示喂狗间隔2s
	for(;;)
	{
		ReadLong(LongPortFun);
		
		if(LongPortFun[Switch1_EPC] != LastSwitch1EPC)
		{
			*pHSensorMode=LongPortFun[Switch1_EPC];
		}
		if(LongPortFun[Switch2_EPC] != LastSwitch2EPC)
		{
			*pHSensorMode=LongPortFun[Switch2_EPC];
			*pHAutoPolar=LongPortFun[Switch2_EPC];
		}
		LastSwitch1EPC = LongPortFun[Switch1_EPC];
		LastSwitch2EPC = LongPortFun[Switch2_EPC];
		
		delay_ms(10);
		IWDG_Feed();   		 	//2s之内没有喂狗则复位
	}
}

/**-----------------modbus_task任务---------------------**/
void modbus_task(void *pdata)
{
	u8 err;
	
	iMbox = OSMboxCreate(NULL);                  //建立并初始化一个消息邮箱，空邮箱 
	sem_p = OSSemCreate(1);
	sem_v = OSSemCreate(1);
	mutex = OSMutexCreate(1,&err);               //建立并初始化一个互斥信号量，优先级为1    
	for(;;)
	{
		Modbus_Event();
		
		/*********根据*pHAdjustFlag调用调节参数，完成后*pHAdjustFlag=255**********/
		if(*pHAdjustFlag!=255)  AddSub();
		
		/********需要快速处理的指令********/
		ClickButton 	= *pHClickBut;
		
		
		if(*pHTCaliFlag == 1)   //读取校准标志，查看是否启动校准
		{
			*pHTCaliFlag = 0;
			TravelCal.CaliStep = 0;
			TravelCal.StallDir = 0;
			TravelCal.CaliFlag = 1;
		}
			 
		if(*pHTCaliCancel == 1)              //取消校准按下
		{   
			*pHTCaliCancel = 0;
			TravelCal.CaliFlag = 0;
			TravelCal.CaliStep = 0;
			TravelCal.StallDir = 0;
		}
		
		delay_ms(5);
	}
}

/**-----------------LCD任务---------------------**/
void lcd_task(void *pdata)
{
	u16 *pd;
	u8 SensorTips,WarmFlag;
	static u16 TimeCnt;

	
	for(;;)
	{
//		OSSemPend(sem_v, 0, &err);
		/*****************************串口屏下发指令，修改保持寄存器，然后更新全局变量***********************************/

		/*********根据屏幕指令修改变量值**********/
		gWorkMode		 =*pHWorkMode;
		gSensorMode		 =*pHSensorMode;
		gSensorSignal    =*pHSignal;
		gAutoPolar		 =*pHAutoPolar;
		gManuPolar		 =*pHManuPolar;	
		gMotorType		 =*pHMotorType;
		switch(gMotorType)                  //查询电机类型
		{
			case 0:                         //如果选择的无刷电机就开CAP,和外部中断作为霍尔检测    4对级
				NUMBERTURN = 7500000;       //3次换相转过角度45度   （45/(t1+t2+t3)）*60/360  乘以60表示1分钟转过角度
				SERIES     =   24;
			break;                   
			case 1:                         //5对级
				NUMBERTURN = 6000000;
				SERIES     =   30;
			break; 
			default:SetDataInit();          break;            //系统初始化  
		}
		gPowerOnMode = *pHPowerOnMode;    //保存上电状态
		gCurMatNum	 = *pHMatNum;         //保存当前材料标号
		
		if(*pHTCaliTorque == 0)			gCaliTorque	 =1;
		else if(*pHTCaliTorque == 1)	gCaliTorque	 =2;
		else if(*pHTCaliTorque == 2)	gCaliTorque	 =3;
		else if(*pHTCaliTorque == 3)	gCaliTorque	 =4;
		
		gLimitMode	 = *pHLimitMode;
		gSPCMode	 = *pHSPCMode;        //保存SPC模式
		gNoWaitEN	 = *pHNoWaitEN;       //保存无料使能

		gLongIo0Mode = *pHPortFun0;
		gLongIo1Mode = *pHPortFun1;
		gLongIo2Mode = *pHPortFun2;
		gLongIo3Mode = *pHPortFun3;
		
		gAlarmSwitch = *pHAlarmSwitch;
		gRelay		 = *pHRelay;
		gKeepWait    = *pHKeepWait;
		
//		OSMutexPend(mutex,0,&err);             //提高任务优先级
//		OSMutexPost(mutex);                    //恢复任务优先级
		
		switch(*pHFlexSpeed)
		{
			case 1:		gAFlexAcc = AFlexAcc; *pHFlexSpeed = 0;	 break;
			case 2:		gAFlexDec = AFlexDec; *pHFlexSpeed = 0;  break;
			case 4:		gMFlexAcc = MFlexAcc; *pHFlexSpeed = 0;  break;
			case 8:		gMFlexDec = MFlexDec; *pHFlexSpeed = 0;  break;
			default:	break;
		}
		/*****************备份还原********************/
		BackupRestore();
		
		/******************更新输入寄存器,读取输入寄存器器用于显示***********************/
		
		/*****************警告标志计算***************/
		WarmFlag = WarmOut();

		/**************更新输入寄存器参数，以便主机读取**********/
//		if(TravelCal.CaliStep==3)  
//		{
//			if(TimeCnt==30)	{TravelCal.CaliStep = 0; TimeCnt=0;}   
//			TimeCnt++;
//		}
		/*********更新传感器显示******pSensorRate****/
		SensorDis();          //根据主机更新的传感器模式调用不同程序，显示传感器参数
		
		/*********更新校准传感器显示*****pSensorValue、pMatDis*****/
		SensorTips = MatCal.fun[*pHSCaliStep]();    //根据主机更新的校准步骤调用不同程序，显示传感器参数 ，返回校准成功或者失败
		
		*pGainDead 		= gGainData|(gDeadZone<<8);
		*pFineTune  	= (gFineTune|(WarmFlag<<8))|SensorTips<<12|gBackupFlag<<13|gWorkMode<<14;
		*pDisPulseNum  	= HallRate|gAutoPolar<<14|gManuPolar<<15;
		*pMatDis       	= Mat0EPC12|Mat1EPC12<<2|Mat2EPC12<<4|Mat3EPC12<<6|Mat4EPC12<<8|Mat5EPC12<<10|Mat6EPC12<<12|Mat7EPC12<<14;
		*pLimitFun1    	= gLimitMode|gExtendLimit<<8;
		*pLimitFun2		= gCenterLimit|gIndentLimit<<8;
		*pSPCFun1		= gSPCMode|gSPCStopTime<<8;
		*pSPCFun2		= gSPCExtendLimit|gSPCIndentLimit<<8;
		*pNoMatFun1		= gNoWaitEN|gKeepWait<<4|gNoDetectValve<<8;
		*pNoMatFun2		= gNoDetectTime|gNoWaitTime<<8;
		*pAFlex			= gAFlexAcc|gAFlexDec<<8;
		*pMFlex			= gMFlexAcc|gMFlexDec<<8;
		*pAutoSpeed		= gAutoSpeed|LongPortFun[AutoDef]<<13;
		*pManuSpeed		= gManuSpeed|LongPortFun[ManuDef]<<13;
		*pCentSpeed		= gCentSpeed|LongPortFun[PusherCenter]<<13;
		*pCurrentPara	= (u16)(g_ADC_Buf[0]*0.08)|gFuncTorque<<8|*pHTCaliTorque<<12|TravelCal.CaliStep<<14;
		*pPortFun		= gLongIo0Mode|gLongIo1Mode<<4|gLongIo2Mode<<8|gLongIo3Mode<<12;
		*pFunc			= gSensorMode|gMotorType<<2|gPowerOnMode<<4|gAlarmSwitch<<6|gSensorSignal<<8|gRelay<<11;
		//OSSemPost(sem_p);
		delay_ms(50);
	}
}

/**-----------------LED任务---------------------**/
void led_task(void *pdata)
{
	Interrupt_Config();
	for(;;)
	{
		PBout(12)=1;
		delay_ms(500);
		PBout(12)=0;
		delay_ms(500);
	}
}



