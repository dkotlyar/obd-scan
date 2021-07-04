#include "main.h"

int main(void) {
	cli();
	SETBIT(LED_DDR, LED_Pn, 1);
	sei();
	
	while(1) {

	    uint8_t a = 5;
	
		SETBIT(LED_DDR, LED_Pn, 1);
		_delay_ms(100);
		SETBIT(LED_DDR, LED_Pn, 0);
		_delay_ms(500);
		SETBIT(LED_DDR, LED_Pn, 1);
		_delay_ms(100);
		SETBIT(LED_DDR, LED_Pn, 0);
		_delay_ms(3000);
	
	}
}
