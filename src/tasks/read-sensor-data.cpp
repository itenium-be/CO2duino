#include "read-sensor-data.h"
#include <Arduino.h>
#include <Adafruit_SGP30.h>
#include "../data-model.h"
#include "send-notification.h"

// SGP30 CO2 sensor
extern Adafruit_SGP30 sgp;

// SGP30 mutex
extern SemaphoreHandle_t sgp30Mutex;

// Data struct
extern CO2Data data;

// Read CO2 and TVOC values from our SGP30 sensor every second
void readSensorData(void *parameter)
{
    for (;;)
    {
        // Take control of the SGP30 mutex before accessing the sensor
        xSemaphoreTake(sgp30Mutex, portMAX_DELAY);

        // Check if we've got data from the sensor
        // if failed, log to serial and try again after a second
        if (!sgp.IAQmeasure())
        {
            Serial.println("Measurement failed");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        data.measured_co2 = sgp.eCO2;
        data.measured_tvoc = sgp.TVOC;

        // See if we've got a high reading
        if (data.measured_co2 > 800)
        {
            // Only send notification when the last one is more than 10 minutes ago
            if (data.lastNotification == 0 || millis() - data.lastNotification > 600000)
            {
                // We send the notification using a task so it doesn't block the main loop
                // the task kills itself
                xTaskCreate(
                    sendNotification,           // Function that should be called
                    "Send GET request to IFTT", // Name of the task (for debugging)
                    10000,                      // Stack size (bytes)
                    NULL,                       // Parameter to pass
                    1,                          // Task priority
                    NULL                        // Task handle
                );

                data.lastNotification = millis();
            }
        }

        // Release the SGP30 mutex
        xSemaphoreGive(sgp30Mutex);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}