#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

// Task Prototypes (Defined in app/ files)
extern void vTelemetryTask(void *pv);
extern void vSensorTask(void *pv);
extern void vAlarmTask(void *pv);

int main(void) {
    uart_init(); // Layer 1
    
    // Layer 3 Launches
    xTaskCreate(vTelemetryTask, "Heart", 128, NULL, 1, NULL);
    xTaskCreate(vSensorTask,    "Sens",  128, NULL, 1, NULL);
    xTaskCreate(vAlarmTask,     "Alarm", 128, NULL, 2, NULL);

    vTaskStartScheduler(); // Layer 2 starts the mission
    return 0;
}
