#include "FreeRTOS.h"
#include "task.h"

void vHeartbeatTask(void *pvParameters) {
    for(;;) {
        // This is where your telemetry logic will eventually go
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

int main(void) {
    xTaskCreate(vHeartbeatTask, "Heartbeat", 100, NULL, 1, NULL);
    vTaskStartScheduler();
    for(;;);
}
