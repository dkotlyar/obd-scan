#include "main.h"
#include "usart_lib.h"
// Library from example "CAN ATmega32M1" http://www.hpinfotech.ro/cvavr-examples.html | https://forum.digikey.com/t/can-example-atmega32m1-stk600/13039
// but original Atmel Library does not accessible by link http://www.atmel.com/tools/cansoftwarelibrary.aspx
#include "canlib/can_lib.h"
#include "obd2.h"

#define blink() {LED_ON();_delay_ms(50);LED_OFF();_delay_ms(50);}
#define long_blink() {LED_ON();_delay_ms(400);LED_OFF();_delay_ms(250);}

uint32_t _millis = 0;
uint32_t get_millis(void) {
    return _millis;
}

void init(void) {
    usart0_init(9600);
    usart0_println_sync("Start up firmware. Build 3");
    uint8_t can_init_result = can_init(0);
    usart0_print_sync("Can init result: ");
    usart0_println_sync(can_init_result == 0 ? "research of bit timing configuration failed" : "baudrate performed");

    obd2_init();

    SETBIT_1(LED_DDR, LED_Pn);
    LED_OFF();
}

void loop(void) {
    obd2_loop();
}

int main(void) {
	cli();
	init();
	sei();
	
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
	while(1) {
	    loop();
	}
#pragma clang diagnostic pop
}
