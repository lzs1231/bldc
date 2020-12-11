#include "bsp.h"
#include "stmflash.h"
#include "lcdtest.h"
#include "main.h"
#include "wdg.h"
#include "modbus.h"


#define get4bit(a,b)  ((a>>(b*4))&0x000f)

//������Ҫ�洢�Ĳ���
__noinit__ u16   gCheckFlag;		       //У���־
__noinit__ u16   gIwdgFlag;                //���Ź���λ��־
__noinit__ u16   gBackupFlag;              //���ݱ�־     0��ʾ�ޱ��ݣ�1��ʾ�б���

__noinit__ u16   gWorkMode;                //0=�ֶ�   1=�Զ�  2=����
__noinit__ u16   gSensorMode;              //��ƫģʽ��0=��1=�ң�2=��+�ң�3=SPC  ��EPC1������������1������EPC2������������2������CPC����������һ������SPC
__noinit__ u16   gSensorSignal;

__noinit__ u16   gAutoPolar;               //�Զ����ԣ�1=������0=����;
__noinit__ u16   gManuPolar;               //�ֶ����ԣ�1=������0=����;
__noinit__ u16   gMotorType;               //�������   0��ˢ���4    1��ˢ���5  2��ˢ���
__noinit__ u16   gPowerOnMode;             //��������ģʽ��,0=�ֶ���1=�Զ���2=���ģ�3=�ϴ�
__noinit__ u16   gCurMatNum;               //Ĭ�ϲ���

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
__noinit__ u16   gExtendLimit;             //ǰ��λ������
__noinit__ u16   gCenterLimit;		       //������λ������
__noinit__ u16   gIndentLimit;			   //����λ������

__noinit__ u16   gSPCMode;                 //���ξ�ƫģʽ  0���ڲ�������  1���ⲿ������
__noinit__ u16   gSPCStopTime;             //spcֹͣʱ��=0.2��
__noinit__ u16   gSPCExtendLimit;          //spc���ֹͣ��
__noinit__ u16   gSPCIndentLimit;           //spc����ֹͣ��

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
__noinit__ u16   gAlarmSwitch;
__noinit__ u16   gRelay;
__noinit__ u16   gKeepWait;

__noinit__ u16   g_Mat0_SensorL_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat0_SensorL_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat0_SensorR_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat0_SensorR_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat0EPC12;

__noinit__ u16   g_Mat1_SensorL_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat1_SensorL_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat1_SensorR_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat1_SensorR_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat1EPC12;

__noinit__ u16   g_Mat2_SensorL_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat2_SensorL_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat2_SensorR_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat2_SensorR_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat2EPC12;

__noinit__ u16   g_Mat3_SensorL_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat3_SensorL_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat3_SensorR_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat3_SensorR_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat3EPC12;

__noinit__ u16   g_Mat4_SensorL_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat4_SensorL_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat4_SensorR_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat4_SensorR_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat4EPC12;

__noinit__ u16   g_Mat5_SensorL_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat5_SensorL_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat5_SensorR_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat5_SensorR_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat5EPC12;

__noinit__ u16   g_Mat6_SensorL_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat6_SensorL_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat6_SensorR_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat6_SensorR_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat6EPC12;

__noinit__ u16   g_Mat7_SensorL_H;      //�󴫸����ĸ��ź�ֵ
__noinit__ u16   g_Mat7_SensorL_L;      //�󴫸����ĵ��ź�ֵ 
__noinit__ u16   g_Mat7_SensorR_H;      //�Ҵ������ĸ��ź�ֵ
__noinit__ u16   g_Mat7_SensorR_L;      //�Ҵ������ĵ��ź�ֵ 
__noinit__ u16    Mat7EPC12;


u16  LongPortFun[FunNum];      	  	 /*******Զ��IO����******/
u16  ClickButton;                    //�㶯��������
u8 	 Warm[WarmNum];               	 /*******������־******/

volatile u16    g_ADC_Buf[4];        //DMAĿ���ַ


u16  SensorL_value;                  //������1��ֵ
u16  SensorR_value;                  //������2��ֵ                      

u32  NUMBERTURN;                     //3�λ���ʱ��
u8   SERIES;                         //���������

u8   LastHallData;


OS_EVENT *iMbox;                    //�������� 
OS_EVENT *sem_p;
OS_EVENT *sem_v;
OS_EVENT *mutex;            		//�����ź���

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

//LCD1����
#define	MODBUS_TASK_PRIO			6      //���񴴽����ȼ�
#define	MODBUS_STK_SIZE				64
OS_STK  MODBUS_TASK_STK[MODBUS_STK_SIZE];
void    modbus_task(void *pdata);

//LCD1����
#define	LCD_TASK_PRIO				7      //���񴴽����ȼ�
#define	LCD_STK_SIZE				64
OS_STK  LCD_TASK_STK[LCD_STK_SIZE];
void    lcd_task(void *pdata);

//LED����
#define	LED_TASK_PRIO				20      //���񴴽����ȼ�
#define	LED_STK_SIZE				32
OS_STK  LED_TASK_STK[LED_STK_SIZE];
void    led_task(void *pdata);

void SensorInit(void)
{
	g_Mat0_SensorL_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat0_SensorL_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat0_SensorR_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat0_SensorR_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat0EPC12=3;

	g_Mat1_SensorL_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat1_SensorL_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat1_SensorR_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat1_SensorR_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat1EPC12=0;

	g_Mat2_SensorL_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat2_SensorL_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat2_SensorR_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat2_SensorR_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat2EPC12=0;

	g_Mat3_SensorL_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat3_SensorL_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat3_SensorR_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat3_SensorR_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat3EPC12=0;

	g_Mat4_SensorL_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat4_SensorL_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat4_SensorR_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat4_SensorR_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat4EPC12=0;
	
	g_Mat5_SensorL_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat5_SensorL_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat5_SensorR_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat5_SensorR_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat5EPC12=0;

	g_Mat6_SensorL_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat6_SensorL_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat6_SensorR_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat6_SensorR_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat6EPC12=0;

	g_Mat7_SensorL_H=3930;      //�󴫸����ĸ��ź�ֵ
	g_Mat7_SensorL_L=165;      //�󴫸����ĵ��ź�ֵ 
	g_Mat7_SensorR_H=3930;      //�Ҵ������ĸ��ź�ֵ
	g_Mat7_SensorR_L=165;      //�Ҵ������ĵ��ź�ֵ 
	Mat7EPC12=0;
}

void SetDataInit()                 //���ò�����ʼ��
{
	gCheckFlag = 0xee55;		 //У���־
	gIwdgFlag = 0;               //�����������Ź�����
	gBackupFlag = 0;             //�Ƿ��б��ݣ�1��ʾ�б��ݣ�0��ʾû�б���
	
	gWorkMode=0;                 //0=�ֶ�   1=�Զ�  2=����
	gSensorMode=0;               //��ƫģʽ��0=��1=�ң�2=��+�ң�3=SPC  ��EPC1������������1������EPC2������������2������CPC����������һ������SPC
	gSensorSignal = 1;
	
	gAutoPolar=0;                //�Զ����ԣ�0=������1=����;
	gManuPolar=0;                //�ֶ����ԣ�0=������1=����;
	gMotorType = 0;              //�������   0��ˢ���4   1��ˢ���5   2��ˢ���
	gPowerOnMode  =0;            //��������ģʽ��,0=�ֶ���1=�Զ���2=���ģ�3=�ϴ� 
	gCurMatNum=0;                //Ĭ�ϲ���

	gGainData = 40;              //ϵͳ����������;
	gDeadZone = 1;               //ä�����ã�������ʶ��ä���������ϱ�����ȱ��ʱ����ֹ����
	gFineTune = 80;              //΢��=����������Ҫ΢������λ��ʱ����Ҫ�ı䴫����λ�ã�ͨ������΢���������������������λ��

	gAutoSpeed = 800;            //�Զ��ٶ�
	gManuSpeed = 600;            //�ֶ��ٶ�
	gCentSpeed = 600;            //�����ٶ�
	
	gCaliTorque=1;
	gFuncTorque = 5;             //����֧��ʱת�����ƣ����Ϊ10A
	gCurrentPulseNum=200;        //��ǰ����λ��������
	gMAXPulseNum=500;            //�г����������

	gLimitMode = 0;              //��λ��ʽ   0Ϊ�ڲ���λ  1Ϊ�ⲿ��λ������λ   2�ڲ����ⲿ
	gExtendLimit=95;             //�����λ������
	gCenterLimit=50;		     //������λ������
	gIndentLimit=5;			     //������λ������

	gNoWaitEN=2;                 //���ϵȴ����ܣ�0�ص�����;1���ֹͣ;2�޲���;
	gNoDetectValve=95;           //���ϼ�ֵⷧ  
	gNoDetectTime=1;             //���ϼ��ʱ��=0.1��
	gNoWaitTime=50;              //���ϵȴ�ʱ��=5��  

	gSPCMode = 0;                //���ξ�ƫģʽ  0���ڲ�������  1���ⲿ������
	gSPCStopTime=2;              //spcֹͣʱ��=0.2��
	gSPCExtendLimit=90;          //spc���ֹͣ��
	gSPCIndentLimit=10;          //spc����ֹͣ��

	gLongIo0Mode=0;              //Զ�������1״̬ 
	gLongIo1Mode=0;              //Զ�������2״̬
	gLongIo2Mode=0;              //Զ�������3״̬
	gLongIo3Mode=0;              //Զ�������4״̬

	gAFlexAcc=AFlexAcc;          //�Զ����Լ���
	gAFlexDec=AFlexDec;          //�Զ����Լ���
	gMFlexAcc=MFlexAcc;          //�ֶ����Լ���
	gMFlexDec=MFlexDec;          //�ֶ����Լ���	
	gAlarmSwitch  = 0;
	gSensorSignal = 1;
	gRelay		  = 0;
	gKeepWait	  = 0;
}

void ParameterInit()             //������������
{
	SetDataInit();               //���ò�����ʼ��
	SensorInit();                //������������ʼ��
	Modbus_Init();
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

	pdata=pdata;
	
    BSP_Init();     
  
	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)!= RESET)         //�ж��Ƿ����˶������Ź���λ
	{
		RCC->CSR = 1<<24;    	/*Clear reset flag*/
		gIwdgFlag++;          	//������λ��__noinit__���ε�ȫ�ֱ����ᱣ�ָ�λ֮ǰ�����ݣ���������
//		PBout(12)=1;	
	}else{                      //�ϵ縴λ
		ReadPara();           	//��ȡflash�б���Ĳ���
		ReadSensor();
//		PBout(12)=0;
	}
	
	if(gCheckFlag!=0xee55)  			SetDataInit();  	 //���������ȡ���󣬾����³�ʼ������
	if(g_Mat0_SensorL_H == 0xffff)  	SensorInit();		 //���������ȡ���󣬾����³�ʼ������������
	
	switch(gPowerOnMode)                    		 //��ѯ����ʱ�Ĺ���״̬
	{
	   case 0: gWorkMode=0;          	break;      //�����ֶ�
	   case 1: gWorkMode=1;          	break;      //����ʱΪ�Զ�״̬
	   case 2: gWorkMode=2;             break;      //����ʱΪ����״̬
	   case 3: 						 	break;      //�ָ��ػ���״̬
	   default: SetDataInit();    		break;      //ϵͳ��ʼ��          
	}
	*pHWorkMode = gWorkMode;			//0=�ֶ�   1=�Զ�  2=����
	
	switch(gMotorType)                  //��ѯ�������
    {
        case BrushlessMotor4:           //���ѡ�����ˢ����Ϳ�CAP,���ⲿ�ж���Ϊ�������    4�Լ�
			NUMBERTURN = 7500000;       //3�λ���ת���Ƕ�45��   ��45/(t1+t2+t3)��*60/360  ����60��ʾ1����ת���Ƕ�
		    SERIES     =   24;
		break;                   
        case BrushlessMotor5:                    //5�Լ�
			NUMBERTURN = 6000000;
		    SERIES     =   30;
		break; 
        default:SetDataInit();          break;   //ϵͳ��ʼ��  
    }
	
	/***********����λ�ö�ȡ***********/
	LastHallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);   //��ת��λ��    �ϳ�GPIOC8|GPIOC7|GPIOC6 	                                                            
	
	Modbus_Init();       			//���ݶ�ȡ��ȫ�ֱ������±��ּĴ���
#if (OS_TASK_STAT_EN > 0)   		//����ͳ������
	OSStatInit();     
#endif	
	OS_ENTER_CRITICAL();             //�����ٽ���(���ж�)
	//OSTaskCreate();                ��������
	OSTaskCreate(readio_task, (void*)0, &READ_IO_TASK_STK[READ_IO_STK_SIZE-1],READ_IO_TASK_PRIO);
	OSTaskCreate(modbus_task, (void*)0, &MODBUS_TASK_STK[MODBUS_STK_SIZE-1],MODBUS_TASK_PRIO);
	OSTaskCreate(lcd_task, (void*)0, &LCD_TASK_STK[LCD_STK_SIZE-1],LCD_TASK_PRIO);
	OSTaskCreate(led_task, (void*)0, &LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);  //����ʼ����
	OS_EXIT_CRITICAL();              //�˳��ٽ���(���ж�)
}

/**-----------------readio_task����---------------------**/
void readio_task(void *pdata)
{
	static u16 LastSwitch1EPC,LastSwitch2EPC;
	
//  �������Ϊȫ�ֱ�����psi_pid_task()���ȼ�����si_pid_task()���ȼ�,SetPwmValue���ܱ������ȼ���������ģ��Ӷ�����            
//	Wwdg_Init(0x7f,0x7d,3);
	IWDG_Init(7,312);   		  //7��ʾ256��Ƶ��256/40K=6.4ms,6.4*312=2s��ʾι�����2s
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
		IWDG_Feed();   		 	//2s֮��û��ι����λ
	}
}

/**-----------------modbus_task����---------------------**/
void modbus_task(void *pdata)
{
	u8 err;
	
	iMbox = OSMboxCreate(NULL);                  //��������ʼ��һ����Ϣ���䣬������ 
	sem_p = OSSemCreate(1);
	sem_v = OSSemCreate(1);
	mutex = OSMutexCreate(1,&err);               //��������ʼ��һ�������ź��������ȼ�Ϊ1    
	for(;;)
	{
		Modbus_Event();
		
		/*********����*pHAdjustFlag���õ��ڲ�������ɺ�*pHAdjustFlag=255**********/
		if(*pHAdjustFlag!=255)  AddSub();
		
		/********��Ҫ���ٴ����ָ��********/
		ClickButton 	= *pHClickBut;
		
		
		if(*pHTCaliFlag == 1)   //��ȡУ׼��־���鿴�Ƿ�����У׼
		{
			*pHTCaliFlag = 0;
			TravelCal.CaliStep = 0;
			TravelCal.StallDir = 0;
			TravelCal.CaliFlag = 1;
		}
			 
		if(*pHTCaliCancel == 1)              //ȡ��У׼����
		{   
			*pHTCaliCancel = 0;
			TravelCal.CaliFlag = 0;
			TravelCal.CaliStep = 0;
			TravelCal.StallDir = 0;
		}
		
		delay_ms(5);
	}
}

/**-----------------LCD����---------------------**/
void lcd_task(void *pdata)
{
	u16 *pd;
	u8 SensorTips,WarmFlag;
	static u16 TimeCnt;

	
	for(;;)
	{
//		OSSemPend(sem_v, 0, &err);
		/*****************************�������·�ָ��޸ı��ּĴ�����Ȼ�����ȫ�ֱ���***********************************/

		/*********������Ļָ���޸ı���ֵ**********/
		gWorkMode		 =*pHWorkMode;
		gSensorMode		 =*pHSensorMode;
		gSensorSignal    =*pHSignal;
		gAutoPolar		 =*pHAutoPolar;
		gManuPolar		 =*pHManuPolar;	
		gMotorType		 =*pHMotorType;
		switch(gMotorType)                  //��ѯ�������
		{
			case 0:                         //���ѡ�����ˢ����Ϳ�CAP,���ⲿ�ж���Ϊ�������    4�Լ�
				NUMBERTURN = 7500000;       //3�λ���ת���Ƕ�45��   ��45/(t1+t2+t3)��*60/360  ����60��ʾ1����ת���Ƕ�
				SERIES     =   24;
			break;                   
			case 1:                         //5�Լ�
				NUMBERTURN = 6000000;
				SERIES     =   30;
			break; 
			default:SetDataInit();          break;            //ϵͳ��ʼ��  
		}
		gPowerOnMode = *pHPowerOnMode;    //�����ϵ�״̬
		gCurMatNum	 = *pHMatNum;         //���浱ǰ���ϱ��
		
		if(*pHTCaliTorque == 0)			gCaliTorque	 =1;
		else if(*pHTCaliTorque == 1)	gCaliTorque	 =2;
		else if(*pHTCaliTorque == 2)	gCaliTorque	 =3;
		else if(*pHTCaliTorque == 3)	gCaliTorque	 =4;
		
		gLimitMode	 = *pHLimitMode;
		gSPCMode	 = *pHSPCMode;        //����SPCģʽ
		gNoWaitEN	 = *pHNoWaitEN;       //��������ʹ��

		gLongIo0Mode = *pHPortFun0;
		gLongIo1Mode = *pHPortFun1;
		gLongIo2Mode = *pHPortFun2;
		gLongIo3Mode = *pHPortFun3;
		
		gAlarmSwitch = *pHAlarmSwitch;
		gRelay		 = *pHRelay;
		gKeepWait    = *pHKeepWait;
		
//		OSMutexPend(mutex,0,&err);             //����������ȼ�
//		OSMutexPost(mutex);                    //�ָ��������ȼ�
		
		switch(*pHFlexSpeed)
		{
			case 1:		gAFlexAcc = AFlexAcc; *pHFlexSpeed = 0;	 break;
			case 2:		gAFlexDec = AFlexDec; *pHFlexSpeed = 0;  break;
			case 4:		gMFlexAcc = MFlexAcc; *pHFlexSpeed = 0;  break;
			case 8:		gMFlexDec = MFlexDec; *pHFlexSpeed = 0;  break;
			default:	break;
		}
		/*****************���ݻ�ԭ********************/
		BackupRestore();
		
		/******************��������Ĵ���,��ȡ����Ĵ�����������ʾ***********************/
		
		/*****************�����־����***************/
		WarmFlag = WarmOut();

		/**************��������Ĵ����������Ա�������ȡ**********/
//		if(TravelCal.CaliStep==3)  
//		{
//			if(TimeCnt==30)	{TravelCal.CaliStep = 0; TimeCnt=0;}   
//			TimeCnt++;
//		}
		/*********���´�������ʾ******pSensorRate****/
		SensorDis();          //�����������µĴ�����ģʽ���ò�ͬ������ʾ����������
		
		/*********����У׼��������ʾ*****pSensorValue��pMatDis*****/
		SensorTips = MatCal.fun[*pHSCaliStep]();    //�����������µ�У׼������ò�ͬ������ʾ���������� ������У׼�ɹ�����ʧ��
		
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

/**-----------------LED����---------------------**/
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



