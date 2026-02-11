#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init();
void uart_print(const char *str);
char uart_getc(void);
void send_uart_byte(uint8_t data);
void send_uart_raw(uint8_t *data, uint16_t length);

#endif
