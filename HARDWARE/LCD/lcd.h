#ifndef __LCD_H
#define __LCD_H

#include "sys.h"
#include "ctrl.h"
//#define	LED_GPIO		GPIOA      //����LED���IO��
//#define	LED_PIN		  GPIO_Pin_3   //����LED�������λ

#define	LEDH_GPIO	  GPIOA      
#define	LEDH_PIN	  GPIO_Pin_8 

#define	LEDL_GPIO	  GPIOD      
#define	LEDL_PIN	  GPIO_Pin_2

void BSP_Init(void);

#endif
