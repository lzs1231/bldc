#ifndef __SPEEDPLAN_H__
#define __SPEEDPLAN_H__

#include "sys.h"

/***********************��ȡ����**********************/
int GetDirection(int out);

/***********************�ٶȹ滮**********************/
int SpeedPlan(int PlaceOut);


//�г�У׼�ٶȹ滮
int T_TCplan(int PlaceOut,int LastPlaceOut,u16 acc,u16 dec);






#endif
