/*
 * dc_motor.c
 *
 *  Created on: Oct 4, 2022
 *      Author: mohaned
 */

#ifndef DC_MOTOR_C_
#define DC_MOTOR_C_
#include"dc_motor.h"
#include"pwm_timer0.h"

void DcMotor_Init(void)
{
	GPIO_setupPinDirection(PORT_MOTOR_ID, IN1_MOTOR_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(PORT_MOTOR_ID, IN2_MOTOR_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(PORT_MOTOR_ID, EN_MOTOR, PIN_OUTPUT);
	GPIO_writePin(PORT_MOTOR_ID, IN1_MOTOR_ID, LOGIC_LOW);
	GPIO_writePin(PORT_MOTOR_ID, IN2_MOTOR_ID, LOGIC_LOW);
}
void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	switch (state)
	{
	case no_rotate:
		GPIO_writePin(PORT_MOTOR_ID, IN1_MOTOR_ID, LOGIC_LOW);
			GPIO_writePin(PORT_MOTOR_ID, IN2_MOTOR_ID, LOGIC_LOW);
			PWM_Timer0_Start(0);
			break;
	case clockwise:
		GPIO_writePin(PORT_MOTOR_ID, IN1_MOTOR_ID, LOGIC_HIGH);
			GPIO_writePin(PORT_MOTOR_ID, IN2_MOTOR_ID, LOGIC_LOW);
			PWM_Timer0_Start(((uint16)(speed*255))/100);
			break;
	case anti_clockwise:
		GPIO_writePin(PORT_MOTOR_ID, IN1_MOTOR_ID, LOGIC_LOW);
			GPIO_writePin(PORT_MOTOR_ID, IN2_MOTOR_ID, LOGIC_HIGH);
			PWM_Timer0_Start(((uint16)(speed*255))/100);
			break;
	}


}


#endif /* DC_MOTOR_C_ */
