#ifndef __USART_H
#define __USART_H

#include "sys.h" 



void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);

void Send1Char(u8 t);					//ʹ�ô���1����һ���ֽ�
void Send1Int(int t);					//ʹ�ô���1����4���ֽ�
void Test_SendData(u8 *buff,u16 len);   //ʹ�ô���1������len���ֽ�

void Send232Char(u8 t);                //ʹ��232����һ���ֽ�
void RS232_SendData(u8 *buff,u8 len);  //ʹ��232����len���ֽ�
void Send485Char(u8 t);                //ʹ��485����һ���ֽ�
void RS485_SendData(u8 *buff,u8 len);  //ʹ��485����len���ֽ�

#endif


