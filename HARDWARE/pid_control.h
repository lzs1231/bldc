#ifndef __PID_CONTROL_H__
#define __PID_CONTROL_H__
#include "sys.h"

void PIDInit(void);


int CenterPID(int PlaceEk,int LimitSpeed);
int SpeedPID(int t_PlaceOut,int MeasuSpeed);
int PlacePID(int PlaceEk,int LimitSpeed);
int SPCPID(int SensorValue,int SPC_Out,int SPCBit);

#endif




