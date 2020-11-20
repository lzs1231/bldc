#ifndef __HANDLE_H
#define __HANDLE_H

#include "sys.h"

/***********************��ȡ����**********************/
int GetDirection(int out);

/*********************���ϼ�����**********************/
int No_Material_Detection(int P);

/**********************�������***************************/
int PlaceOutHandle(u16 sensor1,u16 sensor2);

/*********************��λ�����ӳ���****************************/
int LimitProcessing(int P);

/**********************spc����******************************/
int SPC_Control(int V1,int V2);

/*******************���Ĺ�λ�ӳ���***********************/
int Cent_Control(int speed);

/***************�г�����ӦУ׼*******************/
int TravelCalibration(void);

/*************��������*******************/
int CurrentProtection(int P,u16 I,u16 U);

/**********************����Ӽ��ٿ���*************************/
int AccDecSpeed(int placeout);

/************�����⣬�ⲿ�����Դ����20V�����28V����ֹͣ������***********/
int V_stop(int PWM,int V);

#endif


