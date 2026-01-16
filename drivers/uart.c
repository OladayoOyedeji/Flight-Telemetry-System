#include "uart.h"
#define UART0_DR (*((volatile uint32_t *)0x4000C000))
#define UART0_FR (*((volatile uint32_t *)0x4000C018))
#define UART_FR_RXFE (1 << 4) // Receive FIFO empty flag

void uart_init() {
    // Hardware-specific initialization logic
}

void uart_print(const char *str) {
    while(*str) UART0_DR = (uint32_t)(*str++);
}

// Helper to read one character (Non-blocking)
char uart_getc(void) {
    if (!(UART0_FR & UART_FR_RXFE)) { // If NOT empty
        return (char)(UART0_DR & 0xFF);
    }
    return 0;
}
