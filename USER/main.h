#ifndef __MAIN_H__
#define __MAIN_H__

#include "longkey.h"
#include "sys.h"

//#define _TEST_DMA
//#define _TEST_Place        //���ٶ�PID���Բ�����ʾ
//#define _TEST_Speed          //���ٶ�PID���Բ�����ʾ
//#define _TEST_Cent         //�򿪶���PID���Բ�����ʾ
//#define _TEST_SPC          //��SPCPID���Բ�����ʾ

#define  AFlexAcc   50                //�Զ����Լ���
#define  AFlexDec   100               //�Զ����Լ���
#define  MFlexAcc   20                //�ֶ����Լ���
#define  MFlexDec   80                //�ֶ����Լ���


//������Ҫ�洢�Ĳ���
extern  u16   gCheckFlag;		           //У���־
extern  u16   gIwdgFlag;                    //���Ź���λ��־
extern  u16   gBackupFlag;      //�󴫸����ĵ��ź�ֵ 
extern  u16   gWorkMode;                 //����ģʽ��0�ֶ���1�Զ�,2����
extern  u16   gSensorMode;             //��ƫģʽ��1=��EPC1��2=��EPC2��3=��+��CPC��4=SPC

extern  u16   gAutoPolar;                //�Զ����ԣ�1=������0=����;
extern  u16   gManuPolar;                //�ֶ����ԣ�1=������0=����;
extern  u16   gMotorType;                //�������
extern  u16   gPowerOnMode;              ///��������ģʽ��,0=�ֶ���1=�Զ���2=���ģ�3=�ϴ� 
extern  u16   gCurMatNum;            //Ĭ�ϲ���  

extern  u16   gGainData;                 //ϵͳ����������,����;
extern  u16   gDeadZone;                 //ä�����ã�������ʶ��ä���������ϱ�����ȱ��ʱ����ֹ����
extern  u16   gFineTune;               //΢��=����;
extern  u16   gAutoSpeed;                //�Զ��ٶ�
extern  u16   gManuSpeed;                //�ֶ��ٶ�
extern  u16   gCentSpeed;                //�����ٶ�

extern  u16   gCaliTorque;            //У׼ת��
extern  u16   gFuncTorque;              //����֧��ʱת�����ƣ����Ϊ10A

extern  s16   gCurrentPulseNum;         //��ǰ����λ��������	
extern  s16   gMAXPulseNum;             //������0�����ֵMAX_pulse_num

extern  u16   gLimitMode;                //��λ��ʽ
extern  u16   gBehindLimit;              //ǰ��λ������
extern  u16   gCenterLimit;		    	 //������λ������
extern  u16   gFrontLimit;				 //����λ������


extern  u16   gSPCMode;                  //���ξ�ƫģʽ  0���ڲ�������  1���ⲿ������
extern  u16   gSPCStopTime;              //spsֹͣʱ��=0.2��
extern  u16   gSPCBehindLimit;
extern  u16   gSPCFrontLimit;

extern  u16   gNoWaitEN;               //���ϵȴ�����0=��ֹ��1=ʹ��
extern  u16   gNoDetectValve;          //���ϼ�ֵⷧ  
extern  u16   gNoDetectTime;           //���ϼ��ʱ��=0.1��
extern  u16   gNoWaitTime;             //���ϵȴ�ʱ��=1.5��  


extern  u16   gLongIo0Mode;            //Զ�������1״̬ 
extern  u16   gLongIo1Mode;            //Զ�������2״̬
extern  u16   gLongIo2Mode;            //Զ�������3״̬
extern  u16   gLongIo3Mode;            //Զ�������4״̬ 

extern  u16   gAFlexAcc;                  //�Զ����Լ���
extern  u16   gAFlexDec;                  //�Զ����Լ���
extern  u16   gMFlexAcc;                  //�ֶ����Լ���
extern  u16   gMFlexDec;                  //�ֶ����Լ���

extern  u16   gAFlexSacc; 
extern  u16   gAFlexSdec; 
extern  u16   gMFlexSacc;
extern  u16   gMFlexSdec;

extern  u16   gLanguageChoice;           //����
extern  u16   gLockTime;
extern  u16   gBrightness;
extern  u16   gBuzzer;
extern  u8    Password[6];

extern  char   *SysPassword;      //ϵͳ����

extern  u16   g_Mat0_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
extern  u16   g_Mat0_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
extern  u16   g_Mat0_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
extern  u16   g_Mat0_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
extern  u16    Mat0EPC12;

extern  u16   g_Mat1_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
extern  u16   g_Mat1_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
extern  u16   g_Mat1_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
extern  u16   g_Mat1_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
extern  u16    Mat1EPC12;

extern  u16   g_Mat2_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
extern  u16   g_Mat2_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
extern  u16   g_Mat2_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
extern  u16   g_Mat2_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
extern  u16    Mat2EPC12;

extern  u16   g_Mat3_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
extern  u16   g_Mat3_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
extern  u16   g_Mat3_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
extern  u16   g_Mat3_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
extern  u16    Mat3EPC12;

extern  u16   g_Mat4_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
extern  u16   g_Mat4_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
extern  u16   g_Mat4_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
extern  u16   g_Mat4_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
extern  u16    Mat4EPC12;

extern  u16   g_Mat5_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
extern  u16   g_Mat5_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
extern  u16   g_Mat5_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
extern  u16   g_Mat5_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
extern  u16    Mat5EPC12;

extern  u16   g_Mat6_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
extern  u16   g_Mat6_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
extern  u16   g_Mat6_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
extern  u16   g_Mat6_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
extern  u16    Mat6EPC12;

extern  u16   g_Mat7_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
extern  u16   g_Mat7_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
extern  u16   g_Mat7_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
extern  u16   g_Mat7_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
extern  u16    Mat7EPC12;







/*******������־******/
enum   WarmFlagType
{
	NoErrFlag,
	LockFlag,        //������־
	RunReadyFlag,    //����׼��
	ProhibitFlag,    //��ֹ�л�������״̬
	LimitFlag,       //��λ����
	OverrunFlag,     //��������
	StallFlag,       //�����ת
	NotConFlag,      //��������
	ComErrFlag,      //ͨ�Ŵ���
	VolHighFlag,     //��ѹ����
	VolLowFlag,      //��ѹ����
	WarmNum,
};

extern	u8 	 Warm[WarmNum];
extern  u16  LongPortFun[FunNum];
extern  u16  ClickButton;         //�㶯��������


extern  volatile u16  g_ADC_Buf[4];
extern  volatile bool g_ADC_OK;


extern  u32  time0,time1,time2;
extern  u16  SensorL_value;                  //������1��ֵ
extern  u16  SensorR_value;                  //������2��ֵ

extern  u32  NUMBERTURN;
extern  u8   SERIES;

extern  u8   LastHallData;

/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef enum
{
  CW=-1,       // ��ʱ�뷽�� ��ת
  CCW=1        // ˳ʱ�ӷ��� ��ת
}MOTOR_DIR;

typedef enum 
{
  STOP=0,    // ֹͣ
  RUN=1      // ��ת
}MOTOR_STATE;

typedef struct MyMotorType
{
	__IO int          motor_speed;        // ���ת��(RPM):0..2500
	__IO MOTOR_STATE  motor_state;        // �����ת״̬��ָֹͣ���֮��ʼɲ������һ��ʱ�����Ż�ֹͣ
	__IO MOTOR_STATE  order_state;        // ָ����ת״̬
	__IO MOTOR_DIR    motor_direction;    // ���ת�����򣬷���Ҳ���ӳ���ָ���
	__IO MOTOR_DIR    order_direction;    // ָ��ת������
	__IO uint32_t     step_counter;       // �������������������ڲ������ת��
	__IO uint16_t     stalling_count;     // ͣ����־�������ֵ��2000����Ϊ���ֹͣ��ת
}MotorDev;

extern MotorDev bldc_dev;



void ParameterInit(void);   

#endif



