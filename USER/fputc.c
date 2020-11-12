#include <stdio.h>
#include "usart.h"	 
 
 
/**--------------jlink��ӡ��Ҫ�˶κ��������ڴ�ӡ��Ҫע�͵�--------------**/
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


/**--------------���ڴ�ӡ��Ҫ�˶κ�����jlink��ӡ��Ҫע�͵�--------------**/

//ʹ��֮ǰ��Ҫ�����ô���ʱ�ӡ���ʼ������

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
//#if 1
//#pragma import(__use_no_semihosting)             
////��׼����Ҫ��֧�ֺ���                 
//struct __FILE 
//{ 
//	int handle; 
//}; 
//FILE __stdout;       
////����_sys_exit()�Ա���ʹ�ð�����ģʽ    
//_sys_exit(int x) 
//{ 
//	x = x; 
//} 
////�ض���fputc���� 
//int fputc(int ch, FILE *f)
//{      
//	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
//  USART1->DR = (u8) ch;      
//	return ch;
//}
//#endif 




//�������´���,֧��printf����,��Ҫѡ��use MicroLIB
//int fputc(int ch, FILE *f)
//{

// /* ѭ��ֱ��������� */
// while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

// /* ��USARTдһ���ַ� */
// USART_SendData(USART1, (u8) ch);
// 
// return ch;
//}


