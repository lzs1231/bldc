#ifndef __MAIN_H__
#define __MAIN_H__

#include "longkey.h"
#include "sys.h"



extern u16 *const pGainDead 	 ;
extern u16 *const pFineTune	     ;
extern u16 *const pDisPulseNum   ;
extern u16 *const pSensorRate    ;
extern u16 *const pSensorValue   ;
extern u16 *const pMatDis        ;
extern u16 *const pLimitFun1     ;
extern u16 *const pLimitFun2     ;
extern u16 *const pSPCFun1		 ;
extern u16 *const pSPCFun2		 ;
extern u16 *const pNoMatFun1 	 ;
extern u16 *const pNoMatFun2 	 ;
extern u16 *const pAFlex 		 ;
extern u16 *const pMFlex 		 ;
extern u16 *const pLongIoMode    ;
extern u16 *const pAutoSpeed 	 ;
extern u16 *const pManuSpeed	 ;
extern u16 *const pCentSpeed 	 ;
extern u16 *const pCurrentPara   ;
extern u16 *const pAdjustData	 ;
extern u16 *const pOtherPara	 ;



extern u16 *const pHAdjustFlag  ;
extern u16 *const pHClickBut    ;
extern u16 *const pHAddSub      ;

extern u16 *const pHWorkMode    ;
extern u16 *const pHSensorMode  ;
extern u16 *const pHLimitMode   ;
extern u16 *const pHSPCMode     ;
extern u16 *const pHNoWaitEN    ;
extern u16 *const pHAutoPolar   ;
extern u16 *const pHManuPolar   ;
extern u16 *const pHMotorType   ;
extern u16 *const pHPowerOnMode ;


extern u16 *const pHPort0Fun 	;
extern u16 *const pHPort1Fun 	;
extern u16 *const pHPort2Fun 	;
extern u16 *const pHPort3Fun 	;


extern u16 *const pHParaConfirm ;
extern u16 *const pHMatNum 	    ;  // ��ǰ���ϱ��

extern u16 *const pHSCaliStep   ;
extern u16 *const pHSensorNum   ;

extern u16 *const pHTCaliTorque ;
extern u16 *const pHTCaliFlag   ;
extern u16 *const pHTCaliCancel ;

extern u16 *const pHFlexSpeed	;    //���ԼӼ���ֵ


extern u16 *const pHSysYes		;    //ȷ��ִ��
extern u16 *const pHWarmFlag	;   //�����־

//������Ҫ�洢�Ĳ���
extern  u16   gCheckFlag;		         //У���־
extern  u16   gIwdgFlag;                  //���Ź���λ��־
extern  u16   gBackupFlag;    		

extern  u16   gWorkMode;                 //����ģʽ��0�ֶ���1�Զ�,2����
extern  u16   gSensorMode;               //��ƫģʽ��1=��EPC1��2=��EPC2��3=��+��CPC��4=SPC

extern  u16   gAutoPolar;                //�Զ����ԣ�1=������0=����;
extern  u16   gManuPolar;                //�ֶ����ԣ�1=������0=����;
extern  u16   gMotorType;                //�������
extern  u16   gPowerOnMode;              //��������ģʽ��,0=�ֶ���1=�Զ���2=���ģ�3=�ϴ� 
extern  u16   gCurMatNum;               //Ĭ�ϲ���

extern  u16   gGainData;                 //ϵͳ����������,����;
extern  u16   gDeadZone;                 //ä�����ã�������ʶ��ä���������ϱ�����ȱ��ʱ����ֹ����
extern  u16   gFineTune;                 //΢��=����;

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
extern  u16  ClickButton;           //�㶯��������

extern  volatile u16  g_ADC_Buf[4];
extern  volatile bool g_ADC_OK;

extern  u32  time0,time1,time2;
extern  u16  SensorL_value;                  //������1��ֵ
extern  u16  SensorR_value;                  //������2��ֵ

extern  u32  NUMBERTURN;
extern  u8   SERIES;

extern  u8   LastHallData;
extern  u16  HallRate;             //��ǰ��������г��еı���


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



