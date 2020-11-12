#ifndef __BSP_H
#define __BSP_H

#include "sys.h"
#include "includes.h"
#include "delay.h"


//#define	LED_GPIO		GPIOA      //定义LED输出IO口
//#define	LED_PIN		  GPIO_Pin_3   //定义LED输出引脚位

//#define	LEDH_GPIO	  GPIOE     
//#define	LEDH_PIN	  GPIO_Pin_5 

#define	LEDL_GPIO	  GPIOB      
#define	LEDL_PIN	  GPIO_Pin_12

#define	LEDH_GPIO	  GPIOD     
#define	LEDH_PIN	  GPIO_Pin_2


#define	LKEY_GPIO	  GPIOC     
#define	LKEY1_PIN	  GPIO_Pin_12
#define	LKEY2_PIN	  GPIO_Pin_13
#define	LKEY3_PIN	  GPIO_Pin_14
#define	LKEY4_PIN	  GPIO_Pin_15

void BSP_Init(void);
void Interrupt_Config(void);     //中断优先级配置

#endif
