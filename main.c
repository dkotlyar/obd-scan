#include "main.h"
#include "usart_lib.h"

void init(void) {
    usart0_init(9600);
    usart0_print_sync("Hello");
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
