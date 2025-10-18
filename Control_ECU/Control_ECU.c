/*
 * Control_ECU.c
 *
 *  Created on: Oct 31, 2022
 *      Author: mohaned
 */

#include"buzzer.h"
#include"dc_motor.h"
#include"external_eeprom.h"
#include"std_types.h"
#include"timer1.h"
#include"twi.h"
#include"uart.h"
#include"util/delay.h"
#include"avr/io.h"
/***************************************preprocessors*******************************/

#define FIRST_ADDRESS 0X0000
#define HMI_ready 0X01
#define CONTROL_ready 0X02
#define COMPARE_FALSE 0X03
#define PRINT_OPTIONS 0X04
#define CHECK_PASSWORD_POSITIVE 0X05
#define OPEN_DOOR 0X06
#define CLOSE_DOOR 0X07
#define CHECK_PASSWORD_NEGATIVE 0X08
#define COMPARE_TRUE 0X09

/**************************************typedef***************************************/
typedef enum receive_print_operation
{
	change_pass,start_again
}receive_print_operation;
/****************************functions prototype*************************************/
/*receive first password from HMI then save it in password1[6]  */
void recieving_first_pass(void);

/*receive second password from HMI then save it in password2[6] */
void recieving_second_pass(void);

/*receive options data(+) or (-) sent from HMI
 * then processing on this data and return to main
 * if user wants to change password or open the door only
 *   */
receive_print_operation receive_print_options(void);

/*increment (g_control) to check on this global variable
 * to control printing  the state of the door
 * */
void callback_set_time(void);

/*compare first password and second password then return true or false */
boolean compare(uint8*str1,uint8*str2);

/*********************************global variables*********************************/

uint8 password1[6];
uint8 password2[6];
uint8 i;
Timer1_ConfigType init_3sec_timer1;
uint8 g_control=0;


int main(void)
{
	/********************************initializations*******************************/
	init_3sec_timer1.initial_value=0;
	init_3sec_timer1.mode=CTC;
	init_3sec_timer1.prescaler=CLK_1024; /*for frequency 8MHZ...so tick of timer=1.25msec*/
	init_3sec_timer1.compare_value=2400;  /*when reach 3 seconds go to callback function (ISR)*/
	Timer1_setCallBack(callback_set_time); /*set (callback_set_time()) as a callback of timer  */


	UART_ConfigType init_uart;
	init_uart.bit_data=DATA_8;
	init_uart.baud_rate.mode=DOUBLE_SPEED_MODE;
	init_uart.baud_rate.speed=9600;
	init_uart.parity=DISABLED;
	init_uart.stop_bit=ONE_STOP_BIT;
	UART_init(&init_uart);


	TWI_ConfigType twi_init;
	twi_init.address=0X0B;
	twi_init.bit_rate.prescalar=PRESCALAR_1;
	twi_init.bit_rate.value_of_rate=2;
	TWI_init(&twi_init);


	DcMotor_Init();
	SREG|=(1<<7);

	/****************************************main program logic********************************/
	recieving_first_pass();
	recieving_second_pass();
	while(compare(password1,password2)==FALSE)    /*polling to check 2 passwords are the same*/
	{
		UART_sendByte(CONTROL_ready);/*polling for sending to HMI*/
		while(UART_recieveByte() != (HMI_ready)){}
		UART_sendByte(COMPARE_FALSE);
		recieving_first_pass();

		recieving_second_pass();
	}
	/*now the two passwords are the same so we can change the password*/
	for(i=0;i<5;i++) /*put the password2 in external memory*/
	{
		EEPROM_writeByte((uint16)(FIRST_ADDRESS+i), &password2[i]);
		_delay_ms(20);
	}
	/*here we are sure that right password saved in memory*/
	while(1)
	{
		_delay_ms(2000);
		UART_sendByte(CONTROL_ready);/*polling for sending to HMI to print options*/
		while(UART_recieveByte() != (HMI_ready)){}
		if(receive_print_options()==change_pass)
		{
			recieving_first_pass();
			recieving_second_pass();
			while(compare(password1,password2)==FALSE)    /*polling to check 2 passwords are the same*/
			{
				UART_sendByte(CONTROL_ready);/*polling for sending to HMI*/
				while(UART_recieveByte() != (HMI_ready)){}
				UART_sendByte(COMPARE_FALSE);
				recieving_first_pass();
				_delay_ms(50);
				recieving_second_pass();
			}
			/*now the two passwords are the same so we can change the password*/
			for(i=0;i<5;i++) /*put the password2 in external memory*/
			{
				EEPROM_writeByte((uint16)(FIRST_ADDRESS+i),& password2[i]);
				_delay_ms(20);
			}
		}
		else
		{
			/*if it returns (start_again) do nothing due to while(1) will
			 * call receive_print_options() again
			 *   */

		}





	}



}

/********************************functions implementation*******************************/

void recieving_first_pass(void)
{

	for(i=0;i<5;i++)
	{
		UART_sendByte(CONTROL_ready);
		while(UART_recieveByte() != (HMI_ready)){}
		password1[i]= UART_recieveByte();
	}

}


void recieving_second_pass(void)
{
	for(i=0;i<5;i++)
	{
		UART_sendByte(CONTROL_ready);
		while(UART_recieveByte() != (HMI_ready)){}
		password2[i]= UART_recieveByte();
	}

}



boolean compare(uint8*str1,uint8*str2)
{ uint8 state=TRUE;
for(i=0;i<5;i++)
{
	if(str1[i]!=str2[i])
	{
		state=FALSE;
	}
}
return state;
}


receive_print_operation receive_print_options(void)
{


	for(i=0;i<5;i++) /*get password2 from external memory*/
	{
		EEPROM_readByte(FIRST_ADDRESS+i, &password2[i]);
		_delay_ms(20);
	}

	UART_sendByte(CONTROL_ready);
	while(UART_recieveByte() != (HMI_ready)){}
	if(UART_recieveByte()==CHECK_PASSWORD_POSITIVE)

	{
		for(i=0;i<=3;i++)
		{
			if(i==3) /*if pasword was wrong more than 3 times */
			{
				Buzzer_on();
				_delay_ms(1000);
				Buzzer_off();
				return start_again;
			}
			recieving_first_pass();

			if(compare(password1,password2)==TRUE)/*if input password is right*/
			{
				/*now we are sure that the input password is right*/
				UART_sendByte(CONTROL_ready);/*polling for sending to HMI to compare*/
				while(UART_recieveByte() != (HMI_ready)){}
				UART_sendByte(COMPARE_TRUE);

				UART_sendByte(CONTROL_ready);/*polling for sending to HMI to open the door */
				while(UART_recieveByte() != (HMI_ready)){}
				UART_sendByte(OPEN_DOOR);
				Timer1_init(&init_3sec_timer1); /*start timer*/
				while(g_control<5) /*till the timer counts 15sec */
				{
					DcMotor_Rotate(clockwise,100);
				}
				while(g_control<6) /*till the timer counts 18sec */
				{
					DcMotor_Rotate(no_rotate, 0);
				}
				while(g_control<11) /*till the timer counts 33sec*/
				{
					DcMotor_Rotate(anti_clockwise, 100);
				}

				DcMotor_Rotate(no_rotate, 0);
				return start_again;

			}
			else /*if the password is not right*/
			{

				UART_sendByte(CONTROL_ready);/*polling for sending to HMI to print options*/
				while(UART_recieveByte() != (HMI_ready)){}
				UART_sendByte(COMPARE_FALSE);
				continue;

			}
		}
	}

	else       /*receiving "-" bottom pressed by user from HMI */

	{
		for(i=0;i<=3;i++)
		{
			if(i==3) /*if pasword was wrong more than 3 times */
			{
				Buzzer_on();
				_delay_ms(1000);
				Buzzer_off();
				return start_again;
			}
			recieving_first_pass();

			if(compare(password1,password2)==TRUE)/*if input password is right*/
			{
				return change_pass;

			}
			else /*if the password is not right*/
			{

				UART_sendByte(CONTROL_ready);/*polling for sending to HMI to print options*/
				while(UART_recieveByte() != (HMI_ready)){}
				UART_sendByte(COMPARE_FALSE);
				continue;

			}
		}
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



