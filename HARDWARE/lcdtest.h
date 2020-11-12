#ifndef __LCDTEST_H
#define __LCDTEST_H

#include "sys.h"


enum LcdIDType
{	
	Boot_InterfaceID   =0           ,     //��������
	Company_Information           ,
	/*�Զ��˵�*/
	Auto_ID                 ,
	Polar_A_AdjustID             ,
	Gain_A_AdjustID              ,
	Offset_A_AdjustID            ,
	
	/*�ֶ��˵�*/
	Manu_ID,
	
	/*���Ĳ˵�*/
	Center_ID           ,
	Center_AdjustID               ,
	
	/*��ݲ˵�*/
	Shoutcut_ID                  ,  
	/*��ݲ˵�    ��*/
	Wizard_ID,            //�� 
	Manu_W_ID,            //�ֶ�����
	Sensor_W_SetID,       //����������
	Test_Manu_W_ID,       //�ֶ�������
	Test_Auto_W_ID,       //�Զ�������
	Polar_W_AdjustID,
	Gain_W_AdjustID               ,
	
	/*��ݲ˵�    ��������*/

	Parm_Set_ID                   ,
	/*��ݲ˵�    ��������    ��ƫ��������*/
	Rec_P_SetID                   ,
	
	Mode_Rec_P_SetID              ,         //��ƫģʽ
	
	Gain_Rec_P_SetID              ,         //��ƫ����
	Gain_Rec_P_AdjustID           ,
	Dead_Rec_P_AdjustID           ,
		
	Speed_Rec_P_SetID             ,        //��ƫ�ٶ�
	MSpeed_Rec_P_AdjustID         ,
	ASpeed_Rec_P_AdjustID         ,
	CSpeed_Rec_P_AdjustID         ,
	
	AccDec_Rec_P_SetID            ,                //��ƫ�Ӽ���
	AAccSpeed_Rec_P_AdjustID      ,
	ADecSpeed_Rec_P_AdjustID      ,
	MAccSpeed_Rec_P_AdjustID      ,
	MDecSpeed_Rec_P_AdjustID      ,
	
	Current_Rec_P_SetID           ,      //  =40��ƫ����
    Current_Rec_P_AdjustID        ,
	
	Limit_Rec_P_SetID             ,  //
	Behind_Rec_P_SetID            ,
	Center_Rec_P_SetID            ,
	Front_Rec_P_SetID             ,
	
	SPC_Rec_P_SetID               ,      //��ƫSPC
	SPCStop_Rec_P_AdjustID        ,
	SPCBehind_Rec_P_AdjustID      ,
	SPCFront_Rec_P_AdjustID       ,
	
	NO_Rec_P_SetID                ,      //��ƫ����
	NOValue_Rec_P_AdjustID        ,
	NODetect_Rec_P_AdjustID       ,
	NOWait_Rec_P_AdjustID         ,
	
	PolarityID                    ,        // ����          
	
	/*��ݲ˵�    ��������    ��������������*/
	Sensor_P_SetID                ,
	Offset_Sensor_P_AdjustID      ,
	/*��ݲ˵�    ��������    �ƶ�����������*/
	Motor_P_SetID                 ,
 
	/*��ݲ˵�    ��������    ϵͳ��������*/
	Sys_P_SetID                   ,
	
	Brightness_Sys_P_SetID        ,
	BuzzerSetID                   ,
	Password_SetID                ,     //�������ý���
    LanguageSetID                 ,
	Power_on_StateID              ,     //����״̬
	About_NativeID                ,     //���ڱ���
	RestoreBackupID               ,     //��ԭ����
	SYSInitAffirmID               ,     //ȷ�ϻ�ԭ����
	SYSInitDisID                  ,     //��ԭ������ʾ��
	
	/*��ݲ˵�    �˿ڶ���*/
	Port_Sys_P_SetID              ,
	
	/*******�������******/
	Password_ProtectionID         ,     //���뱣������
	
	/*******ȷ�ϲ����޸�******/
	Confirm_ID                    ,
	
	/*******������У׼����******/
	Material_SwitchID,            //У׼��������ѡ�����
	MatValue_SwitchID,            //ѡ�����
	Sensor_Calibr1ID,             //ѡ��EPC1��EPC2����У׼
	Ask_to_SwitchID ,             //ѯ���Ƿ��л�
	Sensor_Calibr2ID,
	Sensor_Calibr3ID,
	Sensor_Calibr4ID,
		
	/*********�г�����Ӧ********/	
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

/********����*******/
enum PolarType
{
	Forward  = 0,
	Reverse,
};

typedef  struct  MyPolar{
//	enum PolarType MenuCount;
	u16 *PPolarA;
	u16 *PPolarM;
	u8 *DisStr[4];          //����ָ�����飬ָ�����ռ�
	u8 *DisStrSymbol[2];
}PolarDef;

/********�Ӽ���Ĭ�ϻ��Զ���*******/
enum  AccDecType                  //�˿ڶ���
{
	DefaultDef,         //Ĭ��
	Custom,             //�Զ���
};

/********�������*******/
enum MotorType
{
	BrushlessMotor4   = 0,
	BrushlessMotor5     ,
};

/********��λģʽ*******/
enum LimitModeType
{
	Internal   = 0,
	External,
};

/********ʹ�ܽ�ֹ*******/
enum StateYesNoType
{
	Disable   = 0,
	Enable,
};

/********����״̬*******/
enum PowerOnStateType
{
	ManuInterface,
	AutoInterface,
	CentInterface,
	LastInterface,
};

/********SPC����ģʽ*******/
enum SPCModeType
{
	EncoderI,
	SensorE,
};

/********���ݻ�ԭ*******/
enum ParaStorageType
{
	SBackup,
	SRestore,
	SFactorySet,
};

typedef struct MyParaStorage{
    enum ParaStorageType MenuCount;
	void (*fun[3])(void);    //(*fun[4])�����ű�ʾָ������ָ�룬�������ű�ʾ����void *���͵�ָ��
}ParaStorageDef;

/********ȷ�ϸ���*******/
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
	enum AdjustDataType MenuCount;         //��Ҫ�޸Ĳ������
	u8   ExitFlag;                         //ȡ����־��������Ҫ��ԭ����
	u16  TempData;                         //�����ݴ�ı���
	AdjustDataDef AdjustData[d_Number];    //���������ԣ����ޡ����ޡ������ַ
	u16 BackInface;                        //���صĽ���ID
}ConfirmDataDef;



/******��ʱ��ת��ָ������*******/
typedef struct MyDelayInface{
	u8  KeyDwon;             //����״̬
	u8  DelayCnt;            //�������¼���
	u16 BackInface;          //��ת�Ľ���ID
}DelayInfaceDef;

typedef struct MyDelayInfaceGroup{
	u8 Num;
	DelayInfaceDef DelayInface[5];            
}DelayInfaceGroupDef;

/********Ϣ��ʱ��*******/
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
	u16  *PBrightness;         //��Ļ����
	u8  BrightnessArray[5];
	u16  *PLockTime;         //ָ������ʱ�����
	u16  LockTimeArray[4];   //ʱ��ѡ��
}LockTimeDef;

/*******���봦��******/
enum  PasswordType
{
	InputPasswordID=0,       //�������������ü���
	ChangePasswordID,        //�޸����������ü���
	RepeatInputID,           //�ظ����������ü���
};

typedef struct MyPassword
{
	u8 CallID;               //���ݵ��ý���CallID�ж�ִ���ĸ����ܺ���
	u8 ChangeFlag;  
	u8 *PPassword;
	u8 t_Password[7];
	void (*fun[3])(u8 *);	//(*fun[4])�����ű�ʾָ������ָ�룬�������ű�ʾ����void *���͵�ָ��
}PasswordSetDef;

/********������ģʽ*******/
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
	u8 *DisStr[4];           //����ָ�����飬ָ�����ռ�
	u8 *DisStr1[4];          //����ָ�����飬ָ�����ռ�
	void (*fun[4])(void);
}SensorModeDef;

/********������У׼ѡ��*******/
typedef  struct  MyMaterial{
	u16 *PSensor1_H;  //ÿ�����϶�Ӧ�Ĵ�����ֵ
	u16 *PSensor1_L;
	u16 *PSensor2_H;  
	u16 *PSensor2_L;
	u16  *PEPC12;                 //��ʾ������У׼ͼ��
}MaterialDef;

typedef  struct  MyMaterialGroup{               
	u16 BackInf;              //У׼���֮�󷵻صĽ���
	u8  EPCNum;                //���������
	MaterialDef MatValue[8];  //8������
	u16 *CurMatNum;              //���ϱ��
	u8  CaliFlag;               //У׼��ɱ�־
	u16 t_Sensor1_valueL;     //�ݴ��󴫸���Сֵ
	u16 t_Sensor1_valueH;     //�ݴ��󴫸�����ֵ
	u16 t_Sensor2_valueL;
	u16 t_Sensor2_valueH;
}MatCalDef;

/******У׼*******/
typedef struct MyTravelCal{
	u16 BackInf;              //У׼���֮�󷵻صĽ���
	u8  CaliFlag;        //����У׼��־
	u8  CaliStep;        //У׼����
	int StallDir;       //��ת����
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
