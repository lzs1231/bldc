#include "sys.h"
#include "main.h"

#define PosNeg(a,b) ((a^b)<0?1:0)     //符号相同返回0

typedef enum {
	state_1a=1,
	state_1b,
	state_2a,
	state_2b,
	state_3a,
	state_3b,
	state_4a,
	state_4b,
	state_5a,
	state_5b,
}State;


typedef enum{
	event_1=1,
	event_2,
	event_3,
	event_4,
	event_5,
}EventID;

typedef int (*Action)(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);

typedef struct {
	State curState;		//当前状态
	EventID eventId;	//事件ID
	State nextState;	//下个状态
	Action action;		//具体表现
}StateTransform;

typedef struct{
	State state;    
	int transNum;
	StateTransform* transform;
}StateMachine;

EventID			inputEvent;
StateMachine	stateMachine;

int decspeed(int SpeedIn,u16 FlexDec);
int T_Mplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);
int T_Aplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);
int T_Cplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);
int reversal(int P,u8 TimeCnt);

int SpeedPlan(int PlaceOut)
{
	static int  LastPlaceOut,SpeedIn;
	
	switch(gWorkMode)
	{
		case 0:	
			/*当前状态为手动模式*/
			inputEvent = event_1;	    		 //事件1
			
			
			if(stateMachine.state != state_1b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
			{
				stateMachine.state = state_1a;
				SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
				LastPlaceOut = SpeedIn;
				if(SpeedIn == 0)		    	 //减速状态结束
				{
					stateMachine.state = state_1b;
				}
			}else{
				stateMachine.state = state_1b;    
				SpeedIn = T_Mplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
				LastPlaceOut = SpeedIn;
			}
		break;
		case 1:			
			if(LongPortFun[PusherLeft]!=0)    	  			//远程点动打开
			{
				/*当前状态为自动远程点动开*/
				inputEvent = event_2;			 //事件2
				
		
				if(stateMachine.state != state_2b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
				{
					stateMachine.state = state_2a;
					SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
					LastPlaceOut = SpeedIn;
					if(SpeedIn == 0)		    	 //减速状态结束
					{
						stateMachine.state = state_2b;
					}
				}else{
					stateMachine.state = state_2b; 
					SpeedIn = T_Mplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
					LastPlaceOut = SpeedIn;
				}
			}else{	
				/*当前状态为自动远程点动关*/
				inputEvent = event_3;			 //事件3
				
				
				if(stateMachine.state != state_3b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
				{
					stateMachine.state = state_3a;
					SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
					LastPlaceOut = SpeedIn;
					if(SpeedIn == 0)		    	 //减速状态结束
					{
						stateMachine.state = state_3b;
					}
				}else{
					stateMachine.state = state_3b; 
					SpeedIn = T_Aplan(reversal(PlaceOut,5), LastPlaceOut, gAFlexAcc, gAFlexDec);
					LastPlaceOut = SpeedIn;
				}
			}
		break;
		
		case 2:	
			if(LongPortFun[PusherLeft]!=0 || ClickButton!=0){    	//远程点动打开
				/*当前状态为中心点动开*/
				inputEvent = event_4;			 //事件4
				
				
				if(stateMachine.state != state_4b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
				{
					stateMachine.state = state_4a;
					SpeedIn = decspeed(LastPlaceOut,gMFlexDec<<1);
					LastPlaceOut = SpeedIn;
					if(SpeedIn == 0)		    	 //减速状态结束
					{
						stateMachine.state = state_4b;
					}
				}else{
					stateMachine.state = state_4b; 
					SpeedIn = T_Mplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
					LastPlaceOut = SpeedIn;
				}
			}else{	
				/*当前状态为中心点动关*/						
				inputEvent = event_5;			//事件5
				
				
				if(stateMachine.state != state_5b && PosNeg(LastPlaceOut,PlaceOut))                 	 //减速状态
				{
					stateMachine.state = state_5a;
					SpeedIn = decspeed(LastPlaceOut,gMFlexDec);
					LastPlaceOut = SpeedIn;
					if(SpeedIn == 0)		    	 //减速状态结束
					{
						stateMachine.state = state_5b;
					}
				}else{
					stateMachine.state = state_5b; 
					SpeedIn = T_Cplan(reversal(PlaceOut,10), LastPlaceOut, gMFlexAcc, gMFlexDec);
					LastPlaceOut = SpeedIn;
				}						
			}						
		break;
		default:		
		break;
	}
	
//	runStateMachine(&stateMachine, inputEvent);   //根据触发事件与当前状态，执行对应功能函数并切换下一个状态。
//				
//	runState(&stateMachine,inputEvent);
	
	return SpeedIn;
}

int decspeed(int LastPlaceOut,u16 FlexDec)
{
	int SpeedIn;
	
	if(LastPlaceOut > 0)	
	{
		SpeedIn = LastPlaceOut-(FlexDec<<1);
		if(SpeedIn < 0)	SpeedIn = 0;
	}else if(LastPlaceOut < 0)	{
		SpeedIn = LastPlaceOut+(FlexDec<<1);
		if(SpeedIn > 0)	SpeedIn = 0;
	}
	return SpeedIn;
}

int T_Mplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec)
{
	int SpeedIn,PEk;
	
	PEk = PlaceOut-LastPlaceOut;
	if((PlaceOut>0)&&(PEk > 0))
	{	
		SpeedIn = LastPlaceOut+acc;
		if(SpeedIn>PlaceOut)   SpeedIn = PlaceOut;
	}else if((PlaceOut>=0)&&(PEk < 0)){
		SpeedIn = LastPlaceOut-dec;
		if(SpeedIn<PlaceOut)  SpeedIn = PlaceOut;
	}else if((PlaceOut<0)&&(PEk < 0)){
		SpeedIn = LastPlaceOut-acc;
		if(SpeedIn<PlaceOut)   SpeedIn = PlaceOut;
	}else if((PlaceOut<=0)&&(PEk > 0)){
		SpeedIn = LastPlaceOut+dec;
		if(SpeedIn>PlaceOut)  SpeedIn = PlaceOut;
	}else{
		SpeedIn = PlaceOut;
	}
	
	return SpeedIn;
}

int T_Aplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec)
{
	int SpeedIn,PEk;
	
	PEk = PlaceOut-LastPlaceOut;
	
	if((PlaceOut>0)&&(PEk > 0))
	{	
		SpeedIn = LastPlaceOut+acc;
		if(SpeedIn>PlaceOut)   SpeedIn = PlaceOut;
	}else if((PlaceOut<0)&&(PEk < 0)){
		SpeedIn = LastPlaceOut-acc;
		if(SpeedIn<PlaceOut)   SpeedIn = PlaceOut;
	}else{
		SpeedIn = PlaceOut;
	}
	
	return SpeedIn;
}

int T_Cplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec)
{
	int SpeedIn,PEk;
	
	PEk = PlaceOut-LastPlaceOut;
	
	if((PlaceOut>0)&&(PEk > 0))
	{	
		SpeedIn = LastPlaceOut+acc;
		if(SpeedIn>PlaceOut)   SpeedIn = PlaceOut;
	}else if((PlaceOut<0)&&(PEk < 0)){
		SpeedIn = LastPlaceOut-acc;
		if(SpeedIn<PlaceOut)   SpeedIn = PlaceOut;
	}else{
		SpeedIn = PlaceOut;
	}
	
	return SpeedIn;
}

int GetDirection(int out)
{
	if(out==0)		return 0;  
	else if(out<0)	return -1;   
	else			return 1; 
}

//如果是反向的信号输入就先停止4MS输出再启动电机
int reversal(int P,u8 TimeCnt)
{
	int Dir_Deviation;                        //输出偏差的方向
	static int Last_Dir_Deviation;            //记录上一次输出偏差的方向,用于比较输出偏差是否改变方向
	static u8 Number_Pwm;
	
	//提取输入电压的方向
    Dir_Deviation = GetDirection(P);
	
	if(Dir_Deviation!=Last_Dir_Deviation) 		 //反向信号
	{
		if(Number_Pwm < TimeCnt)
		{
			Number_Pwm++;
			return 0;
		}else{
			Number_Pwm=0;
			Last_Dir_Deviation=Dir_Deviation;
			return P;
		}
	}else{
		Number_Pwm=0;				
		Last_Dir_Deviation=Dir_Deviation;
		return P;
	}
}
