// File: telemetry_tasks.c
#include "telemetry_tasks.h"

// Actual memory allocation happens here
uint32_t sensor_checkin = 0;
uint32_t telemetry_checkin = 0;
volatile uint8_t sensor_fail_trigger = 0;
SemaphoreHandle_t xUartMutex = NULL;
QueueHandle_t xTelemetryQueue = NULL;
TaskHandle_t xWatchdogTaskHandle = NULL;

volatile int32_t g_battery_level = 100; 
volatile int32_t g_temperature = 25;
volatile bool g_in_sunlight = true;
volatile bool g_heater_on = false;

void safe_uart_print(const char *pcString) {
    // Wait forever for the Mutex to become available
    if (xSemaphoreTake(xUartMutex, portMAX_DELAY) == pdPASS) {
        uart_print(pcString);
        
        // Give the Mutex back so other tasks can print
        xSemaphoreGive(xUartMutex);
    }
}

// 4. Satellite Logic
void vTelemetryTask(void *pvParameters) {
    TelemetryData_t receivedData;
    char buffer[128]; // Temporary space to build our string
    
    for(;;) {
        // This task "blocks" (sleeps) until the sensor task sends something
        if(xQueueReceive(xTelemetryQueue, &receivedData, portMAX_DELAY))
        {
            safe_uart_print("\n--- Incoming Telemetry ---");

            // Use sprintf to format the sensor data into our buffer
            // %d is a placeholder for integers
            snprintf(buffer, sizeof(buffer), "\nTimestamp: [DATA]\nTemp: %d C\nBattery: %d%%\n", 
                    (int)receivedData.temperature, 
                    (int)receivedData.battery_level);
            
            safe_uart_print(buffer);
            // Logic Check: Did we get what we expected?
            if (receivedData.battery_level <= 20)
            {
                safe_uart_print("Status: WARNING - LOW POWER\n");
            } 
            else if (receivedData.temperature > 70)
            {
                safe_uart_print("Status: CRITICAL - OVERHEATING\n");
            } 
            else if (receivedData.temperature < -10)
            {
                safe_uart_print("Status: WARNING - THERMAL STRESS (COLD)\n");
            } 
            else
            {
                safe_uart_print("Status: NOMINAL\n");
            }
        }
        telemetry_checkin = 1; // I'm alive!
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// --- New Sensor Simulation Task ---
void vSensorTask(void *pvParameters) {
    TelemetryData_t sensorUpdate;
    for(;;) {
        if (sensor_fail_trigger) {
            safe_uart_print("Sensor Task: I'm stuck in an infinite loop!\n");
            while(1); // Task is now dead to the scheduler
        }
        // Simulate "Reading" sensors
        if (xTelemetryQueue != NULL) {
            sensorUpdate.temperature = g_temperature;
            sensorUpdate.battery_level = g_battery_level;
            sensorUpdate.timestamp = xTaskGetTickCount();

            // Send data to the queue (wait 0 ticks if full)
            xQueueSend(xTelemetryQueue, &sensorUpdate, 0);
        }
        if (g_temperature < 5 && !g_heater_on) {
            g_heater_on = true;
            safe_uart_print("[AUTO] Low Temp Detected! Turning on Heater.\n");
        }
        sensor_checkin = 1; // I'm alive!
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

void vWatchdogTask(void *pvParameters) {
    for(;;) {
        // Wait 3 seconds before checking
        vTaskDelay(pdMS_TO_TICKS(3000));

        if (sensor_checkin && telemetry_checkin) {
            safe_uart_print("WATCHDOG: All tasks healthy.\n");
            
            // Clear flags for the next round
            sensor_checkin = 0;
            telemetry_checkin = 0;
        } else {
            safe_uart_print("\n!!! WATCHDOG FAILURE DETECTED !!!\n");
            if (!sensor_checkin) safe_uart_print(" - Sensor Task HUNG\n");
            if (!telemetry_checkin) safe_uart_print(" - Telemetry Task HUNG\n");
            
            // In a real satellite, you'd call a hardware reset here
            safe_uart_print("System Rebooting...\n");
            while(1); // Lock up to simulate a crash/reboot
        }
    }
}

void vCommandTask(void *pvParameters) {
    char c;
    for(;;) {
        c = uart_getc();

        switch (c)
        {
            case ('s'):
            {
                safe_uart_print("\n[COMMAND] Manual Status Check: All Systems Nominal.\n");
                break;
            } 
            case ('r'):
            {
                safe_uart_print("\n[COMMAND] Resetting Telemetry Stream...\n");
                // You could clear the queue here if you wanted!
                xQueueReset(xTelemetryQueue);
                // 2. Reset the failure flags so the Watchdog stops panicking
                sensor_fail_trigger = 0;
                sensor_checkin = 1; // Force a "good" check-in

                // 3. Optional: Reset physics if the error was power-related
                g_battery_level = 100;
                
                safe_uart_print("[SYSTEM] Queue cleared. Sensors normalized.\n");
                break;
            }
            case ('k'):
            {
                safe_uart_print("\n[CHAOS] Injecting fault into Sensor Task...\n");
                sensor_fail_trigger = 1;
                break;
            }
            case ('h'):
            {
                g_heater_on = !g_heater_on;
                if (g_heater_on) {
                    safe_uart_print("\n[COMMAND] Heater ACTIVATED. Power consumption increased.\n");
                } else {
                    safe_uart_print("\n[COMMAND] Heater DEACTIVATED.\n");
                }
                break;
            }
            case 'z':
                safe_uart_print("\n[COMMAND] PERFORMING SYSTEM RECOVERY...\n");
    
                // Reset Environmental Physics
                g_battery_level = 100;
                g_temperature = 25;
                g_in_sunlight = true;
    
                // Reset Fault Flags
                sensor_fail_trigger = 0;
                sensor_checkin = 0;
    
                // Wipe the communication pipe
                xQueueReset(xTelemetryQueue);
    
                safe_uart_print("[SUCCESS] All counters and queues reset to Nominal.\n");
                break;
            case 'i': // Ignore mode
                safe_uart_print("\n[COMMAND] Maintenance Mode: Watchdog Suspended.\n");
                vTaskSuspend(xWatchdogTaskHandle); // Stops the watchdog from killing tasks
                break;
            default:
        }
        // We don't need to check every microsecond
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

void vBatteryTask(void *pvParameters) {
    for(;;) {
        // Drain 1% every 5 seconds
        vTaskDelay(pdMS_TO_TICKS(5000));
        
        if (g_battery_level > 0) {
            g_battery_level--;
        }

        // Logic for "Low Power"
        if (g_battery_level < 10 && g_battery_level > 0) {
            safe_uart_print("\n!!! [POWER] CRITICAL BATTERY LOW !!!\n");
        } 
        else if (g_battery_level <= 0) {
            safe_uart_print("\n[FATAL] Power Lost. Satellite De-orbiting...\n");
            vTaskSuspendAll(); // FreeRTOS command to stop everything
            while(1); 
        }
    }
}

void vEnvironmentTask(void *pvParameters) {
    uint32_t seconds_in_current_state = 0;

    for(;;) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Run every 1 second
        seconds_in_current_state++;

        // Orbit Logic: Switch between Sun and Shadow every 20 seconds
        if (seconds_in_current_state >= 20) {
            g_in_sunlight = !g_in_sunlight;
            seconds_in_current_state = 0;
            
            if (g_in_sunlight) {
                safe_uart_print("\n[ENV] >>> Emerged into Sunlight (Charging...)\n");
            } else {
                safe_uart_print("\n[ENV] >>> Entered Eclipse (Battery Drain Only)\n");
            }
        }

        // Physics Logic
        if (g_in_sunlight) {
            if (g_temperature < 80) g_temperature++; // Heat up in sun
            if (g_battery_level < 100) g_battery_level++; // Charge battery
        }
        else {
            // ECLIPSE LOGIC
            if (g_heater_on) {
                // Heater keeps us at 20 degrees, but drains 3% battery instead of 1%
                if (g_temperature < 20) g_temperature++; 
                g_battery_level -= 3;
            }
            else
            {
                if (g_temperature > -40) g_temperature--; // Cool down in shade
                g_battery_level--; // Drain battery
            }
        }
    }
}
