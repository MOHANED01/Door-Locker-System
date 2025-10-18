/*
 * timer1.c
 *
 *  Created on: Oct 31, 2022
 *      Author: mohaned
 */
#include"timer1.h"
#include"avr/io.h"
#include <avr/interrupt.h>
static volatile void (*g_callBackPtr)(void) = NULL_PTR;
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	TCNT1=Config_Ptr->initial_value;    /*initialize timer */
	/*FOC1A:B=11 for non PWM mode
	 * WGM10:1 =00 for normal mode or ctc mode
	 * */
	TCCR1A=(1<<FOC1A)|(1<<FOC1B);
	TCCR1B=(((Config_Ptr->prescaler)&0X07)|((Config_Ptr->mode>>2)<< WGM12));
	if(Config_Ptr->mode==CTC)
	{
		OCR1A=Config_Ptr->compare_value;
	}
	TIMSK|=(1<<OCIE1A); // enable MIE
}

void Timer1_deInit(void)
{
	TCCR1A=0;
	TCCR1B=0; /*no clk*/
	TCNT1=0; /*clear initial value*/
	OCR1A=0;  /*clear compare value*/

}
void Timer1_setCallBack(void(*a_ptr)(void))
{
	g_callBackPtr=a_ptr;
}
