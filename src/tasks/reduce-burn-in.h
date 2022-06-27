#ifndef TASK_REDUCE_BURN_IN
#define TASK_REDUCE_BURN_IN

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "../data-model.h"

// Data struct
extern CO2Data data;

// Display
extern Adafruit_SSD1306 display;

// Every minute invert the display's colors to reduce OLED burn in
void reduceBurnIn(void *parameter)
{
    for (;;)
    {
        display.invertDisplay(data.invertColors);
        data.invertColors = !data.invertColors;

        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}

#endif