/*
 * HMI_ECU.c
 *
 *  Created on: Oct 30, 2022
 *      Author: mohaned
 */
#include"keypad.h"
#include"common_macros.h"
#include"lcd.h"
#include"timer1.h"
#include"uart.h"
#include"util/delay.h"
#include"avr/io.h"
#include"std_types.h"
#include "uart.h"
/************************************preprocessors******************************/
#define FIRST_ADDRESS 0X00
#define HMI_ready 0X01
#define CONTROL_ready 0X02
#define COMPARE_FALSE 0X03
#define PRINT_OPTIONS 0X04
#define CHECK_PASSWORD_POSITIVE 0X05
#define OPEN_DOOR 0X06
#define CLOSE_DOOR 0X07
#define CHECK_PASSWORD_NEGATIVE 0X08
#define COMPARE_TRUE 0X09
/*************************************typedefs**********************************/
typedef enum print_operation
{
	change_pass,start_again
}print_operation;
/*********************************function prototypes*************************/
/*ask first time for password ...after receiving all elements
 * in string ...pass it to control
 * */
void enter_and_send_pass(void);

/*ask second time for password ...after receiving all elements
 * in string ...pass it to control ..then polling to check that the two passwords
 * are the same to call (print_options_and_check)
 * */

void re_enter_and_send_pass(void);

/*this function prints options and check from control if the data wrong
 * or right .... it returns (change_pass) if data right
 *  and returns (start_again) if password wrong for 3 times
 *  */

print_operation print_options_and_check(void);

/*increment (g_control) to check on this global variable
 * to control printing  the state of the door
 * */

void callback_set_time(void);
/************************************global variables**********************************/
uint8 password1[6];
uint8 password2[6];
uint8 i;
uint8 g_control=0;
Timer1_ConfigType init_3sec_timer1;

int main(void)
{
	/***********************************initializations*********************************/

	init_3sec_timer1.initial_value=0;
	init_3sec_timer1.mode=CTC;
	init_3sec_timer1.prescaler=CLK_1024; /*for frequency 1MHZ...so tick of timer=1msec*/
	init_3sec_timer1.compare_value=3000;  /*when reach 3 seconds go to callback function (ISR)*/
	Timer1_setCallBack(callback_set_time); /*set (callback_set_time()) as a callback of timer  */

	UART_ConfigType init_uart;
	init_uart.bit_data=DATA_8;
	init_uart.baud_rate.mode=DOUBLE_SPEED_MODE;
	init_uart.baud_rate.speed=9600;
	init_uart.parity=DISABLED;
	init_uart.stop_bit=ONE_STOP_BIT;
	UART_init(&init_uart);
	LCD_init();
	SREG|=(1<<7);

	/**********************************main program logic*********************************/

	enter_and_send_pass();
	re_enter_and_send_pass();

	while(UART_recieveByte() != (CONTROL_ready)){} /*polling to receive from control */
	UART_sendByte(HMI_ready);
	while(UART_recieveByte()==COMPARE_FALSE) /*polling to check the 2 passwords are the same*/
	{
		LCD_clearScreen();
		enter_and_send_pass();
		re_enter_and_send_pass();
		while(UART_recieveByte() != (CONTROL_ready)){} /*polling to receive from control */
		UART_sendByte(HMI_ready);
	}
	/*till here we sure that the 2 passwords entered by the user are the same*/
	while(1)
	{
		LCD_clearScreen();
		while(UART_recieveByte() != (CONTROL_ready)){} /*polling to receive from control */
		while(UART_recieveByte()!=PRINT_OPTIONS){}
		if(print_options_and_check()==change_pass)/*if user wants to change pass */
		{
			enter_and_send_pass();
			re_enter_and_send_pass();

			while(UART_recieveByte() != (CONTROL_ready)){} /*polling to receive from control */
			UART_sendByte(HMI_ready);
			while(UART_recieveByte()==COMPARE_FALSE) /*polling to check the 2 passwords are the same*/
			{
				LCD_clearScreen();
				enter_and_send_pass();
				_delay_ms(500);
				re_enter_and_send_pass();
			}
		}
		else
		{
			/*if it returns (start_again) do nothing due to while(1) will
			 * call print_options_and_check() again
			 *   */

		}
	}

}

/********************************functions implementation**********************************/
void enter_and_send_pass(void)
{
	uint8 i=0,reading;
	LCD_clearScreen();
	LCD_displayString("plz enter pass:");
	while(KEYPAD_getPressedKey()!='=') /*get first pass from user */
	{
		reading=KEYPAD_getPressedKey();
		password1[i]=reading;
		_delay_ms(300);
		LCD_moveCursor(1, i);
		LCD_displayCharacter('*');
		i++;
	}

	LCD_clearScreen();
	/*polling to send string to control */
	for(i=0;i<5;i++)
	{
		while(UART_recieveByte() != (CONTROL_ready)){}
		UART_sendByte(HMI_ready);
		UART_sendByte(password1[i]);
	}

}



void re_enter_and_send_pass(void)
{
	uint8 i=0,reading;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"plz re-enter the ");
	LCD_displayStringRowColumn(1,0,"same pass:");
	while(KEYPAD_getPressedKey()!='=') /*get first pass from user */
	{
		reading=KEYPAD_getPressedKey();
		password2[i]=reading;
		_delay_ms(300);
		LCD_displayCharacter('*');
		i++;
	}
	for(i=0;i<5;i++)
	{
		while(UART_recieveByte() != (CONTROL_ready)){}
		UART_sendByte(HMI_ready);

		UART_sendByte(password2[i]);
	}
}

print_operation print_options_and_check(void)
{

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"+ : open door");
	LCD_displayStringRowColumn(1,0,"- : change pass");
	if(KEYPAD_getPressedKey()=='+')
	{

		while(UART_recieveByte() != (CONTROL_ready)){}
		UART_sendByte(HMI_ready);
		UART_sendByte(CHECK_PASSWORD_POSITIVE); /*send to control to check the password*/
		for(i=0;i<=3;i++) /*ask user 3 times to enter the password*/
		{
			if(i==3) /*in case of 3 times wrong password*/
			{
				LCD_clearScreen();
				LCD_displayString("ERROR");
				_delay_ms(1000);
				return start_again;


			}
			enter_and_send_pass();
			while(UART_recieveByte() != (CONTROL_ready)){} /*polling to receive from control */
			UART_sendByte(HMI_ready);
			if(UART_recieveByte()!=COMPARE_FALSE) /*if false continue for asking */
			{
				break;  /*if true break the loop*/
			}
		}

		{
			while(UART_recieveByte() != (CONTROL_ready)){} /*polling to receive from control */
			UART_sendByte(HMI_ready);
			if(UART_recieveByte()==OPEN_DOOR)
			{
				LCD_clearScreen();
				Timer1_init(&init_3sec_timer1); /*start timer*/
				while(g_control<5) /*till the timer counts 15sec */
				{
					LCD_displayString("DOOR IS UNLOCKING");
				}
				while(g_control<6) /*till the timer counts 18sec */
				{
					LCD_clearScreen();
					LCD_displayString("DOOR IS HOLD");
				}
				while(g_control<11) /*till the timer counts 33sec*/
				{
					LCD_clearScreen();
					LCD_displayString("DOOR IS LOCKING");
				}

			}
		}
	}
	else  /*in case of user wants to change password*/
	{
		while(UART_recieveByte() != (CONTROL_ready)){}
		UART_sendByte(HMI_ready);
		UART_sendByte(CHECK_PASSWORD_NEGATIVE); /*send to control to check the password*/
		for(i=0;i<=3;i++) /*ask user 3 times to enter the password*/
		{
			if(i==3) /*in case of 3 times wrong password*/
			{
				LCD_clearScreen();
				LCD_displayString("ERROR");
				_delay_ms(1000);
				return start_again;
			}
			enter_and_send_pass();


			while(UART_recieveByte() != (CONTROL_ready)){} /*polling to receive from control */
			UART_sendByte(HMI_ready);
			if(UART_recieveByte()!=COMPARE_FALSE) /*if false continue for asking */
			{
				break;                           /*if true break the loop*/
			}
		}
		return change_pass;



	}
}
void callback_set_time(void)
{
	g_control++;
	if(g_control>11)   /*each interrupt increase (g_control) by 1 that indicates 3 sec delayed
	 *we want (15+3+15) so we want max value of (g_control) =11 then initialize it again
	 *to zero
	 */
	{
		g_control=0;
	}
}
