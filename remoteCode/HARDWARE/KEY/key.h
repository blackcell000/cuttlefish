#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define WK_UP PAin(0)	//PA0  WK_UP

#define KEYFront  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//ǰ��
#define KEYBack  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//����
#define KEYLeft  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//��
#define KEYRight  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)//��
#define KEY4  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)//
#define KEY5  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)//
#define KEY6  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)//


 

#define KEYFront_PRES 	1	//
#define KEYBack_PRES	  2	//
#define KEYLeft_PRES   3
#define KEYRight_PRES  4
#define KEY4_PRES       5
#define KEY5_PRES       6
#define KEY6_PRES       7
#define doubleClick 8
#define longPress 9
#define singleClick 10


void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��				
u8 click_KEYBack (u8 time);
u8 click_KEYFront (u8 time);


#endif
