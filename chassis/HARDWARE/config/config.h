#ifndef __CONFIG_H
#define __CONFIG_H


#include "hardware.h"


#define MOYU

/*�û�������*/

#ifdef MOYU
#define SERVO_NUMBER				12		//�����
#define SERVO_NUMBER_WITH_ARM				15		//�����
#endif


#ifdef SHEN_ME_GUI
#define ROBOT_ID					0x06	//�����˱��(�ӻ���)
#else
#define ROBOT_ID					0x05	//�����˱��(�ӻ���)
#endif

extern const u8 SERVO_NAME[SERVO_NUMBER][100];
//extern u16 Pos_down[11][6];
//extern u16 Pos_up[11][6];
extern u16 InitialPosition[SERVO_NUMBER];
extern u16 shouqi[SERVO_NUMBER];
extern u16 fangxia[SERVO_NUMBER];
extern u16 Pos_go_ahead[11][12];
extern u16 Pos_turn_left[11][12];
extern u16 Pos_turn_right[11][12];
extern u16 InitialPosition_with_arm[SERVO_NUMBER_WITH_ARM];
#endif
