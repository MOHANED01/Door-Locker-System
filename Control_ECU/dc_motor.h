/*
 * dc_motor.h
 *
 *  Created on: Oct 4, 2022
 *      Author: mohaned
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_
#include"std_types.h"
#include"gpio.h"
#define PORT_MOTOR_ID PORTB_ID
#define IN1_MOTOR_ID PIN4_ID
#define IN2_MOTOR_ID PIN5_ID
#define EN_MOTOR PIN3_ID
typedef enum
{
	no_rotate,clockwise,anti_clockwise
}DcMotor_State;
void DcMotor_Init(void);
void DcMotor_Rotate(DcMotor_State state,uint8 speed);
#endif /* DC_MOTOR_H_ */
