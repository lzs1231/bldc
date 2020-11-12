#ifndef __LCDTEST_H
#define __LCDTEST_H

#include "sys.h"


enum LcdIDType
{	
	Boot_InterfaceID   =0           ,     //开机界面
	Company_Information           ,
	/*自动菜单*/
	Auto_ID                 ,
	Polar_A_AdjustID             ,
	Gain_A_AdjustID              ,
	Offset_A_AdjustID            ,
	
	/*手动菜单*/
	Manu_ID,
	
	/*中心菜单*/
	Center_ID           ,
	Center_AdjustID               ,
	
	/*快捷菜单*/
	Shoutcut_ID                  ,  
	/*快捷菜单    向导*/
	Wizard_ID,            //向导 
	Manu_W_ID,            //手动操作
	Sensor_W_SetID,       //传感器设置
	Test_Manu_W_ID,       //手动试运行
	Test_Auto_W_ID,       //自动试运行
	Polar_W_AdjustID,
	Gain_W_AdjustID               ,
	
	/*快捷菜单    参数设置*/

	Parm_Set_ID                   ,
	/*快捷菜单    参数设置    纠偏参数设置*/
	Rec_P_SetID                   ,
	
	Mode_Rec_P_SetID              ,         //纠偏模式
	
	Gain_Rec_P_SetID              ,         //纠偏增益
	Gain_Rec_P_AdjustID           ,
	Dead_Rec_P_AdjustID           ,
		
	Speed_Rec_P_SetID             ,        //纠偏速度
	MSpeed_Rec_P_AdjustID         ,
	ASpeed_Rec_P_AdjustID         ,
	CSpeed_Rec_P_AdjustID         ,
	
	AccDec_Rec_P_SetID            ,                //纠偏加减速
	AAccSpeed_Rec_P_AdjustID      ,
	ADecSpeed_Rec_P_AdjustID      ,
	MAccSpeed_Rec_P_AdjustID      ,
	MDecSpeed_Rec_P_AdjustID      ,
	
	Current_Rec_P_SetID           ,      //  =40纠偏电流
    Current_Rec_P_AdjustID        ,
	
	Limit_Rec_P_SetID             ,  //
	Behind_Rec_P_SetID            ,
	Center_Rec_P_SetID            ,
	Front_Rec_P_SetID             ,
	
	SPC_Rec_P_SetID               ,      //纠偏SPC
	SPCStop_Rec_P_AdjustID        ,
	SPCBehind_Rec_P_AdjustID      ,
	SPCFront_Rec_P_AdjustID       ,
	
	NO_Rec_P_SetID                ,      //纠偏无料
	NOValue_Rec_P_AdjustID        ,
	NODetect_Rec_P_AdjustID       ,
	NOWait_Rec_P_AdjustID         ,
	
	PolarityID                    ,        // 极性          
	
	/*快捷菜单    参数设置    传感器参数设置*/
	Sensor_P_SetID                ,
	Offset_Sensor_P_AdjustID      ,
	/*快捷菜单    参数设置    推动器参数设置*/
	Motor_P_SetID                 ,
 
	/*快捷菜单    参数设置    系统参数设置*/
	Sys_P_SetID                   ,
	
	Brightness_Sys_P_SetID        ,
	BuzzerSetID                   ,
	Password_SetID                ,     //密码设置界面
    LanguageSetID                 ,
	Power_on_StateID              ,     //开机状态
	About_NativeID                ,     //关于本机
	RestoreBackupID               ,     //还原备份
	SYSInitAffirmID               ,     //确认还原备份
	SYSInitDisID                  ,     //还原备份显示中
	
	/*快捷菜单    端口定义*/
	Port_Sys_P_SetID              ,
	
	/*******密码键盘******/
	Password_ProtectionID         ,     //密码保护界面
	
	/*******确认参数修改******/
	Confirm_ID                    ,
	
	/*******传感器校准界面******/
	Material_SwitchID,            //校准传感器或选择材料
	MatValue_SwitchID,            //选择材料
	Sensor_Calibr1ID,             //选择EPC1或EPC2进行校准
	Ask_to_SwitchID ,             //询问是否切换
	Sensor_Calibr2ID,
	Sensor_Calibr3ID,
	Sensor_Calibr4ID,
		
	/*********行程自适应********/	
	Travel_SetID        		  ,   
	Travel_AdaptationID           ,	
	
	SPCInsideID,
	SPCExternalID,
	
};

enum LanguageType
{
	Chinese  = 0,
	English,
};

/********极性*******/
enum PolarType
{
	Forward  = 0,
	Reverse,
};

typedef  struct  MyPolar{
//	enum PolarType MenuCount;
	u16 *PPolarA;
	u16 *PPolarM;
	u8 *DisStr[4];          //常量指针数组，指向常量空间
	u8 *DisStrSymbol[2];
}PolarDef;

/********加减速默认或自定义*******/
enum  AccDecType                  //端口定义
{
	DefaultDef,         //默认
	Custom,             //自定义
};

/********电机类型*******/
enum MotorType
{
	BrushlessMotor4   = 0,
	BrushlessMotor5     ,
};

/********限位模式*******/
enum LimitModeType
{
	Internal   = 0,
	External,
};

/********使能禁止*******/
enum StateYesNoType
{
	Disable   = 0,
	Enable,
};

/********开机状态*******/
enum PowerOnStateType
{
	ManuInterface,
	AutoInterface,
	CentInterface,
	LastInterface,
};

/********SPC工作模式*******/
enum SPCModeType
{
	EncoderI,
	SensorE,
};

/********备份还原*******/
enum ParaStorageType
{
	SBackup,
	SRestore,
	SFactorySet,
};

typedef struct MyParaStorage{
    enum ParaStorageType MenuCount;
	void (*fun[3])(void);    //(*fun[4])加括号表示指向函数的指针，不加括号表示返回void *类型的指针
}ParaStorageDef;

/********确认更改*******/
enum AdjustDataType
{
	d_Gain_Data,         //1~100
	d_Dead_Zone,         //0%~80%
	d_Auto_Speed,        //100~2000
	d_Manu_Speed,
	d_Cent_Speed,
	d_Fine_Tuning,       //0~160
	d_Behind_limit,      //5~95
	d_Center_limit,      //5~95
	d_Front_limit,       //5~95
	d_No_Detect_Valve,   //5~95
	d_No_Detect_Time,    //0.1~10s
	d_No_Wait_Time,      //0.1~10s
	d_SPC_stop_time,     //0.2~25s
	d_SPC_Behind_limit,  //10~90
	d_SPC_Front_limit,	 //10~90
	d_AFlex_acc,         //5~100
	d_AFlex_dec,         //5~100
	d_MFlex_acc,         //5~100
	d_MFlex_dec,         //5~100
	d_BrightnessDis,     //1~100
	d_Function_Torque,
	d_Number,
};

typedef struct MyAdjustData{
	u16 LowerLimit;
	u16 UpperLimit;
	u16 *tData;
	u8  StepData;
}AdjustDataDef;

typedef struct MyConfirmData{
	enum AdjustDataType MenuCount;         //需要修改参数序号
	u8   ExitFlag;                         //取消标志，表明需要还原数据
	u16  TempData;                         //用于暂存的变量
	AdjustDataDef AdjustData[d_Number];    //参数的属性：上限、下限、保存地址
	u16 BackInface;                        //返回的界面ID
}ConfirmDataDef;



/******延时跳转到指定界面*******/
typedef struct MyDelayInface{
	u8  KeyDwon;             //按键状态
	u8  DelayCnt;            //按键按下计数
	u16 BackInface;          //跳转的界面ID
}DelayInfaceDef;

typedef struct MyDelayInfaceGroup{
	u8 Num;
	DelayInfaceDef DelayInface[5];            
}DelayInfaceGroupDef;

/********息屏时间*******/
enum  BrightnessType
{
	Brightness20=0,
	Brightness40,
	Brightness60,
	Brightness80,
	Brightness100,
};
enum  LockTimeType
{
	Second_30=0,
	Minute_1,
	Minute_5,
	Not_lock,
};

typedef  struct  MyLockTime{
	u16  *PBrightness;         //屏幕亮度
	u8  BrightnessArray[5];
	u16  *PLockTime;         //指向锁屏时间序号
	u16  LockTimeArray[4];   //时间选择
}LockTimeDef;

/*******密码处理******/
enum  PasswordType
{
	InputPasswordID=0,       //输入密码界面调用键盘
	ChangePasswordID,        //修改密码界面调用键盘
	RepeatInputID,           //重复密码界面调用键盘
};

typedef struct MyPassword
{
	u8 CallID;               //根据调用界面CallID判断执行哪个功能函数
	u8 ChangeFlag;  
	u8 *PPassword;
	u8 t_Password[7];
	void (*fun[3])(u8 *);	//(*fun[4])加括号表示指向函数的指针，不加括号表示返回void *类型的指针
}PasswordSetDef;

/********传感器模式*******/
enum SensorModeType
{
	EPC1,
	EPC2,
	CPC,
	SPC,
};

typedef  struct  MySensorMode{
//	enum SensorModeType MenuCount;
	u16 *PSensorMode;
	u8 *DisStr[4];           //常量指针数组，指向常量空间
	u8 *DisStr1[4];          //常量指针数组，指向常量空间
	void (*fun[4])(void);
}SensorModeDef;

/********传感器校准选择*******/
typedef  struct  MyMaterial{
	u16 *PSensor1_H;  //每个材料对应的传感器值
	u16 *PSensor1_L;
	u16 *PSensor2_H;  
	u16 *PSensor2_L;
	u16  *PEPC12;                 //显示传感器校准图标
}MaterialDef;

typedef  struct  MyMaterialGroup{               
	u16 BackInf;              //校准完成之后返回的界面
	u8  EPCNum;                //传感器编号
	MaterialDef MatValue[8];  //8个材料
	u16 *CurMatNum;              //材料编号
	u8  CaliFlag;               //校准完成标志
	u16 t_Sensor1_valueL;     //暂存左传感器小值
	u16 t_Sensor1_valueH;     //暂存左传感器大值
	u16 t_Sensor2_valueL;
	u16 t_Sensor2_valueH;
}MatCalDef;

/******校准*******/
typedef struct MyTravelCal{
	u16 BackInf;              //校准完成之后返回的界面
	u8  CaliFlag;        //启动校准标志
	u8  CaliStep;        //校准步骤
	int StallDir;       //堵转方向
	u16  *CalCurrent;
}TravelCalDef;



extern MatCalDef MatCal;
extern TravelCalDef TravelCal;
void SensorEPC1Dis(void);    //EPC1
void SensorEPC2Dis(void);    //EPC2
void SensorCPCDis(void);   //CPC
void SensorSPCDis(void);   //SPC

void InputPassword(u8 *str);
void ChangePassword(u8 *str);
void RepeatInput(u8 *str);


#endif
