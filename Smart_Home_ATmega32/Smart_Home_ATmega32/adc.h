/*
 * adc.h
 *
 * Created: 4/4/2023 12:01:57 AM
 *  Author: phu20
 */ 

#ifndef ADC_H_
#define ADC_H_
#define degree_sysmbol 0xdf

void initADC()
{
	// Set the reference voltage to AVcc
	ADMUX |= (1 << REFS0);
	// Set the ADC prescaler to 128
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// Enable the ADC module
	ADCSRA |= (1 << ADEN);
}

int readADC(int channel)
{
	// Set the channel to read
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	_delay_us(10); // Add a short delay here
	// Start the conversion
	ADCSRA |= (1 << ADSC);
	// Wait for the conversion to complete
	while (ADCSRA & (1 << ADSC));
	// Read the ADC value
	uint16_t value = ADCL | (ADCH << 8);
	return value;
}


#endif /* ADC_H_ */