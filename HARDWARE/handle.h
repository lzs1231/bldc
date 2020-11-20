#ifndef __HANDLE_H
#define __HANDLE_H

#include "sys.h"

/***********************获取方向**********************/
int GetDirection(int out);

/*********************无料检测程序**********************/
int No_Material_Detection(int P);

/**********************输出处理***************************/
int PlaceOutHandle(u16 sensor1,u16 sensor2);

/*********************限位处理子程序****************************/
int LimitProcessing(int P);

/**********************spc控制******************************/
int SPC_Control(int V1,int V2);

/*******************中心归位子程序***********************/
int Cent_Control(int speed);

/***************行程自适应校准*******************/
int TravelCalibration(void);

/*************电流保护*******************/
int CurrentProtection(int P,u16 I,u16 U);

/**********************电机加减速控制*************************/
int AccDecSpeed(int placeout);

/************掉电检测，外部输入电源低于20V或高于28V，就停止电机输出***********/
int V_stop(int PWM,int V);

#endif


