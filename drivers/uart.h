#ifndef UART_H
#define UART_H

#include <stdint.h> // <--- ADD THIS LINE

void uart_init();
void uart_print(const char *str);
char uart_getc(void);

#endif
