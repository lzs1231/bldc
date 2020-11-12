#ifndef __LCDTEST_H
#define __LCDTEST_H

#include "sys.h"


enum LcdIDType
{	
	Boot_InterfaceID   =0        ,     //��������
	Company_Information          ,
	/*�Զ��˵�*/
	Auto_ID                      ,
	Gain_A_SetID              ,
	
	/*�ֶ��˵�*/
	Manu_ID,
	
	/*���Ĳ˵�*/
	Center_ID                    ,
	
	/*��ݲ˵�*/
	Shoutcut_ID                  ,  
	/*��ݲ˵�    ��*/
	Wizard_ID,            //�� 
	Manu_W_ID,            //�ֶ�����
	Sensor_W_SetID,       //����������
	Test_Manu_W_ID,       //�ֶ�������
	Test_Auto_W_ID,       //�Զ�������
	/*��ݲ˵�    ��������*/

	Parm_Set_ID                   ,
	/*��ݲ˵�    ��������    ��ƫ��������*/
	Rec_P_SetID                   ,
	
	Mode_Rec_P_SetID              ,         //��ƫģʽ
	
	Gain_Rec_P_SetID              ,         //��ƫ����
		
	Speed_Rec_P_SetID             ,        //��ƫ�ٶ�
	
	AccDec_Rec_P_SetID            ,                //��ƫ�Ӽ���
	
	Current_Rec_P_SetID           ,      //  =40��ƫ����
	
	Limit_Rec_P_SetID             ,  //
	
	SPC_Rec_P_SetID               ,      //��ƫSPC
	
	NO_Rec_P_SetID                ,      //��ƫ����
	
	PolarityID                    ,        // ����          
	
	/*��ݲ˵�    ��������    ��������������*/
	Sensor_P_SetID                ,

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
	AdjustID                      ,     //���ڽ���
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
typedef struct MyAdjustData{
	u16 LowerLimit;
	u16 UpperLimit;
	u16 *const tData;
	u16  StepData;
}AdjustDataDef;

typedef struct MyConfirmData{
	u8   ExitFlag;                         //ȡ����־��������Ҫ��ԭ����
	u16  TempData;                         //�����ݴ�ı���
	AdjustDataDef AdjustData[25];    //���������ԣ����ޡ����ޡ������ַ
}ConfirmDataDef;

/********������ģʽ*******/
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

/********������У׼ѡ��*******/
typedef  struct  MyMaterial{
	u16 *PSensor1_H;  //ÿ�����϶�Ӧ�Ĵ�����ֵ
	u16 *PSensor1_L;
	u16 *PSensor2_H;  
	u16 *PSensor2_L;
	u16  *PEPC12;                 //��ʾ������У׼ͼ��
}MaterialDef;

typedef  struct  MyMaterialGroup{               
	MaterialDef MatValue[8];  //8������
	u16 *CurMatNum;              //���ϱ��	
	u8  CaliFlag;              //У׼��ɱ�־
	u16 t_Sensor1_valueL;     //�ݴ��󴫸���Сֵ
	u16 t_Sensor1_valueH;     //�ݴ��󴫸�����ֵ
	u16 t_Sensor2_valueL;
	u16 t_Sensor2_valueH;
	u8 (*fun[3])(void);
}MatCalDef;

/******У׼*******/
typedef struct MyTravelCal{
	u8  CaliFlag;        //����У׼��־
	u8  CaliStep;        //У׼����
	int StallDir;        //��ת����
}TravelCalDef;


extern SensorModeDef SensorMode;
extern MatCalDef MatCal;
extern TravelCalDef TravelCal;
extern ConfirmDataDef ConfirmData;

/************4�ִ�����ģʽ��Ӧ����ʾ����***************/
void SensorEPC1Dis(void);    //EPC1
void SensorEPC2Dis(void);    //EPC2
void SensorCPCDis(void);   //CPC
void SensorSPCDis(void);   //SPC

/**************������У׼������ʾ����*******************/
u8 NoShelterDis(void);
u8 ShelterDis(void);
u8 RealTimeDis(void);

/*****************���ݻ�ԭ********************/
void BackupRestore(void)  ; 


void AddSub(void);
void Confirm(void);

#endif
