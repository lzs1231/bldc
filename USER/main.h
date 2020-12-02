#ifndef __MAIN_H__
#define __MAIN_H__

#include "longkey.h"
#include "sys.h"

extern u16 *const pGainDead 	 ;
extern u16 *const pFineTune	 	;
extern u16 *const pDisPulseNum  ;
extern u16 *const pSensorRate   ;
extern u16 *const pSensorValue  ;
extern u16 *const pMatDis       ;
extern u16 *const pLimitFun1    ;
extern u16 *const pLimitFun2     ;
extern u16 *const pSPCFun1		 ;
extern u16 *const pSPCFun2		 ;
extern u16 *const pNoMatFun1 	 ;
extern u16 *const pNoMatFun2 	 ;
extern u16 *const pAFlex 		 ;
extern u16 *const pMFlex 		 ;
extern u16 *const pAutoSpeed 	 ;
extern u16 *const pManuSpeed	 ;
extern u16 *const pCentSpeed 	 ;
extern u16 *const pCurrentPara   ;
extern u16 *const pAdjustData	 ;
extern u16 *const pPortFun	 	;
extern u16 *const pFunc	 	;

extern u16 *const pHAdjustFlag  ;
extern u16 *const pHClickBut    ;
extern u16 *const pHAddSub      ;
extern u16 *const pHParaConfirm ;
extern u16 *const pHSCaliStep   ;
extern u16 *const pHSensorNum   ;
extern u16 *const pHTCaliFlag   ;
extern u16 *const pHTCaliCancel ;
extern u16 *const pHFlexSpeed	;    //柔性加减速值
extern u16 *const pHSysYes		;    //确定执行
extern u16 *const pHWarmFlag	;   //警告标志

extern u16 *const pHWorkMode    ;
extern u16 *const pHMatNum 	    ;  // 当前材料编号
extern u16 *const pHTCaliTorque ;

extern u16 *const pHLimitMode   ;
extern u16 *const pHSPCMode     ;
extern u16 *const pHNoWaitEN    ;
extern u16 *const pHAutoPolar   ;
extern u16 *const pHManuPolar   ;


extern u16 *const pHSensorMode  ;
extern u16 *const pHMotorType   ;
extern u16 *const pHPowerOnMode ;
extern u16 *const pHAlarmSwitch  ;
extern u16 *const pHRelay;	
extern u16 *const pHSignal 	;
extern u16 *const pHKeepWait ;
extern u16 *const pHPortFun0 	;
extern u16 *const pHPortFun1 	;
extern u16 *const pHPortFun2 	;
extern u16 *const pHPortFun3 	;

//以下是要存储的参数
extern  u16   gCheckFlag;		         //校验标志
extern  u16   gIwdgFlag;                 //看门狗复位标志
extern  u16   gBackupFlag;    		

extern  u16   gWorkMode;                 //工作模式：0手动，1自动,2中心
extern  u16   gSensorMode;               //纠偏模式，1=左EPC1，2=右EPC2，3=左+右CPC，4=SPC
extern  u16   gSensorSignal;

extern  u16   gAutoPolar;                //自动极性，1=负极，0=正极;
extern  u16   gManuPolar;                //手动极性，1=负极，0=正极;
extern  u16   gMotorType;                //电机类型
extern  u16   gPowerOnMode;              //开机工作模式，,0=手动，1=自动，2=中心，3=上次 
extern  u16   gCurMatNum;               //默认材料

extern  u16   gGainData;                 //系统调整灵敏度,增益;
extern  u16   gDeadZone;                 //盲区设置，传感器识别盲区，当材料边沿有缺口时，防止抖动
extern  u16   gFineTune;                 //微调=补偿;

extern  u16   gAutoSpeed;                //自动速度
extern  u16   gManuSpeed;                //手动速度
extern  u16   gCentSpeed;                //对中速度

extern  u16   gCaliTorque;            	//校准转矩
extern  u16   gFuncTorque;              //正常支行时转矩限制，最大为10A

extern  s16   gCurrentPulseNum;         //当前所在位置脉冲数	
extern  s16   gMAXPulseNum;             //脉冲数0到最大值MAX_pulse_num

extern  u16   gLimitMode;                //限位方式
extern  u16   gExtendLimit;              //前限位点设置
extern  u16   gCenterLimit;		    	 //中心限位点设置
extern  u16   gIndentLimit;				 //后限位点设置

extern  u16   gSPCMode;                  //蛇形纠偏模式  0：内部编码器  1：外部传感器
extern  u16   gSPCStopTime;              //sps停止时间=0.2秒
extern  u16   gSPCExtendLimit;
extern  u16   gSPCIndentLimit;

extern  u16   gNoWaitEN;               //无料等待功能0=禁止，1=使能
extern  u16   gNoDetectValve;          //无料检测阀值  
extern  u16   gNoDetectTime;           //无料检测时间=0.1秒
extern  u16   gNoWaitTime;             //无料等待时间=1.5秒  

extern  u16   gLongIo0Mode;            //远程输入口1状态 
extern  u16   gLongIo1Mode;            //远程输入口2状态
extern  u16   gLongIo2Mode;            //远程输入口3状态
extern  u16   gLongIo3Mode;            //远程输入口4状态 

extern  u16   gAFlexAcc;                  //自动柔性加速
extern  u16   gAFlexDec;                  //自动柔性减速
extern  u16   gMFlexAcc;                  //手动柔性加速
extern  u16   gMFlexDec;                  //手动柔性减速     

extern  u16   gAlarmSwitch;
extern  u16   gRelay;
extern  u16   gKeepWait;

extern  u16   g_Mat0_SensorL_H;      //左传感器的高信号值
extern  u16   g_Mat0_SensorL_L;      //左传感器的低信号值 
extern  u16   g_Mat0_SensorR_H;      //右传感器的高信号值
extern  u16   g_Mat0_SensorR_L;      //右传感器的低信号值 
extern  u16    Mat0EPC12;

extern  u16   g_Mat1_SensorL_H;      //左传感器的高信号值
extern  u16   g_Mat1_SensorL_L;      //左传感器的低信号值 
extern  u16   g_Mat1_SensorR_H;      //右传感器的高信号值
extern  u16   g_Mat1_SensorR_L;      //右传感器的低信号值 
extern  u16    Mat1EPC12;

extern  u16   g_Mat2_SensorL_H;      //左传感器的高信号值
extern  u16   g_Mat2_SensorL_L;      //左传感器的低信号值 
extern  u16   g_Mat2_SensorR_H;      //右传感器的高信号值
extern  u16   g_Mat2_SensorR_L;      //右传感器的低信号值 
extern  u16    Mat2EPC12;

extern  u16   g_Mat3_SensorL_H;      //左传感器的高信号值
extern  u16   g_Mat3_SensorL_L;      //左传感器的低信号值 
extern  u16   g_Mat3_SensorR_H;      //右传感器的高信号值
extern  u16   g_Mat3_SensorR_L;      //右传感器的低信号值 
extern  u16    Mat3EPC12;

extern  u16   g_Mat4_SensorL_H;      //左传感器的高信号值
extern  u16   g_Mat4_SensorL_L;      //左传感器的低信号值 
extern  u16   g_Mat4_SensorR_H;      //右传感器的高信号值
extern  u16   g_Mat4_SensorR_L;      //右传感器的低信号值 
extern  u16    Mat4EPC12;

extern  u16   g_Mat5_SensorL_H;      //左传感器的高信号值
extern  u16   g_Mat5_SensorL_L;      //左传感器的低信号值 
extern  u16   g_Mat5_SensorR_H;      //右传感器的高信号值
extern  u16   g_Mat5_SensorR_L;      //右传感器的低信号值 
extern  u16    Mat5EPC12;

extern  u16   g_Mat6_SensorL_H;      //左传感器的高信号值
extern  u16   g_Mat6_SensorL_L;      //左传感器的低信号值 
extern  u16   g_Mat6_SensorR_H;      //右传感器的高信号值
extern  u16   g_Mat6_SensorR_L;      //右传感器的低信号值 
extern  u16    Mat6EPC12;

extern  u16   g_Mat7_SensorL_H;      //左传感器的高信号值
extern  u16   g_Mat7_SensorL_L;      //左传感器的低信号值 
extern  u16   g_Mat7_SensorR_H;      //右传感器的高信号值
extern  u16   g_Mat7_SensorR_L;      //右传感器的低信号值 
extern  u16    Mat7EPC12;



/*******报警标志******/
enum   WarmFlagType
{
	NoErrFlag,
	LockFlag,        //锁定标志
	RunReadyFlag,    //运行准备
	ProhibitFlag,    //禁止切换到中心状态
	BreakAlarmFlag,  //断料报警标志  
	LimitFlag,       //限位报警
	OverrunFlag,     //电流超限
	StallFlag,       //电机堵转
	NotConFlag,      //霍尔故障   8
	ComErrFlag,      //通信错误
	VolHighFlag,     //电压过高
	VolLowFlag,      //电压过低  11
	WarmNum,
};

extern	u8 	 Warm[WarmNum];	
extern  u16  LongPortFun[FunNum];
extern  u16  ClickButton;           //点动按键属性

extern  volatile u16  g_ADC_Buf[4];

extern  u32  time0,time1,time2;
extern  u16  SensorL_value;                  //传感器1的值
extern  u16  SensorR_value;                  //传感器2的值

extern  u32  NUMBERTURN;
extern  u8   SERIES;

extern  u8   LastHallData;
extern  u16  HallRate;             //当前脉冲点总行程中的比例


/* 私有类型定义 --------------------------------------------------------------*/
typedef enum
{
	CW=-1,       // 逆时针方向 反转
	CCW=1        // 顺时钟方向 正转
}MOTOR_DIR;

typedef enum 
{
	STOP=0,    // 停止
	RUN=1      // 运转
}MOTOR_STATE;

typedef struct MyMotorType
{
	__IO int          motor_speed;        // 电机转速(RPM):0..2500
	__IO MOTOR_STATE  motor_state;        // 电机旋转状态，停止指令到来之后开始刹车，过一段时间电机才会停止
	__IO MOTOR_STATE  order_state;        // 指令旋转状态
	__IO MOTOR_DIR    motor_direction;    // 电机转动方向，方向也会延迟于指令方向
	__IO MOTOR_DIR    order_direction;    // 指令转动方向
	__IO uint32_t     step_counter;       // 霍尔传感器步数，用于测量电机转速
	__IO uint16_t     stalling_count;     // 停机标志，如果该值超2000，认为电机停止旋转
}MotorDev;

extern MotorDev bldc_dev;

void ParameterInit(void);   

#endif



