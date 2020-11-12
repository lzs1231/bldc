#ifndef __WDG_H__
#define __WDG_H__


#include "sys.h"


void IWDG_Init(uint8_t prv ,uint16_t rlv);
void IWDG_Feed(void) ;          // Î¹¹·

void Wwdg_Init(u8 tr,u8 wr,u8 fprer);
void Wwdg_Feed(u8 cnt);


#endif


