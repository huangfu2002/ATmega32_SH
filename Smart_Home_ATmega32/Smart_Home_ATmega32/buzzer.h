/*
 * buzzer.h
 *
 * Created: 3/31/2023 11:39:10 PM
 *  Author: phu20
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_

void buzzer_alert(void)
{
	PORTC |= (1<<PC3); // set PC1 to 1 to turn on the buzzer
	_delay_ms(200); // delay for 200ms
	PORTC &= ~(1<<PC3); // set PC1 to 0 to turn off the buzzer
	_delay_ms(200); // delay for 200ms
}

#endif /* BUZZER_H_ */