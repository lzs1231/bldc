#include "handle.h"
#include "main.h"
#include "pid_control.h"
#include "stmflash.h"
#include "lcdtest.h"
#include "longkey.h"
#include "speedplan.h"

#define CalibSpeed 400             	 	//У׼�ٶ�100
#define SPC_Out_Max 3000
#define HighVal 1516					//��ѹ����28V
#define LowVal 	1070           		    //��ѹ����20V	
#define StopVal 557 				    //��ѹ����16V��Ӧ891    10V��Ӧ557




u16  HallRate;                 		 //��ǰ��������г��еı���  

u8 OcclusionL_rate,OcclusionR_rate;       //�������ڵ���
     


int GetPolar(int out, u16 palor)
{
	switch(palor)
	{
		case 0:		return out;
		case 1: 	return (-out);
		default: 	return 0;
	}
}

int SetZone(int dev)      			//����ä��
{
	if((dev<=gDeadZone)&&(dev>=-gDeadZone)) //ä��֮�ڲ����
	{
		return 0;
	}
	else if(dev>gDeadZone)			//ƫ��Ϊ����ä��5��ä��û���ڵ�
	{     
		return (dev-gDeadZone);     //���outΪ����������
	}
	else 							//ƫ��Ϊ����ä����ȫ�ڵ�
	{                    	
		return (dev+gDeadZone);     //���outΪ�����������
	}
}


//����ٶ�����
int Speed_limit(int out, int ASpeed)
{
	if((out<-ASpeed))    return(-ASpeed);         //��ѯҪ����ĵ�ѹ�Ƿ����趨��������ֵ���絽�˾��޶���������ֵ
	else if(out>ASpeed)  return(ASpeed);
	else   				 return(out);  			//û���Ͳ�����
}

int LimitOutput(int SOut, int DirOut, int TorqueEK, u16 SetTorque)
{
	int IOut;
	
	switch(DirOut)                                 //�������ź�����
	{														   
		 case -1: IOut=SOut+(TorqueEK<<4);  if(IOut>-SetTorque) IOut = -200;  break;//I_Out = LimitOutput(I_Out,-1);  break;
		 case  0: IOut=0;                                   break;
		 case  1: IOut=SOut-(TorqueEK<<4);  if(IOut<SetTorque)  IOut = 200;   break;//I_Out = LimitOutput(I_Out,1);  break;    //�����г��ޣ�������С���
		 default:  break;
	}
	return IOut;
}


//���ϼ�����
int No_Material_Detection(int out)                  //gNoWaitEN 0�ص�����;1���ֹͣ;2�޲���;
{  
	static u16 DetectNum=0;
	static u16 WaitNum=0;
	u16 DetectTime = gNoDetectTime*20;              //���ʱ�䣬
	u16 WaitTime   = gNoWaitTime*20;
	
	switch(gNoWaitEN)
	{
		case 0:			
			if((out<-gNoDetectValve)||(out>gNoDetectValve)) //ƫ�����ֵ���ȴ�
			{
				if(DetectNum<DetectTime)        //���ʱ��С���趨ʱ��
				{
					DetectNum++;
					return out;
				}
				else 							//���ʱ������趨ʱ�䣬��ֹͣ���
				{   
					Warm[BreakAlarmFlag] = BreakAlarmFlag;
					gWorkMode = 2;
					LongPortFun[PusherCenter]=5;	 
					return 0;
				}
			}
			else
			{
				LongPortFun[PusherCenter]=0;
				Warm[BreakAlarmFlag] = NoErrFlag;
				DetectNum=0;
				WaitNum=0;	 
				return out;
			}
		case 1:		
			if((out<-gNoDetectValve)||(out>gNoDetectValve)) //ƫ�����ֵ���ȴ�
			{
				if(DetectNum<DetectTime)        //���ʱ��С���趨ʱ��
				{
					DetectNum++;
					return out;
				}
				else							//���ʱ������趨ʱ�䣬�͵ȴ�
				{              
					Warm[BreakAlarmFlag] = BreakAlarmFlag;
					if(WaitNum < WaitTime)      //�ȴ�һ��ʱ��
					{
						if(*pHKeepWait == 0) WaitNum++;
						return 0;
					}
					else
					{
						return out; 
					}
				} 
			}
			else
			{
				Warm[BreakAlarmFlag] = NoErrFlag;
				DetectNum=0;
				WaitNum=0;	 
				return out;
			}	
		break;
		case 2:		return out;	
		default:	return 0;	
	}
	
}

int WorkModeOut(s8 OcclusionLrate,s8 OcclusionRrate)
{
	int out;

	switch(gWorkMode)               //����ģʽ
    {
        case 0:          			//����㶯�ƶ�״̬  ���������ƫ��   
			out = 0;					
		break;                    		
			
	    case 1: 
			switch(gSensorMode)   	//�жϴ�����ѡ��
			{
				case 0:	 out = SetZone(gFineTune-80-OcclusionLrate);	   break;        //�趨ֵ-�ⶨֵ=ƫ� ƫ��Ϊ����������   ƫ��Ϊ�����������
				case 1:  out = SetZone(gFineTune-80-OcclusionRrate);	   break;
				case 2:	 if(((OcclusionLrate>90)&&(OcclusionRrate>90))||((OcclusionLrate<-90)&&(OcclusionRrate<-90)))	
						 {
							out=0;
						 }else{
							out = SetZone(OcclusionRrate-OcclusionLrate+gFineTune-80);
						 }	   
				break;
				case 3:  out=SPC_Control(OcclusionLrate,OcclusionRrate);				   break;
				default: out = 0;        break;
			}
			out = GetPolar(out, gAutoPolar);      		  //�Զ����ٵļ����л�
		
			if(gSensorMode!=3)                            //���������SPCģʽ�ͽ���
			{
				out=PlacePID(No_Material_Detection(out), gAutoSpeed);            //ƫ��deviation����PID���������
			} 
			out=Speed_limit(out, gAutoSpeed);             //�Զ�ʱ������ٶ�����=���3000
			
			if(((gLongIo0Mode==6)||(gLongIo1Mode==6)||(gLongIo2Mode==6)||(gLongIo3Mode==6))&&LongPortFun[OperPrepar]==0)
			{
				Warm[RunReadyFlag] = RunReadyFlag;
				out=0;              //�����������׼����������Ч���������Զ�ģʽ
			}
			else
			{
				Warm[RunReadyFlag] = NoErrFlag;
			}
		break;   //�Զ�		   ��ͣ
			
		case 2:
			
			out = CenterPID(gCenterLimit*10-HallRate,gCentSpeed);//���Ĺ�λ
		break;
			
        default: out = 0;  break;
    }
	
	switch(ClickButton|LongPortFun[PusherLeft])         //�ж��Ƿ����ֶ���ť����		
	{
		case 0:													break;							
		case 1:		out = GetPolar(gManuSpeed, gManuPolar);		break;
		case 2:		out = -GetPolar(gManuSpeed, gManuPolar);	break;
		case 3:		out = 0;									break;	
		default:	out = 0;									break;									
	}
	out = SpeedPlan(out);
	
	return LimitProcessing(out);
}

u8 getKeepOutRate(u16 SensorValue,u16 SensorH,u16 SensorL)
{
	float f; 
	//���ñ���ĵ�ǰ���ϵĶ�Ӧ�����������ֵ����Сֵ��
	f=200.0*(SensorH-SensorValue)/(SensorH-SensorL);    		//������������ڵ���-��Сֵ��ȫ���ڵ���
	if(f<0)       {f=0;}
	if(f>200.0)   {f=200.0;}  
	return f;//��Χ0~200
}

/**********************�������***************************/
int PlaceOutHandle(u16 sensorL,u16 sensorR)
{
	int out;      
	
	switch(gSensorSignal)
	{
		case 1:	
			OcclusionL_rate = getKeepOutRate(sensorL,*MatCal.MatValue[*MatCal.CurMatNum].PSensorL_H,*MatCal.MatValue[*MatCal.CurMatNum].PSensorL_L);	
			OcclusionR_rate = getKeepOutRate(sensorR,*MatCal.MatValue[*MatCal.CurMatNum].PSensorR_H,*MatCal.MatValue[*MatCal.CurMatNum].PSensorR_L);
		break;
		case 2:	
			OcclusionL_rate = getKeepOutRate(sensorL,2979,125);
			OcclusionR_rate = getKeepOutRate(sensorR,2979,125);
		break;
		case 4:	
			OcclusionL_rate = getKeepOutRate(sensorL,2979,125);
			OcclusionR_rate = getKeepOutRate(sensorR,2979,125);
		break;
		default:	break;
	}
	
	if(Warm[NotConFlag] == NotConFlag)	return 0;
	
	HallRate= (u16)(gCurrentPulseNum*1000/gMAXPulseNum); //���㵱ǰ����ռ���г̵�ǧ�ֱ�

	//���У׼�г̱�־Ϊ1����ʾ��ҪУ׼�г�;�粻���ڲ����г̾ͽ�����λ����
	out = (TravelCal.CaliFlag==1 ? TravelCalibration() : WorkModeOut(OcclusionL_rate-100,OcclusionR_rate-100));                           

    return out;                                      //�������ֵ     
}  

//spc����
int SPC_Control(int V1,int V2)
{
	static int SPC_bit=1;
	static int Last_HallRate;
    unsigned int time=gSPCStopTime*25;
	int spc_out_stop=gSPCExtendLimit*10; 
	int spc_in_stop=gSPCIndentLimit*10;
	int DEC_S_Min= spc_in_stop+30;
	int DEC_S_Max= spc_out_stop-30;
	int hall_change=0;
    int out=0;
	static u32 num_spc_time=0;            //ֹͣʱ�������
	
    if(gSPCMode==0)//�ڲ����������趨��������λ�㣩
	{
		hall_change = HallRate-Last_HallRate;       	//λ�øı䷽��
		switch(SPC_bit)
		{
		    case -1: 
				if(HallRate<=spc_in_stop)		 	    // ��ǰ���嵽����λ��<10
				{
					 num_spc_time++;
					 if(num_spc_time<time)		    	//ֹͣ0.2��=50*4ms��50��
					 {
						  out=0;
					 }else{                            	//ֹͣʱ��ﵽ���������
						  num_spc_time=0;			 	//���㣬����
						  SPC_bit=-SPC_bit;
					 }
				}else{								    //��ǰ����û�е�����λ��
					 num_spc_time=0;  
					 if((HallRate<=DEC_S_Min)&&(HallRate>spc_in_stop))     //������λ��ܽ���100~130
					 {
						 if(hall_change<0)			    //��ʾSPC��������
						 {
							 out=(SPC_bit*SPC_Out_Max*(HallRate-spc_in_stop))/30;		//Խ�ӽ�������λ�㣬���ԽС
						 }else{
							 out=SPC_bit*SPC_Out_Max;
						 }
					 }else{
						 out=SPC_bit*SPC_Out_Max;
					 }
				}
			break;
			case 1:
				if(HallRate>=spc_out_stop)
				{
					num_spc_time++;
					if(num_spc_time<time)
					{
						out=0;
					}else{
						 num_spc_time=0;
						 SPC_bit=-SPC_bit;
					}
				}else{
					num_spc_time=0;
					if((HallRate>=DEC_S_Max)&&(HallRate<spc_out_stop))   //870~900
					{
						if(hall_change>0)
						{
							out=(SPC_bit*SPC_Out_Max*(spc_out_stop-HallRate))/30;
						}else{
							out=SPC_bit*SPC_Out_Max;
						}
					}else{
						out=SPC_bit*SPC_Out_Max;
					}
				}
			break;
			default: out=0;  break;
		}
		Last_HallRate=HallRate;
	}else{  //�ⲿ������
	    switch(SPC_bit)
	    {
			case -1: 
				if(V1>0)           //-100~100
				{
					num_spc_time++;
					if(num_spc_time<time)
					{
						out=0;
					}else{
						num_spc_time=0;
						SPC_bit=-SPC_bit;
					}
				}else{
					out = SPCPID(V1,SPC_Out_Max,SPC_bit);
				}
				break;
				case 1:
				if(V2>0)    //-100~100
				{
					num_spc_time++;
					if(num_spc_time<time)
					{
						out = 0;
					}else{
						num_spc_time=0;
						SPC_bit=-SPC_bit;
					}
				}else{
					out = SPCPID(V2,SPC_Out_Max,SPC_bit);
				}
			break;
			default: out=0;  break;
		}
	}
	return out; 
}

/************************��λ�����ӳ���******************************/
int LimitProcessing(int P)
{
	int OUT;
	u16 stop_Max = gExtendLimit*10;
	u16 stop_Min = gIndentLimit*10;
	u16 DEC_S_Min = stop_Min+30;      //�������������λ��>20       70~920֮��ȫ������
	u16 DEC_S_Max = stop_Max-30;
	u8  LimitFlag1,LimitFlag2;
	float f1,f2;
	
	if(gLimitMode == 0) //�ڲ���λ
	{
		if((HallRate>DEC_S_Min) && (HallRate<DEC_S_Max))         // 70~930֮��ȫ������
		{
			Warm[LimitFlag] = NoErrFlag;
			OUT = P;
		}else{
			if((HallRate>=stop_Max)||(HallRate<=stop_Min))     //������λ��
			{
				Warm[LimitFlag] = LimitFlag;
				if(HallRate >= stop_Max)            //>=950
				{
					OUT = (P<0?P:0);
				}
				if(HallRate <= stop_Min)            //<=50
				{
					OUT =  (P>0?P:0);
				}
			}else{  //��û�е�����λ�㣬�����������������λ��<20������ܽ���
				Warm[LimitFlag] = NoErrFlag;
				if((HallRate<=DEC_S_Min)&&(HallRate>stop_Min))      //50~70
				{
					f1 = (HallRate-stop_Min)/20.0;
					if(f1>1) f1=1;
					//���������������λ�㿿������ʱ��Ҫ�����ٶ�       ���Զ��������λ�㣬���ý����ٶ�����
					OUT =  (P<0?(int)(P*f1):P);
				}
				if((HallRate>=DEC_S_Max) && (HallRate<stop_Max))      //930~950
				{
					f2 = (stop_Max-HallRate)/20.0;
					if(f2>1) f2=1;
					//��������������λ�㿿������ʱ��Ҫ�����ٶ�        ���Զ�������λ�㣬���ý����ٶ�����
					OUT =  (P>0?(int)(P*f2):P);
				}
			}
		}
	}
	else if(gLimitMode==1)           //�ⲿ��λ
	{		   
		if(LongPortFun[Limit1]==0&LongPortFun[Limit2]==0)        //��ѯ��λ��־�Ƿ���Ч
		{ 
			Warm[LimitFlag] = NoErrFlag;
			OUT =  P;                            //��û����λ���������
		}else{
			Warm[LimitFlag] = LimitFlag;
			if(LongPortFun[Limit1]==1)           //����������λ���������³���
			{
				OUT =  (P<0?P:0);
			}
			if(LongPortFun[Limit2]==1)           //����������λ���������³���
			{
				OUT =  (P>0?P:0);
			}
		}
	}
	else if(gLimitMode==2)
	{
		OUT =  P;

		if((HallRate>=stop_Max)||(HallRate<=stop_Min))  //������λ��
		{
			LimitFlag1 = 1;
			if(HallRate>=stop_Max)            //>=950
			{
				OUT = (P<0?P:0);
			}
			if(HallRate<=stop_Min)            //<=50
			{
				OUT =  (P>0?P:0);
			}
		}else if((stop_Max-HallRate<30)||(HallRate-stop_Min<30)){  	//��û�е�����λ�㣬�����������������λ��<20������ܽ���
			LimitFlag1=0;
			if((HallRate<=DEC_S_Min)&&(HallRate>stop_Min))     		 //50~70
			{
				f1 = (HallRate-stop_Min)/20.0;
				if(f1>1) f1=1;
				//���������������λ�㿿������ʱ��Ҫ�����ٶ�       ���Զ��������λ�㣬���ý����ٶ�����
				OUT =  (P<0?(int)(P*f1):P);
			}
			if((HallRate>=DEC_S_Max)&&(HallRate<stop_Max))     		//930~950
			{
				f2 = (stop_Max-HallRate)/20.0;
				if(f2>1) f2=1;
				//��������������λ�㿿������ʱ��Ҫ�����ٶ�        ���Զ�������λ�㣬���ý����ٶ�����
				OUT =  (P>0?(int)(P*f2):P);
			}
		}else{
			LimitFlag1=0;
		}
		
		if(LongPortFun[Limit1]==0&&LongPortFun[Limit2]==0)        //��ѯ��λ��־�Ƿ���Ч
		{ 
			LimitFlag2 = 0;
		}else{
			LimitFlag2 = 1;
			if(LongPortFun[Limit1]==1)              //����������λ���������³���
			{
				OUT =  (OUT<0?OUT:0);
			}
			if(LongPortFun[Limit2]==1)             //����������λ���������³���
			{
				OUT =  (OUT>0?OUT:0);
			}
		}
		if(LimitFlag1||LimitFlag2 != 0)		Warm[LimitFlag] = LimitFlag;
		else								Warm[LimitFlag] = NoErrFlag;
	}
	return OUT;
}


/***************�г�����ӦУ׼*******************/
int TravelCalibration(void)
{
	static u16 duzhuan_num=0;
	static int LastPulseNum=0;
	static int LastOut=0;
	
    int out=0;
	int Variation;                     				   //�仯��
	
	Warm[LimitFlag] = NoErrFlag;
	switch(TravelCal.CaliStep)
	{
	    case 0: 
		       if(TravelCal.StallDir==-1)		       //��������˶�ת
		       {
			   	   out=0;					           //ֹͣ���
				   gCurrentPulseNum=0;	               //��0����
				   TravelCal.CaliStep=1;               //ת����һ��
			   }else{
			   	   out=-CalibSpeed;               		//��û����ת�ͼ������,����
			   }
		break;
		case 1:
			   if(TravelCal.StallDir==1)                //�������˶�ת
		       {
			      out=0;
			   	  gMAXPulseNum=gCurrentPulseNum;
				  TravelCal.CaliStep=2;
			   }else{
			   	  out=CalibSpeed;                 //��û����ת�ͼ�����������
			   }
		break;
		case 2: 
			out = CenterPID(gCenterLimit*10-HallRate,gCentSpeed);
		break;
	}
	
	
    if(duzhuan_num<160)	                    		 //�Ƚ�ʱ��=800MS
	{
	     if(duzhuan_num==0)		            		 //��ʼʱ��¼��������
	     {
	          LastPulseNum= gCurrentPulseNum;
    	 }
		 duzhuan_num++;		                		 //ʱ��û��
	}else{										    
		 duzhuan_num=0;								 //��0ʱ�������
		 Variation = LastPulseNum-gCurrentPulseNum;  //��ʱ��ʱ�ٱȽϻ����仯����
			

		 if(Variation<4 && Variation>-4)             //��������仯��,�ڹ涨ʱ������仯��������4������Ϊ��ת��
		 {
			TravelCal.StallDir = GetDirection(out);	 //��ȡ�����ѹ�ķ���           
		 }
	}
	out = T_TCplan(out,LastOut,50,0);
	LastOut = out;
	
    return out;
}

/*************��������*******************/
int CurrentProtection(int S_Out,u16 IBus,u16 UBus)   
{
	static int Dir_I_stop=0;
	static u8 bit_I=0;
	static u16 num_I;
	static u16 num_I_stop;
	int I_Out=0;
	static int FuncEK ;                 //��������ʱ������ֵ,�����������趨������ֵFunctionEK
	static int CaliEK ;                 //У׼ʱ������ֵ CalibrEK    ʵ�ʵ���I*0.01(����)*10(�Ŵ���)=BusI*3.3/4096
	int Dir_Xinghao;                    //�����źŵķ���	

	int CaliTorque[4] = {100,200,300,400};
	int FuncTorque[9] = {150,200,350,500,600,700,800,900,1000};     //2,3,4,5,6,7,8,9,10
	
	
	Dir_Xinghao = GetDirection(S_Out);   //��ȡ�����ѹ�ķ���

/************�������������Ƶ���***********/	
	if(TravelCal.CaliFlag==1)            //У׼ʱ�ĵ�������
	{
		CaliEK = IBus-CaliTorque[gCaliTorque-1];
		if(CaliEK>0)                  	 //�����г���
			I_Out = LimitOutput(S_Out,Dir_Xinghao,CaliEK,CaliTorque[gCaliTorque-1]);    //�������������
		else
			I_Out=S_Out;
	}
	else
	{    		//��������ʱ�ĵ�������
		FuncEK = IBus-FuncTorque[gFuncTorque-2];
		if(bit_I==0)                                                        //���������û�йر���� 
		{
		    if(S_Out==0)                                                    //�ź�����Ϊ0����ֹͣ���м����������ҲΪ0
			{
			    num_I=0;
				num_I_stop=0;
				I_Out=0;
			}
			else
			{
			    if(num_I<10000)                                             //2���ʱ
				{
				    num_I++;
					if(FuncEK>0)                                            //�����г���
					{
						Warm[OverrunFlag] = OverrunFlag;
					    num_I_stop++;                                       //2���ڵ������޴���

						I_Out = LimitOutput(S_Out,Dir_Xinghao,FuncEK,FuncTorque[gFuncTorque-2]);      //�������ź�����								      
					}
					else
					{                                                 //û�е������޾��������
						I_Out = S_Out;  
						Warm[OverrunFlag] = NoErrFlag;
					}                                      
				}
				else
				{                                                      //2���ʱ����
				    if(num_I_stop>=5000)                                    //��������޴����ﵽ1�����Ͼ͹ر����
					{ 
					     I_Out=0;
		                 bit_I=1;                                           //��ʾ������־
						 Warm[StallFlag] = StallFlag;
						 Dir_I_stop=Dir_Xinghao;                            //��������ر�ʱ���źŷ���		
					}
					else
					{
						I_Out=S_Out;
					}
					num_I_stop=0;                                           //��0������
					num_I=0;
				}
	        }
		}
		else
		{
		     if((Dir_Xinghao!=Dir_I_stop)&&(Dir_Xinghao!=0)&&(Dir_I_stop!=0))//�з����ź���������������
			 {
				 Warm[StallFlag] = NoErrFlag;
			     I_Out=S_Out;
	             bit_I=0;		
			 }
			 else
			 {
			     I_Out=0;
			 }
		 }
	}
	
/************�����⣬�ⲿ�����Դ����20V�����28V����ֹͣ������***********/
   if((UBus<=HighVal) && (UBus>=LowVal))
   {
	   Warm[VolHighFlag] = NoErrFlag;
	   Warm[VolLowFlag]  = NoErrFlag;
//	   PBout(12) = 0;	
   }
   else
   {
	   if(UBus>HighVal) Warm[VolHighFlag] = VolHighFlag; //��ѹ����
	   if(UBus<LowVal)  Warm[VolLowFlag]  = VolLowFlag;   //��ѹ���� 
//	   PBout(12) = 1;	
   	   I_Out = 0;
	   if(UBus<=StopVal)                       			 //��ѹ����16V��Ӧ891    10V��Ӧ557
	   {
			__ASM volatile("cpsid i");  				 //�ر������ж�
			SavePara();                 				 //��ʼ����洢
	   }
   }
   return(I_Out);
}






