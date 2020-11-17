#include "handle.h"
#include "main.h"
#include "pid_control.h"
#include "stmflash.h"
#include "lcdtest.h"
#include "longkey.h"

#define CalibSpeed 500             	 	//校准速度100
#define SPC_Out_Max 3000
#define HighVal 1516					    //电压高于28V
#define LowVal 	1070           				//电压低于20V	
#define StopVal 557 						//电压低于16V对应891    10V对应557


u16  HallRate;                 		 //当前脉冲点总行程中的比例  

s16 OcclusionL_rate,OcclusionR_rate;       //传感器遮挡率
     
int GetDirection(int out)
{
	if(out==0)		return 0;  
	else if(out<0)	return -1;   
	else			return 1; 
}
int GetPolar(int out, u16 palor)
{
	switch(palor)
	{
		case 0:		return out;
		case 1: 	return (-out);
		default: 	return 0;
	}
}

int SetZone(int dev)      				  //设置盲区
{
	if((dev<=gDeadZone)&&(dev>=-gDeadZone))      //盲区之内不输出
	{
		return 0;
	}else if(dev>gDeadZone){     //偏差为正，盲区5，盲区没有遮挡
		return (dev-gDeadZone);       //输出out为正，电机伸出
	}else{                    	 //偏差为负，盲区完全遮挡
		return (dev+gDeadZone);       //输出out为负，电机缩进
	}
}


//最高速度限制
int Speed_limit(int out, int ASpeed)
{
	if((out<-ASpeed))    return(-ASpeed);         //查询要输出的电压是否到了设定的最高输出值，如到了就限定在最高输出值
	else if(out>ASpeed)  return(ASpeed);
	else   				 return(out);  			//没到就不限制
}

int LimitOutput(int SOut, int DirOut, int TorqueEK, u16 SetTorque)
{
	int IOut;
	
	switch(DirOut)                                 //电流对信号限制
	{														   
		 case -1: IOut=SOut+(TorqueEK*5);  if(IOut>-SetTorque) IOut = -SetTorque;  break;//I_Out = LimitOutput(I_Out,-1);  break;
		 case  0: IOut=0;                                   break;
		 case  1: IOut=SOut-(TorqueEK*5);  if(IOut<SetTorque)  IOut = SetTorque;   break;//I_Out = LimitOutput(I_Out,1);  break;    //电流有超限，慢慢减小输出
		 default:  break;
	}
	return IOut;
}

//无料检测程序
int No_Material_Detection(int out)
{  
	static u8  DetectFlag=0;                      //无料等待是否开启软开关
	static u16 DetectNum=0;
	static u16 WaitNum=0;
	u16 DetectTime = gNoDetectTime*20;           //检测时间，
	u16 WaitTime   = gNoWaitTime*20;

	if((out<-gNoDetectValve)||(out>gNoDetectValve))        //偏差超过阈值，等待
	{
		if(DetectFlag==1)	
		{
			Warm[BreakAlarmFlag] = BreakAlarmFlag;
			return out;
		}else{
			if(DetectNum<DetectTime)        //检测时间小于设定时间
			{
				DetectNum++;
				return out;
			}else{                          //检测时间大于设定时间，就停止输出
				if(WaitNum < WaitTime)      //等待一定时间
				{
					WaitNum++;
				}else{
					DetectFlag=1;  
				}
				return 0;
			} 
		}
	}else{
		Warm[BreakAlarmFlag] = NoErrFlag;
		DetectFlag=0;   //清0内部软开关
		DetectNum=0;
		WaitNum=0;	 
		return out;
	}
	return(out);
}

int WorkModeOut()
{
	int out;

	switch(gWorkMode)               //工作模式
    {
        case 0:          			//进入点动移动状态  ，计算输出偏差   
			out = 0;					
		break;                    		
			
	    case 1: 
			switch(gSensorMode)   	//判断传感器选择
			{
				case 0:	 out = SetZone(gFineTune-80-OcclusionL_rate);	   break;        //设定值-测定值=偏差， 偏差为正，电机伸出   偏差为负，电机缩进
				case 1:  out = SetZone(gFineTune-80-OcclusionR_rate);	   break;
				case 2:	 if(((OcclusionL_rate>90)&&(OcclusionR_rate>90))||((OcclusionL_rate<-90)&&(OcclusionR_rate<-90)))	
						 {
							out=0;
						 }else{
							out = SetZone(OcclusionR_rate-OcclusionL_rate+gFineTune-80);
						 }	   
				break;
				case 3:  out=SPC_Control(OcclusionL_rate,OcclusionR_rate);				   break;
				default: out = 0;        break;
			}
			out = GetPolar(out, gAutoPolar);      		  //自动跟踪的极性切换
		
			if(gSensorMode!=3)                            //如果不是在SPC模式就进行
			{
				if(gNoWaitEN==0)	                      //无料等待使能   
				{
					out=No_Material_Detection(out);       //无料检测如开启就进入
				}		            
				out=PlacePID(out, gAutoSpeed);            //偏差deviation经过PID计算输出量
			} 
			out=Speed_limit(out, gAutoSpeed);             //自动时的最高速度限制=最大3000
			
			if(((gLongIo0Mode==6)||(gLongIo1Mode==6)||(gLongIo2Mode==6)||(gLongIo3Mode==6))&&LongPortFun[OperPrepar]==0)
			{
				Warm[RunReadyFlag] = RunReadyFlag;
				out=0;              //如果开启运行准备，但是无效，则不启动自动模式
			}else{
				Warm[RunReadyFlag] = NoErrFlag;
			}
		break;   //自动		   急停
			
		case 2:
			out = CenterPID(gCenterLimit*10-HallRate,gCentSpeed);//中心归位
		break;
			
        default: out = 0;  break;
    }
	
	switch(ClickButton|LongPortFun[PusherLeft])         //判断是否有手动按钮按下		
	{
		case 0:													break;							
		case 1:		out = GetPolar(gManuSpeed, gManuPolar);		break;
		case 2:		out = -GetPolar(gManuSpeed, gManuPolar);	break;
		case 3:		out = 0;									break;	
		default:	out = 0;									break;									
	}
	
	return out;
}

/**********************输出处理***************************/
int PlaceOutHandle(u16 sensorL,u16 sensorR)
{
	int out;
	float f;         
	
	//调用保存的当前材料的对应传感器的最大值、最小值。
	f=200.0*(*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_H-sensorL)/(*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_H-*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_L);    		//传感器最大（无遮挡）-最小值（全部遮挡）
	if(f<0)       {f=0;}
	if(f>200.0)   {f=200.0;}  
	OcclusionL_rate = (s16)f-100;//范围-100~100
		
	f=200.0*(*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_H-sensorR)/(*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_H-*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_L); 
	if(f<0)      {f=0;}
	if(f>200.0)  {f=200.0;}
	OcclusionR_rate = (s16)f-100;
	
	if(Warm[NotConFlag] == NotConFlag)	return 0;
	
	HallRate= (u16)(gCurrentPulseNum*1000/gMAXPulseNum); //计算当前霍尔占总行程的千分比

	//如果校准行程标志为1，表示需要校准行程;如不是在测量行程就进行限位处理
	out = (TravelCal.CaliFlag==1 ? TravelCalibration() : LimitProcessing(WorkModeOut()));                           

    return out;                                      //更新输出值     
}  

//spc控制
int SPC_Control(int V1,int V2)
{
	static int SPC_bit=1;
	static int Last_HallRate;
    unsigned int time=gSPCStopTime*25;
	int spc_out_stop=gSPCBehindLimit*10; 
	int spc_in_stop=gSPCFrontLimit*10;
	int DEC_S_Min= spc_in_stop+30;
	int DEC_S_Max= spc_out_stop-30;
	int hall_change=0;
    int out=0;
	static u32 num_spc_time=0;            //停止时间计数器
	
    if(gSPCMode==0)//内部编码器（设定的脉冲限位点）
	{
		hall_change = HallRate-Last_HallRate;       	//位置改变方向
		switch(SPC_bit)
		{
		    case -1: 
				if(HallRate<=spc_in_stop)		 	    // 当前脉冲到达限位点<10
				{
					 num_spc_time++;
					 if(num_spc_time<time)		    	//停止0.2秒=50*4ms，50次
					 {
						  out=0;
					 }else{                            	//停止时间达到，电机反向
						  num_spc_time=0;			 	//清零，反向
						  SPC_bit=-SPC_bit;
					 }
				}else{								    //当前脉冲没有到达限位点
					 num_spc_time=0;  
					 if((HallRate<=DEC_S_Min)&&(HallRate>spc_in_stop))     //距离限位点很近了100~130
					 {
						 if(hall_change<0)			    //表示SPC换方向了
						 {
							 out=(SPC_bit*SPC_Out_Max*(HallRate-spc_in_stop))/30;		//越接近缩进限位点，输出越小
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
	}else{  //外部传感器
	    switch(SPC_bit)
	    {
			case -1: 
				if(V1>0)           //-1000~1000
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
					out = SPC_bit*SPCPID(V1,SPC_Out_Max,SPC_bit);
				}
				break;
				case 1:
				if(V2>0)    //-1000~1000
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
					out = SPC_bit*SPCPID(V2,SPC_Out_Max,SPC_bit);
				}
			break;
			default: out=0;  break;
		}
	}
	return out; 
}

/************************限位处理子程序******************************/
int LimitProcessing(int P)
{
	int OUT;
	u16 stop_Max = gBehindLimit*10;
	u16 stop_Min = gFrontLimit*10;
	u16 DEC_S_Min = stop_Min+30;      //脉冲比例距离限位点>20       70~920之间全速运行
	u16 DEC_S_Max = stop_Max-30;
	u8  LimitFlag1,LimitFlag2;
	
	if(gLimitMode == 0) //内部限位
	{
		if((HallRate>DEC_S_Min) && (HallRate<DEC_S_Max))         // 70~930之间全速运行
		{
			Warm[LimitFlag] = NoErrFlag;
			OUT = P;
		}else{
			if((HallRate>=stop_Max)||(HallRate<=stop_Min))     //到达限位点
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
			}else{  //还没有到达限位点，但是脉冲比例距离限位点<20，距离很近了
				Warm[LimitFlag] = NoErrFlag;
				if((HallRate<=DEC_S_Min)&&(HallRate>stop_Min))      //50~70
				{
					//电机继续向缩进限位点靠近，此时需要限制速度       电机远离缩进限位点，不用进行速度限制
					OUT =  (P<0?(int)(P*(HallRate-stop_Min)/30):P);
				}
				if((HallRate>=DEC_S_Max) && (HallRate<stop_Max))      //930~950
				{
					//电机继续向伸出限位点靠近，此时需要限制速度        电机远离伸出限位点，不用进行速度限制
					OUT =  (P>0?(int)(P*(stop_Max-HallRate)/30):P);
				}
			}
		}
	}
	else if(gLimitMode==1)           //外部限位
	{		   
		if(LongPortFun[Limit1]==0&LongPortFun[Limit2]==0)        //查询限位标志是否有效
		{ 
			Warm[LimitFlag] = NoErrFlag;
			OUT =  P;                            //如没有限位则正常输出
		}else{
			Warm[LimitFlag] = LimitFlag;
			if(LongPortFun[Limit1]==1)           //如是缩进限位则运行以下程序
			{
				OUT =  (P<0?P:0);
			}
			if(LongPortFun[Limit2]==1)           //如果是伸出限位则运行以下程序
			{
				OUT =  (P>0?P:0);
			}
		}
	}
	else if(gLimitMode==2)
	{
		OUT =  P;

		if((HallRate>=stop_Max)||(HallRate<=stop_Min))  //到达限位点
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
		}else if((stop_Max-HallRate<30)||(HallRate-stop_Min<30)){  	//还没有到达限位点，但是脉冲比例距离限位点<20，距离很近了
			LimitFlag1=0;
			if((HallRate<=DEC_S_Min)&&(HallRate>stop_Min))     		 //50~70
			{
				//电机继续向缩进限位点靠近，此时需要限制速度       电机远离缩进限位点，不用进行速度限制
				OUT =  (P<0?(int)(P*(HallRate-stop_Min)/30):P);
			}
			if((HallRate>=DEC_S_Max)&&(HallRate<stop_Max))     		//930~950
			{
				//电机继续向伸出限位点靠近，此时需要限制速度        电机远离伸出限位点，不用进行速度限制
				OUT =  (P>0?(int)(P*(stop_Max-HallRate)/30):P);
			}
		}else{
			LimitFlag1=0;
		}
		
		if(LongPortFun[Limit1]==0&&LongPortFun[Limit2]==0)        //查询限位标志是否有效
		{ 
			LimitFlag2 = 0;
		}else{
			LimitFlag2 = 1;
			if(LongPortFun[Limit1]==1)              //如是缩进限位则运行以下程序
			{
				OUT =  (OUT<0?OUT:0);
			}
			if(LongPortFun[Limit2]==1)             //如果是伸出限位则运行以下程序
			{
				OUT =  (OUT>0?OUT:0);
			}
		}
		if(LimitFlag1||LimitFlag2 != 0)		Warm[LimitFlag] = LimitFlag;
		else								Warm[LimitFlag] = NoErrFlag;
	}
	return OUT;
}


/***************行程自适应校准*******************/
int TravelCalibration(void)
{
	static u16 duzhuan_num=0;
	static int LastPulseNum=0;
	
    int out=0;
	int Variation;                     				   //变化量
	
	Warm[LimitFlag] = NoErrFlag;
	switch(TravelCal.CaliStep)
	{
	    case 0: 
		       if(TravelCal.StallDir==-1)		       //如果缩进端堵转
		       {
			   	   out=0;					           //停止输出
				   gCurrentPulseNum=0;	               //清0霍尔
				   TravelCal.CaliStep=1;               //转入下一步
			   }else{
			   	   out=-CalibSpeed;               		//如没到堵转就继续输出,缩进
			   }
		break;
		case 1:
			   if(TravelCal.StallDir==1)                //如果伸出端堵转
		       {
			      out=0;
			   	  gMAXPulseNum=gCurrentPulseNum;
				  TravelCal.CaliStep=2;
			   }else{
			   	  out=CalibSpeed;                 //如没到堵转就继续输出，伸出
			   }
		break;
		case 2: 
			out = CalibPID(gCenterLimit*10-HallRate,gCentSpeed);
		break;
	}
	
	
    if(duzhuan_num<160)	                    		 //比较时间=800MS
	{
	     if(duzhuan_num==0)		            		 //开始时记录霍尔数量
	     {
	          LastPulseNum= gCurrentPulseNum;
    	 }
		 duzhuan_num++;		                		 //时间没到
	}else{										    
		 duzhuan_num=0;								 //清0时间计数器
		 Variation = LastPulseNum-gCurrentPulseNum;  //到时间时再比较霍尔变化数量
			

		 if(Variation<4 && Variation>-4)             //霍尔脉冲变化量,在规定时间内如变化数量少于4，就视为堵转了
		 {
			TravelCal.StallDir = GetDirection(out);	 //提取输入电压的方向           
		 }
	}
	
    return out;
}

/*************电流保护*******************/
int CurrentProtection(int S_Out,u16 IBus,u16 UBus)   
{
	static int Dir_I_stop=0;
	static u8 bit_I=0;
	static u16 num_I;
	static u16 num_I_stop;
	int I_Out=0;
	static int FuncEK ;                 //正常运行时电流差值,测量电流与设定电流差值FunctionEK
	static int CaliEK   ;                //校准时电流差值 CalibrEK    实际电流I*0.01(电阻)*10(放大倍数)=BusI*3.3/4096
	int Dir_Xinghao;                 //输入信号的方向	

	int CaliTorque = gCaliTorque*10;
	int FuncTorque = gFuncTorque*100;
	
	CaliEK = IBus-(gCaliTorque*11.6);
	FuncEK = IBus-(gFuncTorque*116);
	
	Dir_Xinghao = GetDirection(S_Out);   //提取输入电压的方向

/************电流保护，限制电流***********/	
	if(TravelCal.CaliFlag==1)            //校准时的电流控制
	{
		if(CaliEK>0)                  	 //电流有超限
		{
			I_Out = LimitOutput(S_Out,Dir_Xinghao,CaliEK,CaliTorque);    //对输出进行限制
		}else{
			I_Out=S_Out;
		}
	}
	else
	{                     				//正常运行时的电流控制
		if(bit_I==0)                                                        //如果电流还没有关闭输出 
		{
		    if(S_Out==0)                                                    //信号输入为0，就停止所有计数器，输出也为0
			{
			    num_I=0;
				num_I_stop=0;
				I_Out=0;
			}else{
			    if(num_I<20000)                                             //2秒计时
				{
				    num_I++;
					if(FuncEK>0)                                            //电流有超限
					{
						Warm[OverrunFlag] = OverrunFlag;
					    num_I_stop++;                                       //2秒内电流超限次数

						I_Out = LimitOutput(S_Out,Dir_Xinghao,FuncEK,FuncTorque);      //电流对信号限制								      
					}else{                                                 //没有电流超限就正常输出
						I_Out = S_Out;  
						Warm[OverrunFlag] = NoErrFlag;
					}                                      
				}else{                                                      //2秒计时到了
				    if(num_I_stop>=10000)                                    //如电流超限次数达到1半以上就关闭输出
					{ 
					     I_Out=0;
		                 bit_I=1;                                           //显示报警标志
						 Warm[StallFlag] = StallFlag;
						 Dir_I_stop=Dir_Xinghao;                            //保存电流关闭时的信号方向		
					}else{
					     I_Out=S_Out;
					}
					num_I_stop=0;                                           //清0计数器
					num_I=0;
				}
	        }
		}else{
		     if((Dir_Xinghao!=Dir_I_stop)&&(Dir_Xinghao!=0)&&(Dir_I_stop!=0))//有反向信号输入才能重启输出
			 {
				 Warm[StallFlag] = NoErrFlag;
			     I_Out=S_Out;
	             bit_I=0;		
			 }else{
			     I_Out=0;
			 }
		 }
	}
	
/************掉电检测，外部输入电源低于20V或高于28V，就停止电机输出***********/
   if((UBus<=HighVal) && (UBus>=LowVal))
   {
	   Warm[VolHighFlag] = NoErrFlag;
	   Warm[VolLowFlag]  = NoErrFlag;
//	   PBout(12) = 0;	
   }else{
	   if(UBus>HighVal) Warm[VolHighFlag] = VolHighFlag; //电压过高
	   if(UBus<LowVal) Warm[VolLowFlag]  = VolLowFlag;   //电压过低 
//	   PBout(12) = 1;	
   	   I_Out = 0;
	   if(UBus<=StopVal)                       			 //电压低于16V对应891    10V对应557
	   {
			__ASM volatile("cpsid i");  				 //关闭所有中断
			SavePara();                 				 //开始掉电存储
	   }
   }
   return(I_Out);
}






