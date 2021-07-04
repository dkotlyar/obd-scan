#include "main.h"
#include "usart_lib.h"
#include "canlib/can_lib.h"

void init(void) {
    usart0_init(9600);
    usart0_println_sync("Start up firmware. Build 1");
    uint8_t can_init_result = can_init(0);
    usart0_print_sync("Can init result: ");
    usart0_println_sync(can_init_result == 0 ? "research of bit timing configuration failed" : "baudrate performed");
}

void loop(void) {

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
