
#include "telemetry_tasks.h"

int main(void) {
    xUartMutex = xSemaphoreCreateMutex();
    if (xUartMutex == NULL) {
        // Handle error: not enough heap memory
        while(1);
    }
    xTelemetryQueue = xQueueCreate(5, sizeof(TelemetryData_t));
    
    xTaskCreate(vTelemetryTask,   "Heart",    512, NULL, 1, NULL);
    xTaskCreate(vSensorTask,      "Sensor",   128, NULL, 1, NULL);
    xTaskCreate(vCommandTask,     "Cmd",      128, NULL, 2, NULL);
    xTaskCreate(vWatchdogTask,    "Watchdog", 128, NULL, 3, &xWatchdogTaskHandle);
    xTaskCreate(vEnvironmentTask, "Env",      128, NULL, 1, NULL);
    
    vTaskStartScheduler();
    while(1);
    return 0;
}

void vAssertCalled(const char * pcFile, uint32_t ulLine) {
    safe_uart_print("ASSERT FAIL! File: ");
    safe_uart_print(pcFile);
    while(1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) { (void)xTask; (void)pcTaskName; while(1); }
void vApplicationTickHook(void) {}
void vApplicationIdleHook(void) {}
void vApplicationMallocFailedHook(void) { while(1); }

void Timer0IntHandler(void) { while(1); }
void vT2InterruptHandler(void) { while(1); }
void vT3InterruptHandler(void) { while(1); }
void NmiSR(void) { while(1); }
void FaultISR(void) { safe_uart_print("FAULT!\n"); while(1); }
void IntDefaultHandler(void) { while(1); }

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize ) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
