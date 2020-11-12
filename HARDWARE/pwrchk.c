#include "sys.h"
#include "stmflash.h"
#include "pwrchk.h"



//ʹ��PC0��Ϊ�ⲿ�жϽ��е�����
void EXTI_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0; 				//ѡ������0
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING; 	//ѡ������ģʽΪ��������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 		//���Ƶ�����50MHz
	GPIO_Init(GPIOC,&GPIO_InitStructure); 					//����PC.0
}

//�ⲿ�ж�����
void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	EXTI_ClearITPendingBit(EXTI_Line0);    //����жϱ�־

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0);  	//ѡ���жϹܽ�PC.0

	EXTI_InitStructure.EXTI_Line= EXTI_Line0; 						//ѡ���ж���·0
	EXTI_InitStructure.EXTI_Mode= EXTI_Mode_Interrupt; 				//����Ϊ�ж����󣬷��¼�����
	EXTI_InitStructure.EXTI_Trigger= EXTI_Trigger_Rising; 	        //�����жϴ�����ʽΪ�����ش�������ʾϵͳ���磬���ж��б������
	EXTI_InitStructure.EXTI_LineCmd=ENABLE; 						//�ⲿ�ж�ʹ��
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line0)!=RESET)//�ж��Ƿ���λ
	{
		//Dosomething();ִ���ж��¼��������������
		GPIO_SetBits(GPIOD,GPIO_Pin_6);			//����LED��
		
		SavePara();

		EXTI_ClearFlag(EXTI_Line0);			//����LINE�ϱ�־λ
	}
	
}

//void EXTI0_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line0)!=RESET)  //�ж��Ƿ���λ      ���ж��ж��Ƿ�ʹ�ܣ����ж��жϱ�־
//	{
//		Dosomething();                        //ִ���ж��¼�
//		EXTI_ClearITPendingBit(EXTI_Line0)��  //����жϱ�־λ����ֹ���������ж�
//	}
//}









