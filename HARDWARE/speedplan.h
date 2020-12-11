#ifndef __SPEEDPLAN_H__
#define __SPEEDPLAN_H__

#include "sys.h"

/***********************获取方向**********************/
int GetDirection(int out);

/***********************速度规划**********************/
int SpeedPlan(int PlaceOut);


//行程校准速度规划
int T_TCplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);






#endif
