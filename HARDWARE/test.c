



手动界面调用
向导传感器校准调用
传感器设置调用
将这三个界面屏幕ID作为事件evt


typedef struct MyStateSwitch
{
	curstate;   //当前状态
	evtID;     	//事件ID
	nextstate; 	//下一个状态
	func();   	//执行功能函数
}
