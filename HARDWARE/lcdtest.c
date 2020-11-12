

enum
{
	CHANGE,             //切换
	WATING,				//等待启动		
	STRAT,				//启动
	CONSTANT,			//匀速
	BRAKE，				//刹车		
}InterfaceState = WATING;		//初始状态为启动

struct  Control
{
	
}Text,Button,Progress


//定义一个界面结构体
typedef struct  Interface
{
	u8 TEXT[4];
	
}InterState;
