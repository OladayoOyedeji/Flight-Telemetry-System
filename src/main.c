#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

// System Configuration
uint32_t SystemCoreClock = 16000000;
#define UART0_DR (*((volatile uint32_t *)0x4000C000))

// 1. Mandatory FreeRTOS Declarations
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

// 2. Interrupt Wrappers (Names must match startup.c exactly)
void SVC_Handler(void)     { vPortSVCHandler(); }
void PendSV_Handler(void)  { xPortPendSVHandler(); }
void SysTick_Handler(void) { xPortSysTickHandler(); }

// 3. UART Print Helper
void uart_print(const char *str) {
    while(*str) {
        UART0_DR = (uint32_t)(*str++);
    }
}

// 4. Satellite Logic
void vTelemetryTask(void *pvParameters) {
    for(;;) {
        uart_print("Satellite Heartbeat: [OK]\n");
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

// --- New Sensor Simulation Task ---
void vSensorTask(void *pvParameters) {
    for(;;) {
        // In a real satellite, you'd read an ADC (Analog-to-Digital Converter) here
        uart_print("Sensor Data: Temp=24C, Battery=98%\n");
        
        // This task runs twice as fast as the Heartbeat
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

int main(void) {
    xTaskCreate(vTelemetryTask, "Heart", 128, NULL, 1, NULL);

    // Task 2: Sensors (Priority 1)
    xTaskCreate(vSensorTask, "Sensor", 128, NULL, 1, NULL);
    
    vTaskStartScheduler();
    while(1);
    return 0;
}

// 5. Linker Fixes: Define the missing "vAssertCalled"
void vAssertCalled(const char * pcFile, uint32_t ulLine) {
    uart_print("ASSERT FAIL! File: ");
    uart_print(pcFile);
    while(1);
}

// 6. Required Hooks for FreeRTOS
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) { (void)xTask; (void)pcTaskName; while(1); }
void vApplicationTickHook(void) {}
void vApplicationIdleHook(void) {}
void vApplicationMallocFailedHook(void) { while(1); }

// 7. Dummy Handlers for startup.c (Stellaris specific interrupts)
void Timer0IntHandler(void) { while(1); }
void vT2InterruptHandler(void) { while(1); }
void vT3InterruptHandler(void) { while(1); }
void NmiSR(void) { while(1); }
void FaultISR(void) { uart_print("FAULT!\n"); while(1); }
void IntDefaultHandler(void) { while(1); }

// 8. Static Allocation Hook (If enabled in FreeRTOSConfig.h)
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize ) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
