#include "pid_control.h"
#include "sys.h"
#include "string.h"
#include "main.h"

#include "lcdtest.h"
#include "usart.h"	 
#include "doublebuf.h"    //双缓冲区




//增量式PID
typedef struct IncrementPID
{
	float Target;     //设定目标
	float Uk;         //PID输出
	float Udk;        //PID增量
	float Uk_1;       //保存上一次PID输出
	float P;
	float I;
	u8  Flg;        //为1表示进行积分运算，为0表示不进行积分运算
	float D;
	float ek_0;       //当前误差
	float ek_1;       //上一次误差
	float ek_2;       //上上次误差
}IncPID;

//位置式PID
typedef struct PositionPID
{
	float Target;     //设定目标
	float Uk;         //PID输出
	float Uk_1;       //保存上一次PID输出
	float P;
	float I;
	u8  Flg;         //为1表示进行积分运算，为0表示不进行积分运算
	float D;
	float ek_Sum;      //误差积分和
	float ek_0;       //当前误差
	float ek_1;       //上一次误差
}PosPID;

/***********对中PID*************/
IncPID Center_PID;     //定义一个变量，分布了具体的内存空间
IncPID *Center_Point = &Center_PID;   //定义一个指针指向该空间

/***********自动模式PID************/
PosPID Place_PID;     //定义一个变量，分布了具体的内存空间
PosPID *Place_Point = &Place_PID;

/***********速度PID************/
IncPID Speed_PID;     //定义一个变量，分布了具体的内存空间
IncPID *Speed_Point = &Speed_PID;

/***********SPCPID************/
PosPID SPC_PID;     //定义一个变量，分布了具体的内存空间
PosPID *SPC_Point = &SPC_PID;


/*********初始化PID**********/
void PIDInit()
{
	Place_Point->Target = 0;
	Place_Point->Uk = 0;
	Place_Point->Uk_1 = 0;
	Place_Point->P = 0.5;
	Place_Point->I = Place_Point->P/80.0;
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
	SPC_Point->P = 1;
	SPC_Point->I = 0.08;
	SPC_Point->Flg = 1;
	SPC_Point->D = 3;
	SPC_Point->ek_0         = 0;         //ek=0
    SPC_Point->ek_1         = 0;         //ek-1=0
}

int CenterPID(int PlaceEk,int LimitSpeed)
{
	Center_Point->ek_0 = (float)PlaceEk;

	if((Center_Point->ek_0<=50) && (Center_Point->ek_0>=-50))
	{
		if((Center_Point->ek_0<=2) && (Center_Point->ek_0>=-2))          //偏差-1~1，不进行pi运算,留有1的偏差余量，防止抖动
		{
			Center_Point->Uk = 0;
			if(TravelCal.CaliFlag == 1)			//行程校准完毕，清零校准标志
			{		
				TravelCal.CaliStep=3;		
				TravelCal.CaliFlag=0;          
			}
		}else{                          		// 偏差在-50~50之间进行位置环PID运算
			Center_Point->Udk = (Center_Point->P*(Center_Point->ek_0-Center_Point->ek_1)
								 +Center_Point->I*Center_Point->ek_0)
									+Center_Point->D*(Center_Point->ek_0+Center_Point->ek_2-2*Center_Point->ek_1);
			
			Center_Point->Uk = Center_Point->Uk_1+Center_Point->Udk;
			
			if(Center_Point->Uk > LimitSpeed)        Center_Point->Uk = LimitSpeed; 
			else if(Center_Point->Uk < -LimitSpeed)  Center_Point->Uk = -LimitSpeed; 
		}
	}else{             //偏差过大，不进行pi运算
		if(Center_Point->ek_0>50)       Center_Point->Uk = LimitSpeed;  
		else if(Center_Point->ek_0<-50) Center_Point->Uk = -LimitSpeed;  
	}
	
	//保存误差，用于下一次计算
	Center_Point->ek_2 = Center_Point->ek_1;
	Center_Point->ek_1 = Center_Point->ek_0;
	Center_Point->Uk_1 = Center_Point->Uk;

	return Center_Point->Uk;
}


//输入位置环偏差输出（设定值），输入当前速度值（测定值）
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
		Speed_Point->ek_0 = (float)(t_PlaceOut-MeasuSpeed);            //偏差
		if(Speed_Point->ek_0<10 && Speed_Point->ek_0>-10)        //-2~2不进行pid
		{
			Speed_Point->ek_0=0;
			Speed_Point->Uk = Speed_Point->Uk_1;
		}else{                    //偏差过大，才进行PID
			Speed_Point->Udk = Speed_Point->P*(Speed_Point->ek_0-Speed_Point->ek_1)+Speed_Point->I*Speed_Point->ek_0+Speed_Point->D*(Speed_Point->ek_0+Speed_Point->ek_2-2*Speed_Point->ek_1);

#ifdef  _TEST_Speed
SEGGER_RTT_printf(0,"+%4d\t",(int)Speed_Point->Udk);	
//printf("+%4d\t  ",(int)Speed_Point->Udk);
#endif
			Speed_Point->Uk = Speed_Point->Uk_1+Speed_Point->Udk;

			if(Speed_Point->Uk*t_PlaceOut < 0)     Speed_Point->Uk = 0;    //输出与设定值反向了，说明补偿过大
//			if(Speed_Point->Uk*t_PlaceOut < 0)     Speed_Point->Uk = t_PlaceOut>0?1:-1;    //输出与设定值反向了，说明补偿过大
		}
		
		if(Speed_Point->Uk >= LimitI)        Speed_Point->Uk = LimitI;
		else if(Speed_Point->Uk <= -LimitI)  Speed_Point->Uk = -LimitI;
		
		//保存误差，用于下一次计算
		Speed_Point->ek_2 = Speed_Point->ek_1;
		Speed_Point->ek_1 = Speed_Point->ek_0;
		Speed_Point->Uk_1 = Speed_Point->Uk;
		
		Speed_Point->Target = Speed_Point->Uk;         
	}
	else                   //位置环输出偏差为0，但速度不能瞬间减小到0，所以需要进行减速控制
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


/*********************************位置PID运算************************************/
int PlacePID(int PlaceEk,int LimitSpeed)
{   
	//PID运算用的暂存
//	u8 Gain_Data = gGainData*gAutoSpeed/1000;
	float kp = gGainData*Place_Point->P;
	float ki = gGainData*Place_Point->I;
	float kd = gGainData*Place_Point->D;

	s8  Int_point=20;          //积分分离点,偏差在-20~20之间才进行积分运算                

	Place_Point->ek_0 = PlaceEk;   //输入偏差
 
	if((Place_Point->ek_0<=Int_point)&&(Place_Point->ek_0>=-Int_point))                         //误差小于积分分离点就运算I
	{
		Place_Point->Flg = 1;
		Place_Point->ek_Sum += Place_Point->ek_0; 
	}else{
		Place_Point->Flg=0;
		Place_Point->ek_Sum=0;   //积分和清零
	}
	
	if(Place_Point->ek_0==0)
	{
		Place_Point->Uk = 0;
		Place_Point->ek_Sum=0;
	}else{
		Place_Point->Uk=(int)((kp*Place_Point->ek_0) + Place_Point->Flg*ki*Place_Point->ek_Sum + kd*(Place_Point->ek_0-Place_Point->ek_1));
	}
	
	if(Place_Point->Uk > LimitSpeed)       {Place_Point->Uk = LimitSpeed;   }                             //输出值限幅处理
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

	if(LastSPCBit != SPCBit)   //说明反向了,清零积分和
	{
		SPC_Point->ek_Sum =0;
	}
	if((SPC_Point->ek_0<=50) && (SPC_Point->ek_0>=-50))
	{	
		SPC_Point->Flg = ((SPC_Point->ek_0<=10)&&(SPC_Point->ek_0>=-10))?1:0;
			
		SPC_Point->ek_Sum += SPC_Point->ek_0;
		SPC_Point->Uk = SPC_Point->P*SPC_Point->ek_0+SPC_Point->Flg*SPC_Point->I*SPC_Point->ek_Sum+SPC_Point->D*(SPC_Point->ek_0-SPC_Point->ek_1);

		if(SPC_Point->Uk > SPC_Out)        SPC_Point->Uk = SPC_Out; 
		if(SPC_Point->Uk < 0)  			   SPC_Point->Uk = 0; 
	}else{             //偏差过大，不进行pi运算
		if(SPC_Point->ek_0 > 10)       SPC_Point->Uk = SPC_Out;  
	}
	
	//保存误差，用于下一次计算
	SPC_Point->ek_1 = SPC_Point->ek_0;
	LastSPCBit = SPCBit;
	
#ifdef  _TEST_SPC
printf("E%4d\t",SPCBit);
printf("E%4.4f\t",SPC_Point->ek_0);
printf("P%4.4f\t  \n",SPC_Point->Uk);
#endif
	
	return (SPCBit == 1?SPC_Point->Uk:-SPC_Point->Uk);
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
//	else             //偏差过大，不进行pi运算
//	{
//		if(SPC_Point->ek_0 > 10)       SPC_Point->Uk = SPC_Out;  
//	}
//	
//	//保存误差，用于下一次计算
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


