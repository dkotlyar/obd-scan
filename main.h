#ifndef MAIN_H
#define MAIN_H

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED_DDR	    DDRE
#define LED_PORT	PORTE
#define LED_Pn		4

#define SETBIT(reg, bit, value) {if (value) {reg |= (1<<(bit));} else {reg &= ~(1<<(bit));}}
#define GETBIT(reg, bit) ((reg & (1<<bit))>0)

#endif