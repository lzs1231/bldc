#ifndef __CRTL_H
#define __CRTL_H	 
#include "sys.h"
#include "main.h"

void BLDC_GPIOConfig(void);
void BLDC_TIM1Config(void);
void BLDC_TIM3Config(void);
void BLDC_TIM4Config(void);
MOTOR_DIR huanxiang(void);      //»»Ïà
void BLDC_Start(void);     //Æô¶¯
void BLDC_Stop(void);      //Í£Ö¹
void PwmOut(int SpeedOut);     //Êä³ö			    
#endif

