#include "main.h"
#include "usart_lib.h"

ISR (USART0_RX_vect) {
    uint8_t data = UDR0;
    usart0_send_sync(data);
}

void usart0_init(uint16_t baud) {
    // Инициализация USART0
    UCSR0B = (1<<RXCIE0) | // Разрешить прерывание по приёму данных
             (1<<RXEN0) | // Разрешить приём данных по USART
             (1<<TXEN0) | // Разрешить отправку данных по USART
             (0<<UCSZ02);
    // Формат посылки (8 бит)
    // Проверка паритета отключена
    // 1 стоповый бит
    // 8 бит передаваемых данных
    UCSR0C = (0<<UPM01) | (0<<UPM00) |
             (0<<USBS0) |
             (1<<UCSZ01) | (1<<UCSZ00);
    uint16_t speed = (F_CPU / 16) / baud - 1;
    UBRR0H = (speed >> 8) & 0xFF;
    UBRR0L = speed & 0xFF;
}

void usart0_send_sync(uint8_t data) {
    // ожидаем доступности регистра для записи
    while (!(UCSR0A & (1<<UDRE0))) ;
    UDR0 = data; // записываем данные в регистр
}

void usart0_print_sync(char * str) {
    while (*str != 0) {
        usart0_send_sync(*str);
        str++;
    }
}

void usart0_println_sync(char * str) {
    usart0_print_sync(str);
    usart0_send_sync('\r');
    usart0_send_sync('\n');
}