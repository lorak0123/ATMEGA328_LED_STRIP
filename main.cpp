/*
 * ATMEGA328_LED_STRIP.cpp
 *
 * Created: 21.12.2018 20:03:11
 * Author : Karol Pilot
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>


#define RED OCR1B
#define GREEN OCR1A
#define BLUE OCR2A
#define SIGNAL 0

#define DELAY 10
#define TAB_SIZE 20


bool r = true;
bool g = false;
bool b = true;
uint8_t color_p = 0;
uint8_t cycle = 0;
uint16_t tab[TAB_SIZE] = {11};
uint8_t index = 0;


void pwm_init();
void pulse();
uint16_t adc_read(uint8_t adcx);
void show(uint8_t x);
void change_color();
uint16_t get_max(uint8_t x = TAB_SIZE);
void set_max(uint16_t x);



int main(void)
{
	pwm_init();
	
	
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
    while (true) 
    {
		uint16_t x = 0;
		
		for(short i = 0; i < 500; i++)
		{
			uint16_t probe = adc_read(0);
			
			if(probe > x) x = probe;
		}
		
		set_max(x);
		
		
		uint16_t out = 255.0*x/get_max(5);
		
		
		if(out < 105)
		{
			out = 0;
			show(out);
		}
		else 
		{
			if(out > 255) out = 255;
			else out = (out-105)*1.7;
			show(out);
		}
		
		
		while(get_max(20) < 5)
		{
			pulse();
			
			for(short i = 0; i < 2000; i++)
			{
				uint16_t probe = adc_read(0);
				
				if(probe > 0) { set_max(probe); break;}
			}
		}
    }
}


void set_max(uint16_t x)
{
	
	if(tab[index] < x) tab[index] = x;
	
	if(cycle > 15)
	{
		if(index == TAB_SIZE) index = 0;
		else index++;
		
		tab[index] = 0;
		cycle = 0;
	}
	
	cycle++;
}


uint16_t get_max(uint8_t x)
{
	uint16_t res = 0;
	
	for(int8_t i = -1; i < x; i++)
	{
		if(index + i < TAB_SIZE)
		{
			if(res < tab[index+i]) res = tab[index+i];
		}
		else
		{
			if(res < tab[index+i-TAB_SIZE]) res = tab[index+i-TAB_SIZE];
		}
	}
	
	return res;
}


void show(uint8_t x)
{
	if(x==0 && color_p > 7)
	{
		change_color();
		color_p = 0;
	}
	color_p++;

	if(r) RED = x;
	if(g) GREEN = x;
	if(b) BLUE = x;
}

void change_color()
{
	RED = 0x00; r = false;
	GREEN = 0x00; g = false;
	BLUE = 0x00; b = false;
	
	
	
	while(!r && !g && !b)
	{
		if(rand()%2) r = true;
		if(rand()%2) g = true;
		if(rand()%2) b = true;
	}
}


void pwm_init() 
{
	
	TCCR1A |= (1<<WGM10);
	TCCR2A |= (1<<WGM20);
	
	TCCR1B |= (1<<CS11);
	TCCR2B |= (1<<CS21);
	
	OCR1A = 0x00;
	OCR1B = 0x00;
	OCR2A = 0x00;
	
	DDRB |= 0b00001110;
	
	
	TCCR1A |= 1<<COM1A1;
	TCCR1A |= 1<<COM1B1;
	TCCR2A |= 1<<COM2A1;
	
}


void pulse()
{

	for(uint8_t i = 0; i < 255; i++)
	{
		GREEN = i;
		_delay_ms(DELAY);
	}
	
	
	for(uint8_t i = 255; i > 0; i--)
	{
		RED = i;
		_delay_ms(DELAY);
	}
	
	for(uint8_t i = 0; i < 255; i++)
	{
		BLUE = i;
		_delay_ms(DELAY);
	}
	
	
	for(uint8_t i = 255; i > 0; i--)
	{
		GREEN = i;
		_delay_ms(DELAY);
	}
	
	
	for(uint8_t i = 0; i < 255; i++)
	{
		RED = i;
		_delay_ms(DELAY);
	}
	
	for(uint8_t i = 255; i > 0; i--)
	{
		BLUE = i;
		_delay_ms(DELAY);
	}
		
}

uint16_t adc_read(uint8_t x)
{
	ADMUX &= 0xf0;
	
	ADMUX = (ADMUX & 0xF8)|x;

	ADCSRA |= (1<<ADSC);

	while(ADCSRA & (1<<ADSC));
	
	return ADC;
	
}

