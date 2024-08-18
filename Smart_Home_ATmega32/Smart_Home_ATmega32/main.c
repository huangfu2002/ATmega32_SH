/*
 * Smart_Home_ATmega32.c
 *
 * Created: 3/10/2023 10:19:05 PM
 * Author : phu20
 */ 
#define F_CPU 8000000UL			/* Define CPU Frequency e.g. here 8MHz */
#include <avr/io.h>				
#include <avr/interrupt.h>		
#include <util/delay.h>			
#include <stdio.h>
#include <string.h>
#include <avr/sfr_defs.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "adc.h"
#include "lcd.h"
#include "keypad.h"
#include "buzzer.h"

#define PASSWORD_LENGTH 4

char password[PASSWORD_LENGTH+1] = "1234"; // default password

int verify_password(char* entered_password);

// display menu
void display_menu(void)
{
	LCD_String_xy(0,0,"Enter pass +");
	LCD_String_xy(1,0,"Change pass -");/* send string data for printing */
}

// Verify the password
int verify_password(char* entered_password)
{
	return (strcmp(entered_password, password) == 0);
}

// Mode for enter password
void enter_password_mode(void)
{
	char entered_password[PASSWORD_LENGTH+1];
	char key[20];
	int i;

	LCD_Clear();
	LCD_String_xy(0,0,"Enter pass:");
	for(i = 0; i < PASSWORD_LENGTH; i++)
	{
		entered_password[i] = keyfind();
		sprintf(key,"%c",entered_password[i]);
		LCD_String_xy(1,i,key);
		_delay_ms(200);
		LCD_String_xy(1, i, "*");
	}
	entered_password[PASSWORD_LENGTH] = '\0';

	if(verify_password(entered_password)) // if password is true
	{
		LCD_Clear();
		LCD_String_xy(0,0,"Door Opened");
		// Control motor to open the door
		PORTC |= (1<<PC1);
		_delay_ms(500);
		LCD_Clear();
		display_menu();
		// close the door
		PORTC &= ~(1<<PC1);
	}
	else //if password is wrong
	{
		LCD_Clear();
		LCD_String_xy(0,0,"Incorrect pass");
		buzzer_alert(); // activate buzzer alert
		LCD_Clear();
		display_menu();
	}
}

// Mode for change password
void change_password_mode(void)
{
	char entered_password[PASSWORD_LENGTH+1];
	char key[20];
	int i;
	LCD_Clear();
	LCD_String_xy(0,0,"Old pass:");
	for(i = 0; i < PASSWORD_LENGTH; i++)
	{
		entered_password[i] = keyfind();
		sprintf(key,"%c",entered_password[i]);
		LCD_String_xy(1,i,key);
		_delay_ms(200);
		LCD_String_xy(1, i, "*");
	}
	entered_password[PASSWORD_LENGTH] = '\0';

	if(verify_password(entered_password)) // if password is true
	{
		LCD_Clear();
		LCD_String_xy(0,0,"New pass:");
		for(i = 0; i < PASSWORD_LENGTH; i++)
		{
			password[i] = keyfind();
			sprintf(key,"%c",password[i]);
			LCD_String_xy(1,i,key);
			_delay_ms(200);
			LCD_String_xy(1, i, "*");
		}
		password[PASSWORD_LENGTH] = '\0';
		LCD_Clear();
		LCD_String_xy(0,0,"Password changed");
		_delay_ms(500);
		LCD_Clear();
		display_menu();
	}
	else // if password is wrong
	{
		LCD_Clear();
		LCD_String_xy(0,0,"Incorrect pass");
		buzzer_alert();
		LCD_Clear();
		display_menu();
	}
}

// Select options
void select_mode (void)
{
	char key = keyfind();
	if(key == '+')
	{
		enter_password_mode();
	}
	else if(key == '-')
	{
		change_password_mode();
	}
}

// Auto fan based on temperature
void read_temp(void)
{
	int lm35_value = readADC(0);
	int temp = (lm35_value / 1023.0) * 500.0;
	if (temp > 30)
	{
		PORTC |= (1<<PC2);
	}
	else
	{
		PORTC &= ~(1<<PC2);
	}
	// TEST IN LCD
	//char temp_str[16];
	//dtostrf(temp, 6, 2, temp_str);
	// Print the LDR and temperature values
	//LCD_String_xy(0,0,"Temp: ");
	//LCD_String_xy(0,6, temp_str);
}

// Auto light based on LDR resistance
void read_ldr(void)
{
	int ldr_value = readADC(1);
	float ldr_resistance = 0;
	ldr_resistance = 10000.0 / ((1023.0 / ldr_value) - 1.0);
	if(ldr_resistance < 2500)
	{
		PORTC |= (1<<PC0);
	}
	else if(ldr_resistance > 2500)
	{
		PORTC &= ~(1<<PC0);
	}
	// TEST IN LCD
	//char ldr_str[16];
	//dtostrf(ldr_resistance, 6, 2, ldr_str);
	//LCD_String_xy(1,0,"LDR Res: ");
	//LCD_String_xy(1,8,ldr_str);
	}
	
// Timer 1 running tasks
ISR(TIMER1_COMPA_vect) {
	read_temp();
	read_ldr();
}

int main()
{
	DDRC = 0xFF;
	LCD_Init();
	LCD_Clear();
	initADC();
	DDRA &= ~((1 << PA0) | (1 << PA1));
	PORTA |= (1 << PA0) | (1 << PA1);
	// Setup timer 1
	TCCR1B |= (1<<CS12)|(1<<CS10); // Split frequency 1024
	TIMSK |= (1<<OCIE1A); // Enable interrupt when the value is reached
	OCR1A = 15625; // Set the counter value to interrupt after 5 seconds
	sei(); // Enable global interrupt
	display_menu();
	while (1)
	{
		select_mode();
	}
}
