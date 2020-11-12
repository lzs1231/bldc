#ifndef __USART_H
#define __USART_H

#include "sys.h" 



void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);

void Send1Char(u8 t);					//使用串口1发送一个字节
void Send1Int(int t);					//使用串口1发送4个字节
void Test_SendData(u8 *buff,u16 len);   //使用串口1·发送len个字节

void Send232Char(u8 t);                //使用232发送一个字节
void RS232_SendData(u8 *buff,u8 len);  //使用232发送len个字节
void Send485Char(u8 t);                //使用485发送一个字节
void RS485_SendData(u8 *buff,u8 len);  //使用485发送len个字节

#endif


