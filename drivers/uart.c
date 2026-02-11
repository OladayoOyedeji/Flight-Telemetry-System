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

/* // Send a single byte */
/* void send_uart_byte(uint8_t data) */
/* { */
/*     if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdPASS) { */
/*         // Stellaris/QEMU specific: Put char in UART0 */
/*         // Replace with your specific chip's UART put function */
/*         UARTCharPut(UART0_BASE, data);  */
/*         xSemaphoreGive(xUARTMutex); */
/*     } */
/* } */

/* // Send a block of raw memory (like a struct or a checksum) */
/* void send_uart_raw(uint8_t *data, uint16_t length) */
/* { */
/*     if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdPASS) */
/*     { */
/*         for (uint16_t i = 0; i < length; i++) */
/*         { */
/*             UARTCharPut(UART0_BASE, data[i]); */
/*         } */
/*         xSemaphoreGive(xUARTMutex); */
/*     } */
/* } */
