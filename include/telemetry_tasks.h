#ifndef TELEMETRY_TASKS_H
#define TELEMETRY_TASKS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "uart.h"

// The struct definition goes here
typedef struct {
    int32_t temperature;    // Signed to handle negative temps
    uint32_t battery_level; 
    uint32_t timestamp;     
} TelemetryData_t;

// --- Global Variable Declarations ---
extern uint32_t sensor_checkin;
extern uint32_t telemetry_checkin;
extern volatile uint8_t sensor_fail_trigger;
extern SemaphoreHandle_t xUartMutex;
extern QueueHandle_t xTelemetryQueue;
extern TaskHandle_t xWatchdogTaskHandle;

extern volatile int32_t g_battery_level; 
extern volatile int32_t g_temperature;
extern volatile bool g_in_sunlight;
extern volatile bool g_heater_on;

// --- Function Prototypes ---
void safe_uart_print(const char *pcString);
void vTelemetryTask(void *pvParameters);
void vSensorTask(void *pvParameters);
void vWatchdogTask(void *pvParameters);
void vCommandTask(void *pvParameters);
void vBatteryTask(void *pvParameters);
void vEnvironmentTask(void *pvParameters);

#endif
