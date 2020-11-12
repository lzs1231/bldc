#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 



void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
void Send232Char(u8 t);

void Send485Char(u8 t);
void RS485_SendData(u8 *buff,u8 len);

void Send1Char(u8 t);
void Send1Int(int t);
void Test_SendData(u8 *buff,u16 len);

#endif


