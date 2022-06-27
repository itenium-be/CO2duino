#include "reduce-burn-in.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_task_wdt.h>
#include "../data-model.h"

// Data struct
extern CO2Data data;

// Display
extern Adafruit_SSD1306 display;

// Every minute invert the display's colors to reduce OLED burn in
void reduceBurnIn(void *parameter)
{
    // Add task to watchdog
    esp_task_wdt_add(NULL);

    for (;;)
    {
        display.invertDisplay(data.invertColors);
        data.invertColors = !data.invertColors;

        // Reset watchdog timer
        esp_task_wdt_reset();

        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}