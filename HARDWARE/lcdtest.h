#ifndef __LCDTEST_H
#define __LCDTEST_H

#include "sys.h"


enum LcdIDType
{	
	Boot_InterfaceID   =0        ,     //开机界面
	Company_Information          ,
	/*自动菜单*/
	Auto_ID                      ,
	Gain_A_SetID              ,
	
	/*手动菜单*/
	Manu_ID,
	
	/*中心菜单*/
	Center_ID                    ,
	
	/*快捷菜单*/
	Shoutcut_ID                  ,  
	/*快捷菜单    向导*/
	Wizard_ID,            //向导 
	Manu_W_ID,            //手动操作
	Sensor_W_SetID,       //传感器设置
	Test_Manu_W_ID,       //手动试运行
	Test_Auto_W_ID,       //自动试运行
	/*快捷菜单    参数设置*/

	Parm_Set_ID                   ,
	/*快捷菜单    参数设置    纠偏参数设置*/
	Rec_P_SetID                   ,
	
	Mode_Rec_P_SetID              ,         //纠偏模式
	
	Gain_Rec_P_SetID              ,         //纠偏增益
		
	Speed_Rec_P_SetID             ,        //纠偏速度
	
	AccDec_Rec_P_SetID            ,                //纠偏加减速
	
	Current_Rec_P_SetID           ,      //  =40纠偏电流
	
	Limit_Rec_P_SetID             ,  //
	
	SPC_Rec_P_SetID               ,      //纠偏SPC
	
	NO_Rec_P_SetID                ,      //纠偏无料
	
	PolarityID                    ,        // 极性          
	
	/*快捷菜单    参数设置    传感器参数设置*/
	Sensor_P_SetID                ,

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
	AdjustID                      ,     //调节界面
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

typedef struct 
{
	int turnnum;
	int series;
}MotorCharDef;

typedef struct 
{
	int MotorType;
	MotorCharDef MotorChar;
}MotorDef;


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
typedef struct MyAdjustData{
	u16 LowerLimit;
	u16 UpperLimit;
	u16 *const tData;
	u16  StepData;
}AdjustDataDef;

typedef struct MyConfirmData{
	u8   ExitFlag;                         //取消标志，表明需要还原数据
	u16  TempData;                         //用于暂存的变量
	AdjustDataDef AdjustData[25];    //参数的属性：上限、下限、保存地址
}ConfirmDataDef;

/********传感器模式*******/
enum SensorModeType
{
	EPC1,
	EPC2,
	CPC,
	SPC,
};

typedef  struct  MySensorMode{
	u16 *PSensorMode;
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
	MaterialDef MatValue[8];  //8个材料
	u16 *CurMatNum;              //材料编号	
	u8  CaliFlag;              //校准完成标志
	u16 t_Sensor1_valueL;     //暂存左传感器小值
	u16 t_Sensor1_valueH;     //暂存左传感器大值
	u16 t_Sensor2_valueL;
	u16 t_Sensor2_valueH;
	u8 (*fun[3])(void);
}MatCalDef;

/******校准*******/
typedef struct MyTravelCal{
	u8  CaliFlag;        //启动校准标志
	u8  CaliStep;        //校准步骤
	int StallDir;        //堵转方向
}TravelCalDef;


extern SensorModeDef SensorMode;
extern MatCalDef MatCal;
extern TravelCalDef TravelCal;
extern ConfirmDataDef ConfirmData;

/************4种传感器模式对应的显示程序***************/
void SensorEPC1Dis(void);    //EPC1
void SensorEPC2Dis(void);    //EPC2
void SensorCPCDis(void);   //CPC
void SensorSPCDis(void);   //SPC

/**************传感器校准界面显示程序*******************/
u8 NoShelterDis(void);
u8 ShelterDis(void);
u8 RealTimeDis(void);

/*****************备份还原********************/
void BackupRestore(void)  ; 


void AddSub(void);
void Confirm(void);

#endif
