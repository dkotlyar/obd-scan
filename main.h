#ifndef MAIN_H
#define MAIN_H

// COMMON SECTION

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SETBIT(reg, bit, value)     {if (value) {reg |= (1<<(bit));} else {reg &= ~(1<<(bit));}}
#define GETBIT(reg, bit)            ((reg & (1<<bit))>0)
#define MAX(a, b)                   ( (a)>(b) ? (a) : (b) )
#define MIN(a, b)                   ( (a)<(b) ? (a) : (b) )

// PROJECT SECTION

#define CAN_BAUDRATE   250        // in kBit

#define LED_DDR	    DDRE
#define LED_PORT	PORTE
#define LED_Pn		4

#endif