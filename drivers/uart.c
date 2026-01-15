#include "uart.h"
#define UART0_DR (*((volatile uint32_t *)0x4000C000))

void uart_init() {
    // Hardware-specific initialization logic
}

void uart_print(const char *str) {
    while(*str) UART0_DR = (uint32_t)(*str++);
}
