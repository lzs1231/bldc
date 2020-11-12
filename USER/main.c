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
 
//������Ҫ�洢�Ĳ���

__noinit__ u16   gCheckFlag;		           //У���־
__noinit__ u16   gIwdgFlag;                 //���Ź���λ��־
__noinit__ u16   gBackupFlag;               //���ݱ�־     0��ʾ�ޱ��ݣ�1��ʾ�б���

__noinit__ u16   gWorkMode;                 //0=�ֶ�   1=�Զ�  2=����
__noinit__ u16   gSensorMode;               //��ƫģʽ��0=��1=�ң�2=��+�ң�3=SPC  ��EPC1������������1������EPC2������������2������CPC����������һ������SPC

__noinit__ u16   gAutoPolar;                //�Զ����ԣ�1=������0=����;
__noinit__ u16   gManuPolar;                //�ֶ����ԣ�1=������0=����;
__noinit__ u16   gMotorType;               //�������   0��ˢ���4    1��ˢ���5  2��ˢ���
__noinit__ u16   gPowerOnMode;             //��������ģʽ��,0=�ֶ���1=�Զ���2=���ģ�3=�ϴ�
__noinit__ u16   gCurMatNum;              //Ĭ�ϲ���

__noinit__ u16   gGainData;                //ϵͳ����������;
__noinit__ u16   gDeadZone;                //ä�����ã�������ʶ��ä���������ϱ�����ȱ��ʱ����ֹ����
__noinit__ u16   gFineTune;                //΢��=����������Ҫ΢������λ��ʱ����Ҫ�ı䴫����λ�ã�ͨ������΢���������������������λ��

__noinit__ u16   gAutoSpeed;               //�Զ��ٶ�
__noinit__ u16   gManuSpeed;               //�ֶ��ٶ�
__noinit__ u16   gCentSpeed;               //�����ٶ�

__noinit__ u16   gCaliTorque;              //У׼ת��
__noinit__ u16   gFuncTorque;              //����֧��ʱת�����ƣ����Ϊ10A
__noinit__ s16   gCurrentPulseNum;         //��ǰ����λ��������
__noinit__ s16   gMAXPulseNum;             //�г����������

__noinit__ u16   gLimitMode;               //��λ��ʽ   0Ϊ�ڲ���λ  1Ϊ�ⲿ��λ������λ
__noinit__ u16   gBehindLimit;             //ǰ��λ������
__noinit__ u16   gCenterLimit;		       //������λ������
__noinit__ u16   gFrontLimit;			   //����λ������

__noinit__ u16   gSPCMode;                 //���ξ�ƫģʽ  0���ڲ�������  1���ⲿ������
__noinit__ u16   gSPCStopTime;             //spcֹͣʱ��=0.2��
__noinit__ u16   gSPCBehindLimit;          //spc���ֹͣ��
__noinit__ u16   gSPCFrontLimit;           //spc����ֹͣ��

__noinit__ u16   gNoWaitEN;                //���ϵȴ�����  0=ʹ��,1=��ֹ
__noinit__ u16   gNoDetectValve;           //���ϼ�ֵⷧ  
__noinit__ u16   gNoDetectTime;            //���ϼ��ʱ��=0.1��
__noinit__ u16   gNoWaitTime;              //���ϵȴ�ʱ��=1.5��  

__noinit__ u16	 gLongIo0Mode;             //Զ�������1״̬ 
__noinit__ u16   gLongIo1Mode;             //Զ�������2״̬
__noinit__ u16   gLongIo2Mode;             //Զ�������3״̬
__noinit__ u16   gLongIo3Mode;             //Զ�������4״̬ 

__noinit__ u16   gAFlexAcc;                //�Զ����Լ���
__noinit__ u16   gAFlexDec;                //�Զ����Լ���
__noinit__ u16   gMFlexAcc;                //�ֶ����Լ���
__noinit__ u16   gMFlexDec;                //�ֶ����Լ���

__noinit__ u16   gAFlexSacc; 
__noinit__ u16   gAFlexSdec; 
__noinit__ u16   gMFlexSacc;
__noinit__ u16   gMFlexSdec;

__noinit__ u16   gLanguageChoice;             //����
__noinit__ u16   gLockTime;
__noinit__ u16   gBrightness;
__noinit__ u16   gBuzzer;
__noinit__ u8    Password[6];                   //�û�����
char  *SysPassword = "888888";      //ϵͳ����

__noinit__ u16   g_Mat0_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat0_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat0_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat0_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat0EPC12;

__noinit__ u16   g_Mat1_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat1_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat1_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat1_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat1EPC12;

__noinit__ u16   g_Mat2_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat2_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat2_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat2_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat2EPC12;

__noinit__ u16   g_Mat3_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat3_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat3_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat3_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat3EPC12;

__noinit__ u16   g_Mat4_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat4_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat4_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat4_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat4EPC12;

__noinit__ u16   g_Mat5_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat5_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat5_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat5_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat5EPC12;

__noinit__ u16   g_Mat6_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat6_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat6_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat6_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat6EPC12;

__noinit__ u16   g_Mat7_Sensor1_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat7_Sensor1_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat7_Sensor2_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat7_Sensor2_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat7EPC12;


u8 	 Warm[WarmNum];               	 /*******������־******/
u16  ClickButton;                    //�㶯��������
u16  LongPortFun[FunNum];      	  	 /*******Զ��IO����******/

u8   cmd_buffer[CMD_MAX_SIZE];       //ָ���
volatile u16    g_ADC_Buf[4];        //DMAĿ���ַ
volatile bool   g_ADC_OK = FALSE;    //ADC�ɼ��ɹ���־

u16  SensorL_value;                  //������1��ֵ
u16  SensorR_value;                  //������2��ֵ                      

u32  NUMBERTURN;                     //3�λ���ʱ��
u8   SERIES;                         //���������

u8   LastHallData;


OS_EVENT *iMbox;                    //�������� 

//start����
#define	START_TASK_PRIO				22      //���񴴽����ȼ�
#define	START_STK_SIZE				32
OS_STK  START_TASK_STK[START_STK_SIZE];
void    start_task(void *pdata);

//READ_IO����
#define	READ_IO_TASK_PRIO				5     //���񴴽����ȼ�
#define	READ_IO_STK_SIZE				64
OS_STK  READ_IO_TASK_STK[READ_IO_STK_SIZE];
void    readio_task(void *pdata);

//LCD����
#define	LCD_TASK_PRIO				13    		//���񴴽����ȼ�
#define	LCD_STK_SIZE				128
OS_STK  LCD_TASK_STK[LCD_STK_SIZE];
void    lcd_task(void *pdata);

//LED����
#define	LED_TASK_PRIO				20      //���񴴽����ȼ�
#define	LED_STK_SIZE				32
OS_STK  LED_TASK_STK[LED_STK_SIZE];
void    led_task(void *pdata);

void SensorInit(void)
{
	g_Mat0_Sensor1_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat0_Sensor1_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat0_Sensor2_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat0_Sensor2_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat0EPC12=3;

	g_Mat1_Sensor1_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat1_Sensor1_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat1_Sensor2_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat1_Sensor2_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat1EPC12=0;

	g_Mat2_Sensor1_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat2_Sensor1_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat2_Sensor2_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat2_Sensor2_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat2EPC12=0;

	g_Mat3_Sensor1_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat3_Sensor1_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat3_Sensor2_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat3_Sensor2_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat3EPC12=0;

	g_Mat4_Sensor1_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat4_Sensor1_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat4_Sensor2_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat4_Sensor2_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat4EPC12=0;
	
	g_Mat5_Sensor1_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat5_Sensor1_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat5_Sensor2_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat5_Sensor2_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat5EPC12=0;

	g_Mat6_Sensor1_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat6_Sensor1_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat6_Sensor2_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat6_Sensor2_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat6EPC12=0;

	g_Mat7_Sensor1_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat7_Sensor1_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat7_Sensor2_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat7_Sensor2_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat7EPC12=0;
}

void SetDataInit()                 //���ò�����ʼ��
{
	u8 i;
	gCheckFlag = 0xee55;		 //У���־
	gIwdgFlag = 0;               //�����������Ź�����
	gBackupFlag = 0;             //�Ƿ��б��ݣ�1��ʾ�б��ݣ�0��ʾû�б���
	
	gWorkMode=0;                 //0=�ֶ�   1=�Զ�  2=����
	gSensorMode=0;               //��ƫģʽ��0=��1=�ң�2=��+�ң�3=SPC  ��EPC1������������1������EPC2������������2������CPC����������һ������SPC

	gAutoPolar=0;                //�Զ����ԣ�0=������1=����;
	gManuPolar=0;                //�ֶ����ԣ�0=������1=����;
	gMotorType = 1;              //�������   0��ˢ���4   1��ˢ���5   2��ˢ���
	gPowerOnMode  =0;            //��������ģʽ��,0=�ֶ���1=�Զ���2=���ģ�3=�ϴ� 
	gCurMatNum=0;                //Ĭ�ϲ���

	gGainData = 40;              //ϵͳ����������;
	gDeadZone = 1;               //ä�����ã�������ʶ��ä���������ϱ�����ȱ��ʱ����ֹ����
	gFineTune = 80;              //΢��=����������Ҫ΢������λ��ʱ����Ҫ�ı䴫����λ�ã�ͨ������΢���������������������λ��

	gAutoSpeed = 1800;            //�Զ��ٶ�
	gManuSpeed = 1800;            //�ֶ��ٶ�
	gCentSpeed = 1800;            //�����ٶ�
	
	gCaliTorque=10;
	gFuncTorque = 5;             //����֧��ʱת�����ƣ����Ϊ10A
	gCurrentPulseNum=200;        //��ǰ����λ��������
	gMAXPulseNum=500;            //�г����������

	gLimitMode = 0;              //��λ��ʽ   0Ϊ�ڲ���λ  1Ϊ�ⲿ��λ������λ   2�ڲ����ⲿ
	gBehindLimit=95;             //ǰ��λ������
	gCenterLimit=50;		     //������λ������
	gFrontLimit=5;			     //����λ������

	gNoWaitEN=1;                 //���ϵȴ����ܣ�0=ʹ��,1=��ֹ
	gNoDetectValve=95;           //���ϼ�ֵⷧ  
	gNoDetectTime=1;             //���ϼ��ʱ��=0.1��
	gNoWaitTime=15;              //���ϵȴ�ʱ��=1.5��  

	gSPCMode = 0;                //���ξ�ƫģʽ  0���ڲ�������  1���ⲿ������
	gSPCStopTime=2;              //spcֹͣʱ��=0.2��
	gSPCBehindLimit=90;          //spc���ֹͣ��
	gSPCFrontLimit=10;           //spc����ֹͣ��

	gLongIo0Mode=0;              //Զ�������1״̬ 
	gLongIo1Mode=0;              //Զ�������2״̬
	gLongIo2Mode=0;              //Զ�������3״̬
	gLongIo3Mode=0;              //Զ�������4״̬

	gAFlexAcc=AFlexAcc;          //�Զ����Լ���
	gAFlexDec=AFlexDec;          //�Զ����Լ���
	gMFlexAcc=MFlexAcc;          //�ֶ����Լ���
	gMFlexDec=MFlexDec;          //�ֶ����Լ���
	
	gAFlexSacc=0; 
	gAFlexSdec=0; 
	gMFlexSacc=0;
	gMFlexSdec=0;

	gLanguageChoice=0;           //����
	gLockTime=3;
	gBrightness = 4;
	gBuzzer = 1;
	for(i=0;i<6;i++)
	{
		Password[i]=i+0x31;
	}
}

void ParameterInit()             //������������
{
	SetDataInit();               //���ò�����ʼ��
	SensorInit();                //������������ʼ��
//	Clearing();                  //���㱸��
}

int main(void)
{
	OSInit();    //�������������ͳ������
	OSTaskCreate(start_task,(void*)0,&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart();   //������ OSStart()ʱ��OSStart()��������������ҳ��Ǹ��û����������ȼ���������������ƿ飬����������ȼ�����
}

/**-----------------��ʼ����------------------**/
void start_task(void *pdata) 
{	
	OS_CPU_SR cpu_sr=0;
	u8 icnt;
	pdata=pdata;
	
    BSP_Init();   
    queue_reset();       		//��մ��ڽ��ջ�����    
    for(icnt=0;icnt<FunNum;icnt++)
	{
		LongPortFun[icnt] = 0;
	}
	for(icnt=0;icnt<WarmNum;icnt++)
	{
		Warm[icnt] = 0;
	}
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)!= RESET)         //�ж��Ƿ����˶������Ź���λ
	{
		RCC->CSR = 1<<24;    	/*Clear reset flag*/
		gIwdgFlag++;          	//������λ��__noinit__���ε�ȫ�ֱ����ᱣ�ָ�λ֮ǰ�����ݣ���������
		PBout(12)=1;	
	}else{                      //�ϵ縴λ
		ReadPara();           	//��ȡflash�б���Ĳ���
		ReadSensor();
		PBout(12)=0;
	}
	
	if(gCheckFlag!=0xee55)  			SetDataInit();  	 //���������ȡ���󣬾����³�ʼ������
	if(g_Mat0_Sensor1_H == 0xffff)  	SensorInit();		 //���������ȡ���󣬾����³�ʼ������������
	
	switch(gPowerOnMode)             //��ѯ����ʱ�Ĺ���״̬
	{
	   case 0: gWorkMode=0;    SetScreen(Manu_ID);        break;      //�����ֶ�
	   case 1: gWorkMode=1;    SetScreen(Auto_ID);        break;      //����ʱΪ�Զ�״̬
	   case 2: gWorkMode=2;    SetScreen(Center_ID);      break;      //����ʱΪ����״̬
	   case 3:                           
		   if(gWorkMode==0)        SetScreen(Manu_ID);
		   else if(gWorkMode==1)   SetScreen(Auto_ID);
	       else if(gWorkMode==2)   SetScreen(Center_ID);   break;     //�ָ��ػ���״̬
	   default: SetDataInit();    break;    //ϵͳ��ʼ��          
	}
	
	switch(gMotorType)                  //��ѯ�������
    {
        case BrushlessMotor4:           //���ѡ�����ˢ����Ϳ�CAP,���ⲿ�ж���Ϊ�������    4�Լ�
			NUMBERTURN = 7500000;       //3�λ���ת���Ƕ�45��   ��45/(t1+t2+t3)��*60/360  ����60��ʾ1����ת���Ƕ�
		    SERIES     =   24;
		break;                   
        case BrushlessMotor5:                         //5�Լ�
			NUMBERTURN = 6000000;
		    SERIES     =   30;
		break; 
        default:SetDataInit();          break;   //ϵͳ��ʼ��  
    }
	
	/***********����λ�ö�ȡ***********/
	LastHallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);   //��ת��λ��    �ϳ�GPIOC8|GPIOC7|GPIOC6 	                                                            
	
	InitLCD();              //�����ʼ��
#if (OS_TASK_STAT_EN > 0)   //����ͳ������
	OSStatInit();     
#endif	
	OS_ENTER_CRITICAL();             //�����ٽ���(���ж�)
	//OSTaskCreate();                ��������
	OSTaskCreate(readio_task, (void*)0, &READ_IO_TASK_STK[READ_IO_STK_SIZE-1],READ_IO_TASK_PRIO);
	OSTaskCreate(lcd_task, (void*)0, &LCD_TASK_STK[LCD_STK_SIZE-1],LCD_TASK_PRIO);
	OSTaskCreate(led_task, (void*)0, &LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);  //����ʼ����
	OS_EXIT_CRITICAL();              //�˳��ٽ���(���ж�)
}

/**-----------------readio_task����---------------------**/
void readio_task(void *pdata)
{
//  �������Ϊȫ�ֱ�����psi_pid_task()���ȼ�����si_pid_task()���ȼ�,SetPwmValue���ܱ������ȼ���������ģ��Ӷ�����            
//	Wwdg_Init(0x7f,0x7d,3);
	IWDG_Init(7,312);   		  //7��ʾ256��Ƶ��256/40K=6.4ms,6.4*312=2s��ʾι�����2s
	for(;;)
	{
		ReadLong(LongPortFun);
		delay_ms(10);
		IWDG_Feed();   		 	//2s֮��û��ι����λ
	}
}

/**-----------------LED3����---------------------**/
void lcd_task(void *pdata)
{
    qsize  size = 0;  

	for(;;)
	{
		size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                              //�ӻ������л�ȡһ��ָ�� 

        if(size>0&&cmd_buffer[1]!=0x07)                              //���յ�ָ�� �����ж��Ƿ�Ϊ������ʾ
        {                    
            ProcessMessage((PCTRL_MSG) cmd_buffer, size);                            //ָ���  
        }                                                                           
        else if(size>0&&cmd_buffer[1]==0x07)                         //���Ϊָ��0x07��������STM32  
        {                                                                           
            __disable_fault_irq();                                                   
            NVIC_SystemReset();                                                                                                                                          
        }                                                                            

        //    �ر�ע��
        //    MCU��ҪƵ���򴮿����������ݣ����򴮿������ڲ��������������Ӷ��������ݶ�ʧ(��������С����׼��8K��������4.7K)
        //    1) һ������£�����MCU�򴮿����������ݵ����ڴ���100ms���Ϳ��Ա������ݶ�ʧ�����⣻
        //    2) �����Ȼ�����ݶ�ʧ�����⣬���жϴ�������BUSY���ţ�Ϊ��ʱ���ܷ������ݸ���������

        //    TODO: ����û�����
        //    �����и���ʱ��־λ�����ı䣬��ʱ100����ˢ����Ļ
		
		//�������
		UpdateUI();
		
		delay_ms(20);
	}
}



/**-----------------LED����---------------------**/
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



