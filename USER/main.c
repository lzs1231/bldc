#include "sys.h"
#include "delay.h"
#include "bsp.h"
#include "includes.h"
#include "hmi_driver.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stmflash.h"
#include "handle.h"
#include "pid_control.h"
#include "lcdtest.h"
#include "main.h"
#include "wdg.h"
 
//以下是要存储的参数

__noinit__ u16   gCheckFlag;		           //校验标志
__noinit__ u16   gIwdgFlag;                 //看门狗复位标志
__noinit__ u16   gBackupFlag;               //备份标志     0表示无备份，1表示有备份

__noinit__ u16   gWorkMode;                 //0=手动   1=自动  2=对中
__noinit__ u16   gSensorMode;               //纠偏模式，0=左，1=右，2=左+右，3=SPC  ，EPC1超声波传感器1工作，EPC2超声波传感器2工作，CPC两个传感器一起工作，SPC

__noinit__ u16   gAutoPolar;                //自动极性，1=负极，0=正极;
__noinit__ u16   gManuPolar;                //手动极性，1=负极，0=正极;
__noinit__ u16   gMotorType;               //电机类型   0无刷电机4    1无刷电机5  2有刷电机
__noinit__ u16   gPowerOnMode;             //开机工作模式，,0=手动，1=自动，2=中心，3=上次
__noinit__ u16   gCurMatNum;              //默认材料

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
__noinit__ u16   gBehindLimit;             //前限位点设置
__noinit__ u16   gCenterLimit;		       //中心限位点设置
__noinit__ u16   gFrontLimit;			   //后限位点设置

__noinit__ u16   gSPCMode;                 //蛇形纠偏模式  0：内部编码器  1：外部传感器
__noinit__ u16   gSPCStopTime;             //spc停止时间=0.2秒
__noinit__ u16   gSPCBehindLimit;          //spc伸出停止点
__noinit__ u16   gSPCFrontLimit;           //spc缩进停止点

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

__noinit__ u16   gAFlexSacc; 
__noinit__ u16   gAFlexSdec; 
__noinit__ u16   gMFlexSacc;
__noinit__ u16   gMFlexSdec;

__noinit__ u16   gLanguageChoice;             //中文
__noinit__ u16   gLockTime;
__noinit__ u16   gBrightness;
__noinit__ u16   gBuzzer;
__noinit__ u8    Password[6];                   //用户密码
char  *SysPassword = "888888";      //系统密码

__noinit__ u16   g_Mat0_Sensor1_H;      //左传感器的高信号值
__noinit__ u16   g_Mat0_Sensor1_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat0_Sensor2_H;      //右传感器的高信号值
__noinit__ u16   g_Mat0_Sensor2_L;      //右传感器的低信号值 
__noinit__ u16    Mat0EPC12;

__noinit__ u16   g_Mat1_Sensor1_H;      //左传感器的高信号值
__noinit__ u16   g_Mat1_Sensor1_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat1_Sensor2_H;      //右传感器的高信号值
__noinit__ u16   g_Mat1_Sensor2_L;      //右传感器的低信号值 
__noinit__ u16    Mat1EPC12;

__noinit__ u16   g_Mat2_Sensor1_H;      //左传感器的高信号值
__noinit__ u16   g_Mat2_Sensor1_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat2_Sensor2_H;      //右传感器的高信号值
__noinit__ u16   g_Mat2_Sensor2_L;      //右传感器的低信号值 
__noinit__ u16    Mat2EPC12;

__noinit__ u16   g_Mat3_Sensor1_H;      //左传感器的高信号值
__noinit__ u16   g_Mat3_Sensor1_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat3_Sensor2_H;      //右传感器的高信号值
__noinit__ u16   g_Mat3_Sensor2_L;      //右传感器的低信号值 
__noinit__ u16    Mat3EPC12;

__noinit__ u16   g_Mat4_Sensor1_H;      //左传感器的高信号值
__noinit__ u16   g_Mat4_Sensor1_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat4_Sensor2_H;      //右传感器的高信号值
__noinit__ u16   g_Mat4_Sensor2_L;      //右传感器的低信号值 
__noinit__ u16    Mat4EPC12;

__noinit__ u16   g_Mat5_Sensor1_H;      //左传感器的高信号值
__noinit__ u16   g_Mat5_Sensor1_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat5_Sensor2_H;      //右传感器的高信号值
__noinit__ u16   g_Mat5_Sensor2_L;      //右传感器的低信号值 
__noinit__ u16    Mat5EPC12;

__noinit__ u16   g_Mat6_Sensor1_H;      //左传感器的高信号值
__noinit__ u16   g_Mat6_Sensor1_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat6_Sensor2_H;      //右传感器的高信号值
__noinit__ u16   g_Mat6_Sensor2_L;      //右传感器的低信号值 
__noinit__ u16    Mat6EPC12;

__noinit__ u16   g_Mat7_Sensor1_H;      //左传感器的高信号值
__noinit__ u16   g_Mat7_Sensor1_L;      //左传感器的低信号值 
__noinit__ u16   g_Mat7_Sensor2_H;      //右传感器的高信号值
__noinit__ u16   g_Mat7_Sensor2_L;      //右传感器的低信号值 
__noinit__ u16    Mat7EPC12;


u8 	 Warm[WarmNum];               	 /*******报警标志******/
u16  ClickButton;                    //点动按键属性
u16  LongPortFun[FunNum];      	  	 /*******远程IO控制******/

u8   cmd_buffer[CMD_MAX_SIZE];       //指令缓存
volatile u16    g_ADC_Buf[4];        //DMA目标地址
volatile bool   g_ADC_OK = FALSE;    //ADC采集成功标志

u16  SensorL_value;                  //传感器1的值
u16  SensorR_value;                  //传感器2的值                      

u32  NUMBERTURN;                     //3次换相时间
u8   SERIES;                         //电机极对数

u8   LastHallData;


OS_EVENT *iMbox;                    //接收邮箱 

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

//LCD任务
#define	LCD_TASK_PRIO				13    		//任务创建优先级
#define	LCD_STK_SIZE				128
OS_STK  LCD_TASK_STK[LCD_STK_SIZE];
void    lcd_task(void *pdata);

//LED任务
#define	LED_TASK_PRIO				20      //任务创建优先级
#define	LED_STK_SIZE				32
OS_STK  LED_TASK_STK[LED_STK_SIZE];
void    led_task(void *pdata);

void SensorInit(void)
{
	g_Mat0_Sensor1_H=3930;      //左传感器的高信号值
	g_Mat0_Sensor1_L=165;      //左传感器的低信号值 
	g_Mat0_Sensor2_H=3930;      //右传感器的高信号值
	g_Mat0_Sensor2_L=165;      //右传感器的低信号值 
	Mat0EPC12=3;

	g_Mat1_Sensor1_H=3930;      //左传感器的高信号值
	g_Mat1_Sensor1_L=165;      //左传感器的低信号值 
	g_Mat1_Sensor2_H=3930;      //右传感器的高信号值
	g_Mat1_Sensor2_L=165;      //右传感器的低信号值 
	Mat1EPC12=0;

	g_Mat2_Sensor1_H=3930;      //左传感器的高信号值
	g_Mat2_Sensor1_L=165;      //左传感器的低信号值 
	g_Mat2_Sensor2_H=3930;      //右传感器的高信号值
	g_Mat2_Sensor2_L=165;      //右传感器的低信号值 
	Mat2EPC12=0;

	g_Mat3_Sensor1_H=3930;      //左传感器的高信号值
	g_Mat3_Sensor1_L=165;      //左传感器的低信号值 
	g_Mat3_Sensor2_H=3930;      //右传感器的高信号值
	g_Mat3_Sensor2_L=165;      //右传感器的低信号值 
	Mat3EPC12=0;

	g_Mat4_Sensor1_H=3930;      //左传感器的高信号值
	g_Mat4_Sensor1_L=165;      //左传感器的低信号值 
	g_Mat4_Sensor2_H=3930;      //右传感器的高信号值
	g_Mat4_Sensor2_L=165;      //右传感器的低信号值 
	Mat4EPC12=0;
	
	g_Mat5_Sensor1_H=3930;      //左传感器的高信号值
	g_Mat5_Sensor1_L=165;      //左传感器的低信号值 
	g_Mat5_Sensor2_H=3930;      //右传感器的高信号值
	g_Mat5_Sensor2_L=165;      //右传感器的低信号值 
	Mat5EPC12=0;

	g_Mat6_Sensor1_H=3930;      //左传感器的高信号值
	g_Mat6_Sensor1_L=165;      //左传感器的低信号值 
	g_Mat6_Sensor2_H=3930;      //右传感器的高信号值
	g_Mat6_Sensor2_L=165;      //右传感器的低信号值 
	Mat6EPC12=0;

	g_Mat7_Sensor1_H=3930;      //左传感器的高信号值
	g_Mat7_Sensor1_L=165;      //左传感器的低信号值 
	g_Mat7_Sensor2_H=3930;      //右传感器的高信号值
	g_Mat7_Sensor2_L=165;      //右传感器的低信号值 
	Mat7EPC12=0;
}

void SetDataInit()                 //设置参数初始化
{
	u8 i;
	gCheckFlag = 0xee55;		 //校验标志
	gIwdgFlag = 0;               //触发独立看门狗计数
	gBackupFlag = 0;             //是否有备份，1表示有备份，0表示没有备份
	
	gWorkMode=0;                 //0=手动   1=自动  2=对中
	gSensorMode=0;               //纠偏模式，0=左，1=右，2=左+右，3=SPC  ，EPC1超声波传感器1工作，EPC2超声波传感器2工作，CPC两个传感器一起工作，SPC

	gAutoPolar=0;                //自动极性，0=负极，1=正极;
	gManuPolar=0;                //手动极性，0=负极，1=正极;
	gMotorType = 1;              //电机类型   0无刷电机4   1无刷电机5   2有刷电机
	gPowerOnMode  =0;            //开机工作模式，,0=手动，1=自动，2=中心，3=上次 
	gCurMatNum=0;                //默认材料

	gGainData = 40;              //系统调整灵敏度;
	gDeadZone = 1;               //盲区设置，传感器识别盲区，当材料边沿有缺口时，防止抖动
	gFineTune = 80;              //微调=补偿，当需要微调材料位置时候，需要改变传感器位置，通过设置微调参数，避免调整传感器位置

	gAutoSpeed = 1800;            //自动速度
	gManuSpeed = 1800;            //手动速度
	gCentSpeed = 1800;            //对中速度
	
	gCaliTorque=10;
	gFuncTorque = 5;             //正常支行时转矩限制，最大为10A
	gCurrentPulseNum=200;        //当前所在位置脉冲数
	gMAXPulseNum=500;            //行程最大脉冲数

	gLimitMode = 0;              //限位方式   0为内部限位  1为外部限位开关限位   2内部加外部
	gBehindLimit=95;             //前限位点设置
	gCenterLimit=50;		     //中心限位点设置
	gFrontLimit=5;			     //后限位点设置

	gNoWaitEN=1;                 //无料等待功能，0=使能,1=禁止
	gNoDetectValve=95;           //无料检测阀值  
	gNoDetectTime=1;             //无料检测时间=0.1秒
	gNoWaitTime=15;              //无料等待时间=1.5秒  

	gSPCMode = 0;                //蛇形纠偏模式  0：内部编码器  1：外部传感器
	gSPCStopTime=2;              //spc停止时间=0.2秒
	gSPCBehindLimit=90;          //spc伸出停止点
	gSPCFrontLimit=10;           //spc缩进停止点

	gLongIo0Mode=0;              //远程输入口1状态 
	gLongIo1Mode=0;              //远程输入口2状态
	gLongIo2Mode=0;              //远程输入口3状态
	gLongIo3Mode=0;              //远程输入口4状态

	gAFlexAcc=AFlexAcc;          //自动柔性加速
	gAFlexDec=AFlexDec;          //自动柔性减速
	gMFlexAcc=MFlexAcc;          //手动柔性加速
	gMFlexDec=MFlexDec;          //手动柔性减速
	
	gAFlexSacc=0; 
	gAFlexSdec=0; 
	gMFlexSacc=0;
	gMFlexSdec=0;

	gLanguageChoice=0;           //中文
	gLockTime=3;
	gBrightness = 4;
	gBuzzer = 1;
	for(i=0;i<6;i++)
	{
		Password[i]=i+0x31;
	}
}

void ParameterInit()             //参数出厂设置
{
	SetDataInit();               //设置参数初始化
	SensorInit();                //传感器参数初始化
//	Clearing();                  //清零备份
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
	u8 icnt;
	pdata=pdata;
	
    BSP_Init();   
    queue_reset();       		//清空串口接收缓冲区    
    for(icnt=0;icnt<FunNum;icnt++)
	{
		LongPortFun[icnt] = 0;
	}
	for(icnt=0;icnt<WarmNum;icnt++)
	{
		Warm[icnt] = 0;
	}
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)!= RESET)         //判断是否发生了独立看门狗复位
	{
		RCC->CSR = 1<<24;    	/*Clear reset flag*/
		gIwdgFlag++;          	//发生复位，__noinit__修饰的全局变量会保持复位之前的数据，不会清零
		PBout(12)=1;	
	}else{                      //上电复位
		ReadPara();           	//读取flash中保存的参数
		ReadSensor();
		PBout(12)=0;
	}
	
	if(gCheckFlag!=0xee55)  			SetDataInit();  	 //如果参数读取错误，就重新初始化参数
	if(g_Mat0_Sensor1_H == 0xffff)  	SensorInit();		 //如果参数读取错误，就重新初始化传感器参数
	
	switch(gPowerOnMode)             //查询开机时的工作状态
	{
	   case 0: gWorkMode=0;    SetScreen(Manu_ID);        break;      //开机手动
	   case 1: gWorkMode=1;    SetScreen(Auto_ID);        break;      //开机时为自动状态
	   case 2: gWorkMode=2;    SetScreen(Center_ID);      break;      //开机时为中心状态
	   case 3:                           
		   if(gWorkMode==0)        SetScreen(Manu_ID);
		   else if(gWorkMode==1)   SetScreen(Auto_ID);
	       else if(gWorkMode==2)   SetScreen(Center_ID);   break;     //恢复关机的状态
	   default: SetDataInit();    break;    //系统初始化          
	}
	
	switch(gMotorType)                  //查询电机类型
    {
        case BrushlessMotor4:           //如果选择的无刷电机就开CAP,和外部中断作为霍尔检测    4对级
			NUMBERTURN = 7500000;       //3次换相转过角度45度   （45/(t1+t2+t3)）*60/360  乘以60表示1分钟转过角度
		    SERIES     =   24;
		break;                   
        case BrushlessMotor5:                         //5对级
			NUMBERTURN = 6000000;
		    SERIES     =   30;
		break; 
        default:SetDataInit();          break;   //系统初始化  
    }
	
	/***********霍尔位置读取***********/
	LastHallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);   //读转子位置    合成GPIOC8|GPIOC7|GPIOC6 	                                                            
	
	InitLCD();              //画面初始化
#if (OS_TASK_STAT_EN > 0)   //允许统计任务
	OSStatInit();     
#endif	
	OS_ENTER_CRITICAL();             //进入临界区(开中断)
	//OSTaskCreate();                创建任务
	OSTaskCreate(readio_task, (void*)0, &READ_IO_TASK_STK[READ_IO_STK_SIZE-1],READ_IO_TASK_PRIO);
	OSTaskCreate(lcd_task, (void*)0, &LCD_TASK_STK[LCD_STK_SIZE-1],LCD_TASK_PRIO);
	OSTaskCreate(led_task, (void*)0, &LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);  //挂起开始任务
	OS_EXIT_CRITICAL();              //退出临界区(开中断)
}

/**-----------------readio_task任务---------------------**/
void readio_task(void *pdata)
{
//  如果定义为全局变量，psi_pid_task()优先级高于si_pid_task()优先级,SetPwmValue可能被高优先级的任务更改，从而出错            
//	Wwdg_Init(0x7f,0x7d,3);
	IWDG_Init(7,312);   		  //7表示256分频，256/40K=6.4ms,6.4*312=2s表示喂狗间隔2s
	for(;;)
	{
		ReadLong(LongPortFun);
		delay_ms(10);
		IWDG_Feed();   		 	//2s之内没有喂狗则复位
	}
}

/**-----------------LED3任务---------------------**/
void lcd_task(void *pdata)
{
    qsize  size = 0;  

	for(;;)
	{
		size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                              //从缓冲区中获取一条指令 

        if(size>0&&cmd_buffer[1]!=0x07)                              //接收到指令 ，及判断是否为开机提示
        {                    
            ProcessMessage((PCTRL_MSG) cmd_buffer, size);                            //指令处理  
        }                                                                           
        else if(size>0&&cmd_buffer[1]==0x07)                         //如果为指令0x07就软重置STM32  
        {                                                                           
            __disable_fault_irq();                                                   
            NVIC_SystemReset();                                                                                                                                          
        }                                                                            

        //    特别注意
        //    MCU不要频繁向串口屏发送数据，否则串口屏的内部缓存区会满，从而导致数据丢失(缓冲区大小：标准型8K，基本型4.7K)
        //    1) 一般情况下，控制MCU向串口屏发送数据的周期大于100ms，就可以避免数据丢失的问题；
        //    2) 如果仍然有数据丢失的问题，请判断串口屏的BUSY引脚，为高时不能发送数据给串口屏。

        //    TODO: 添加用户代码
        //    数据有更新时标志位发生改变，定时100毫秒刷新屏幕
		
		//画面更新
		UpdateUI();
		
		delay_ms(20);
	}
}



/**-----------------LED任务---------------------**/
void led_task(void *pdata)
{
	Interrupt_Config();
	for(;;)
	{
//		PBout(12)=1;
//		delay_ms(500);
//		PBout(12)=0;
		delay_ms(500);
	}
}



