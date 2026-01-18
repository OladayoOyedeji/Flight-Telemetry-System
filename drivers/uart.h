#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init();
void uart_print(const char *str);
char uart_getc(void);

#endif
