#ifndef USART_LIB_H
#define USART_LIB_H

typedef struct {

} usart_t;

void usart0_init(uint16_t baud);
void usart0_send_sync(uint8_t data);
void usart0_print_sync(char * str);
void usart0_println_sync(char * str);

#endif
