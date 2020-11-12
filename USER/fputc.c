#include <stdio.h>
#include "usart.h"	 
 
 
/**--------------jlink打印需要此段函数，串口打印需要注释掉--------------**/
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000
 
struct __FILE { int handle; /* Add whatever you need here */ };
    FILE __stdout;
    FILE __stdin;
    
int fputc(int ch, FILE *f) 
{
	if (DEMCR & TRCENA) 
	{
		while (ITM_Port32(0) == 0);
		ITM_Port8(0) = ch;
	}
	return(ch);
}


/**--------------串口打印需要此段函数，jlink打印需要注释掉--------------**/

//使用之前需要先配置串口时钟、初始化串口

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#if 1
//#pragma import(__use_no_semihosting)             
////标准库需要的支持函数                 
//struct __FILE 
//{ 
//	int handle; 
//}; 
//FILE __stdout;       
////定义_sys_exit()以避免使用半主机模式    
//_sys_exit(int x) 
//{ 
//	x = x; 
//} 
////重定义fputc函数 
//int fputc(int ch, FILE *f)
//{      
//	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
//  USART1->DR = (u8) ch;      
//	return ch;
//}
//#endif 




//加入以下代码,支持printf函数,需要选择use MicroLIB
//int fputc(int ch, FILE *f)
//{

// /* 循环直到发送完成 */
// while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

// /* 给USART写一个字符 */
// USART_SendData(USART1, (u8) ch);
// 
// return ch;
//}


