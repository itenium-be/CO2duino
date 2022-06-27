#include "save-baseline-data.h"
#include <Arduino.h>
#include <esp_task_wdt.h>
#include <Adafruit_SGP30.h>
#include <Preferences.h>
#include "../data-model.h"

// SGP30 CO2 sensor
extern Adafruit_SGP30 sgp;

// SGP30 mutex
extern SemaphoreHandle_t sgp30Mutex;

// Data struct
extern CO2Data data;

extern Preferences preferences;
extern uint16_t TVOC_base;
extern uint16_t eCO2_base;

// Every minute read out the sensor's baseline data and save it to Preferences
void saveBaselineData(void *parameter)
{
    // Add task to watchdog
    esp_task_wdt_add(NULL);

    for (;;)
    {
        // Take control of the SGP30 mutex before accessing the sensor
        xSemaphoreTake(sgp30Mutex, portMAX_DELAY);

        if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
        {
            Serial.println("Failed to get baseline readings");
        }
        else
        {
            Serial.println("Saving baseline values to EEPROM");
            Serial.print("TVOC_base: ");
            Serial.println(TVOC_base);
            Serial.print("eCO2_base: ");
            Serial.println(eCO2_base);
            preferences.putUShort("TVOC_base", TVOC_base);
            preferences.putUShort("eCO2_base", eCO2_base);
        }

        // Release the SGP30 mutex
        xSemaphoreGive(sgp30Mutex);

        // Reset watchdog timer
        esp_task_wdt_reset();

        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}