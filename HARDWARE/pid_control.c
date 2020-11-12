#include "pid_control.h"
#include "sys.h"
#include "string.h"
#include "main.h"

#include "lcdtest.h"
#include "usart.h"	 
#include "doublebuf.h"    //˫������


//����ʽPID
typedef struct IncrementPID
{
	float Target;     //�趨Ŀ��
	float Uk;         //PID���
	float Udk;        //PID����
	float Uk_1;       //������һ��PID���
	float P;
	float I;
	u8  Flg;        //Ϊ1��ʾ���л������㣬Ϊ0��ʾ�����л�������
	float D;
	float ek_0;       //��ǰ���
	float ek_1;       //��һ�����
	float ek_2;       //���ϴ����
}IncPID;

//λ��ʽPID
typedef struct PositionPID
{
	float Target;     //�趨Ŀ��
	float Uk;         //PID���
	float Uk_1;       //������һ��PID���
	float P;
	float I;
	u8  Flg;         //Ϊ1��ʾ���л������㣬Ϊ0��ʾ�����л�������
	float D;
	float ek_Sum;      //�����ֺ�
	float ek_0;       //��ǰ���
	float ek_1;       //��һ�����
}PosPID;

/***********����PID*************/
IncPID Center_PID;     //����һ���������ֲ��˾�����ڴ�ռ�
IncPID *Center_Point = &Center_PID;   //����һ��ָ��ָ��ÿռ�

/***********�Զ�ģʽPID************/
PosPID Place_PID;     //����һ���������ֲ��˾�����ڴ�ռ�
PosPID *Place_Point = &Place_PID;

/***********�г�У׼PID************/
IncPID Calib_PID;     //����һ���������ֲ��˾�����ڴ�ռ�
IncPID *Calib_Point = &Calib_PID;

/***********�ٶ�PID************/
IncPID Speed_PID;     //����һ���������ֲ��˾�����ڴ�ռ�
IncPID *Speed_Point = &Speed_PID;

/***********SPCPID************/
PosPID SPC_PID;     //����һ���������ֲ��˾�����ڴ�ռ�
PosPID *SPC_Point = &SPC_PID;


/*********��ʼ��PID**********/
void PIDInit()
{
	Place_Point->Target = 0;
	Place_Point->Uk = 0;
	Place_Point->Uk_1 = 0;
	Place_Point->P = 0.5;
	Place_Point->I = Place_Point->P/70.0;
	Place_Point->Flg = 1;
	Place_Point->D = Place_Point->P*5.0;
	Place_Point->ek_Sum = 0;
	Place_Point->ek_0         = 0;         //ek=0
    Place_Point->ek_1         = 0;         //ek-1=0
	
	Center_Point->Target = 0;
	Center_Point->Uk = 0;
	Center_Point->Udk = 0;
	Center_Point->Uk_1 = 0;
	Center_Point->P = 40;
	Center_Point->I = 0.6;
	Center_Point->Flg = 1;
	Center_Point->D = 10;
	Center_Point->ek_0         = 0;         //ek=0
    Center_Point->ek_1         = 0;         //ek-1=0
    Center_Point->ek_2         = 0;          //ek-2=0
	
	Calib_Point->Target = 0;
	Calib_Point->Uk = 0;
	Calib_Point->Udk = 0;
	Calib_Point->Uk_1 = 0;
	Calib_Point->P = 40;
	Calib_Point->I = 0.6;
	Calib_Point->Flg = 1;
	Calib_Point->D = 10;
	Calib_Point->ek_0         = 0;         //ek=0
    Calib_Point->ek_1         = 0;         //ek-1=0
    Calib_Point->ek_2         = 0;          //ek-2=0
	
	Speed_Point->Target = 0;
	Speed_Point->Uk = 0;
	Speed_Point->Udk = 0;
	Speed_Point->Uk_1 = 0;
	Speed_Point->P = 0.3;
	Speed_Point->I = 0.1;
	Speed_Point->Flg = 1;
	Speed_Point->D = 0.5;
	Speed_Point->ek_0         = 0;         //ek=0
    Speed_Point->ek_1         = 0;         //ek-1=0
    Speed_Point->ek_2         = 0;          //ek-2=0
	
	SPC_Point->Target = 0;
	SPC_Point->Uk = 0;
	SPC_Point->Uk_1 = 0;
	SPC_Point->ek_Sum = 0;
	SPC_Point->P = 4;
	SPC_Point->I = 0.08;
	SPC_Point->Flg = 1;
	SPC_Point->D = 3;
	SPC_Point->ek_0         = 0;         //ek=0
    SPC_Point->ek_1         = 0;         //ek-1=0
}

int CalibPID(int PlaceEk,int LimitSpeed)
{
	Calib_Point->ek_0 = (float)PlaceEk;
	
	if(Calib_Point->ek_0<=50 && Calib_Point->ek_0>=-50)       //-50	~50	�ķ�Χ�ڲŽ���PID����
	{
		if(Calib_Point->ek_0<=2 && Calib_Point->ek_0>=-2)     //��-2~2����������ֹ����
		{
			Calib_Point->Uk = 0; 
			TravelCal.CaliStep=3;
			TravelCal.CaliFlag=0;          //�г�У׼��ϣ�����У׼��־
		}else{
			Calib_Point->Udk = Calib_Point->P*(Calib_Point->ek_0-Calib_Point->ek_1)+Calib_Point->I*Calib_Point->ek_0+Calib_Point->D*(Calib_Point->ek_0+Calib_Point->ek_2-2*Calib_Point->ek_1);
			Calib_Point->Uk  = Calib_Point->Uk_1+Calib_Point->Udk;
			if(Calib_Point->Uk>LimitSpeed)        Calib_Point->Uk = LimitSpeed;
			else if(Calib_Point->Uk<-LimitSpeed)  Calib_Point->Uk = -LimitSpeed;       //�����ٶ�����
		}
	}else{
		if(Calib_Point->ek_0>50)        Calib_Point->Uk = LimitSpeed;
		else if(Calib_Point->ek_0<-50)  Calib_Point->Uk = -LimitSpeed;
	}
	
	//������������һ�μ���
	Calib_Point->ek_2 = Calib_Point->ek_1;
	Calib_Point->ek_1 = Calib_Point->ek_0;
	Calib_Point->Uk_1 = Calib_Point->Uk;				  

	return (int)Calib_Point->Uk;
}

int CenterPID(int PlaceEk,int LimitSpeed)
{
	Center_Point->ek_0 = (float)PlaceEk;

	if((Center_Point->ek_0<=50) && (Center_Point->ek_0>=-50))
	{
		if((Center_Point->ek_0<=2) && (Center_Point->ek_0>=-2))          //ƫ��-1~1��������pi����,����1��ƫ����������ֹ����
		{
			Center_Point->Uk = 0;
		}else{                          // ƫ����-50~50֮�����λ�û�PID����
			Center_Point->Udk = (Center_Point->P*(Center_Point->ek_0-Center_Point->ek_1)
								 +Center_Point->I*Center_Point->ek_0)
									+Center_Point->D*(Center_Point->ek_0+Center_Point->ek_2-2*Center_Point->ek_1);
			
			Center_Point->Uk = Center_Point->Uk_1+Center_Point->Udk;
			
			if(Center_Point->Uk > LimitSpeed)        Center_Point->Uk = LimitSpeed; 
			else if(Center_Point->Uk < -LimitSpeed)  Center_Point->Uk = -LimitSpeed; 
		}
	}else{             //ƫ����󣬲�����pi����
		if(Center_Point->ek_0>50)       Center_Point->Uk = LimitSpeed;  
		else if(Center_Point->ek_0<-50) Center_Point->Uk = -LimitSpeed;  
	}
	
	//������������һ�μ���
	Center_Point->ek_2 = Center_Point->ek_1;
	Center_Point->ek_1 = Center_Point->ek_0;
	Center_Point->Uk_1 = Center_Point->Uk;

	return Center_Point->Uk;
}


//����λ�û�ƫ��������趨ֵ�������뵱ǰ�ٶ�ֵ���ⶨֵ��
int SpeedPID(int t_PlaceOut,int MeasuSpeed)
{
#ifdef  _TEST_Speed
SEGGER_RTT_printf(0,"%5d\t",t_PlaceOut);	
//printf("%5d\t",t_PlaceOut);
#endif
	
//	write_double_buf(p_double_buf,(u8 *)&MeasuSpeed,4);
//	Send1Int(MeasuSpeed);
//	Write_RingBuff(MeasuSpeed);
	
	if(t_PlaceOut!=0)
	{	
		Speed_Point->ek_0 = (float)(t_PlaceOut-MeasuSpeed);            //ƫ��
		if(Speed_Point->ek_0<2 && Speed_Point->ek_0>-2)        //-2~2������pid
		{
			Speed_Point->ek_0=0;
			Speed_Point->Uk = Speed_Point->Uk_1;
		}else{                    //ƫ����󣬲Ž���PID
			Speed_Point->Udk = Speed_Point->P*(Speed_Point->ek_0-Speed_Point->ek_1)+Speed_Point->I*Speed_Point->ek_0+Speed_Point->D*(Speed_Point->ek_0+Speed_Point->ek_2-2*Speed_Point->ek_1);

#ifdef  _TEST_Speed
SEGGER_RTT_printf(0,"+%4d\t",(int)Speed_Point->Udk);	
//printf("+%4d\t  ",(int)Speed_Point->Udk);
#endif
	
			Speed_Point->Uk = Speed_Point->Uk_1+Speed_Point->Udk;

//			if(Speed_Point->Uk*t_PlaceOut < 0)     Speed_Point->Uk = 0;    //������趨ֵ�����ˣ�˵����������
			if(Speed_Point->Uk*t_PlaceOut < 0)     Speed_Point->Uk = t_PlaceOut>0?1:-1;    //������趨ֵ�����ˣ�˵����������
		}
		
		if(Speed_Point->Uk >= 1080)        Speed_Point->Uk = 1080;
		else if(Speed_Point->Uk <= -1080)  Speed_Point->Uk = -1080;
		
		//������������һ�μ���
		Speed_Point->ek_2 = Speed_Point->ek_1;
		Speed_Point->ek_1 = Speed_Point->ek_0;
		Speed_Point->Uk_1 = Speed_Point->Uk;
		
		Speed_Point->Target = Speed_Point->Uk;         
	}
	else                   //λ�û����ƫ��Ϊ0�����ٶȲ���˲���С��0��������Ҫ���м��ٿ���
	{
		Speed_Point->Uk_1 = 0;
		Speed_Point->ek_0 = 0; 
		Speed_Point->ek_1 = 0;
		Speed_Point->ek_2 = 0;
		Speed_Point->Target = 0;
	}
#ifdef  _TEST_Speed
SEGGER_RTT_printf(0,"S%5d\t",MeasuSpeed);
SEGGER_RTT_printf(0,"P%4d\t\n",(int)Speed_Point->Target);
//printf("S%5d\t",MeasuSpeed);
//printf("P%4d\t  \n",(int)Speed_Point->Target);
#endif
	return(Speed_Point->Target);
}


/*********************************λ��PID����************************************/
int PlacePID(int PlaceEk,int LimitSpeed)
{   
	//PID�����õ��ݴ�
//	u8 Gain_Data = gGainData*gAutoSpeed/1000;
	float kp = gGainData*Place_Point->P;
	float ki = gGainData*Place_Point->I;
	float kd = gGainData*Place_Point->D;

	s8  Int_point=20;          //���ַ����,ƫ����-20~20֮��Ž��л�������                

	Place_Point->ek_0 = PlaceEk;   //����ƫ��
 
	if((Place_Point->ek_0<=Int_point)&&(Place_Point->ek_0>=-Int_point))                         //���С�ڻ��ַ���������I
	{
		Place_Point->Flg = 1;
		Place_Point->ek_Sum += Place_Point->ek_0; 
	}else{
		Place_Point->Flg=0;
		Place_Point->ek_Sum=0;   //���ֺ�����
	}
	
	if(Place_Point->ek_0==0)
	{
		Place_Point->Uk = 0;
		Place_Point->ek_Sum=0;
	}else{
		Place_Point->Uk=(int)((kp*Place_Point->ek_0) + Place_Point->Flg*ki*Place_Point->ek_Sum + kd*(Place_Point->ek_0-Place_Point->ek_1));
	}
	
	if(Place_Point->Uk > LimitSpeed)       {Place_Point->Uk = LimitSpeed;   }                             //���ֵ�޷�����
	if(Place_Point->Uk < -LimitSpeed)      {Place_Point->Uk = -LimitSpeed;  }	
	Place_Point->ek_1 = Place_Point->ek_0;  
	
#ifdef _TEST_Place
	printf("%5.2f\t",Place_Point->Uk);
#endif	
	return Place_Point->Uk;
}



int SPCPID(int SensorValue,int SPC_Out,int SPCBit)
{
	static int LastSPCBit;
	SPC_Point->ek_0 = 0-SensorValue;

	if(LastSPCBit != SPCBit)   //˵��������,������ֺ�
	{
		SPC_Point->ek_Sum =0;
	}
	if((SPC_Point->ek_0<=500) && (SPC_Point->ek_0>=-500))
	{	
		SPC_Point->Flg = ((SPC_Point->ek_0<=100)&&(SPC_Point->ek_0>=-100))?1:0;
			
		SPC_Point->ek_Sum += SPC_Point->ek_0;
		SPC_Point->Uk = SPC_Point->P*SPC_Point->ek_0+SPC_Point->Flg*SPC_Point->I*SPC_Point->ek_Sum+SPC_Point->D*(SPC_Point->ek_0-SPC_Point->ek_1);

		if(SPC_Point->Uk > SPC_Out)        SPC_Point->Uk = SPC_Out; 
		if(SPC_Point->Uk < 0)  			   SPC_Point->Uk = 0; 
	}else{             //ƫ����󣬲�����pi����
		if(SPC_Point->ek_0 > 100)       SPC_Point->Uk = SPC_Out;  
	}
	
	//������������һ�μ���
	SPC_Point->ek_1 = SPC_Point->ek_0;
	LastSPCBit = SPCBit;
	
#ifdef  _TEST_SPC
printf("E%4d\t",SPCBit);
printf("E%4.4f\t",SPC_Point->ek_0);
printf("P%4.4f\t  \n",SPC_Point->Uk);
#endif
	
	return SPC_Point->Uk;
}

//int SPCPID(int SensorValue,int SPC_Out)
//{
//	SPC_Point->ek_0 = 0-SensorValue;

//	if((SPC_Point->ek_0<=10) && (SPC_Point->ek_0>=-10))
//	{
//		SPC_Point->Udk = (int)(SPC_Point->P*(SPC_Point->ek_0-SPC_Point->ek_1)+SPC_Point->I*SPC_Point->ek_0);
//		SPC_Point->Uk  = SPC_Point->Uk_1+SPC_Point->Udk;
//		if(SPC_Point->Uk > SPC_Out)        SPC_Point->Uk = SPC_Out; 
//		if(SPC_Point->Uk < 0)  			   SPC_Point->Uk = 0; 
//	}
//	else             //ƫ����󣬲�����pi����
//	{
//		if(SPC_Point->ek_0 > 10)       SPC_Point->Uk = SPC_Out;  
//	}
//	
//	//������������һ�μ���
//	SPC_Point->ek_2 = SPC_Point->ek_1;
//	SPC_Point->ek_1 = SPC_Point->ek_0;
//	SPC_Point->Uk_1 = SPC_Point->Uk;
//	
//#ifdef  _TEST_SPC
//printf("E%4.4f\t",SPC_Point->ek_0);
//printf("D%4.4f\t",SPC_Point->Udk);
//printf("P%4.4f\t  \n",SPC_Point->Uk);
//#endif
//	
//	return SPC_Point->Uk;
//}


