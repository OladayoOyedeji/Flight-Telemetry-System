#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

void vSensorTask(void *pvParameters) {
    for(;;) {
        uart_print("Sensor Scan: OK\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
