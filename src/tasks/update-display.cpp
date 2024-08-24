#include "update-display.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans12pt7b.h>
#include "../data-model.h"
#include "../icons.h"
#include "../draw-functions.h"

// Data struct
extern CO2Data data;

// Display
extern Adafruit_SSD1306 display;

// Update display every second
void updateDisplay(void *parameter)
{
    for (;;)
    {
        resetScreen();

        // Display the data on the screen
        display.drawBitmap(0, 0, CO2_icon, 50, 32, 1);
        display.setTextSize(1);
        display.setFont(&FreeSans12pt7b);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(54, 24);
        display.print(data.measured_co2);

        // Show the WiFi icon if connected, blink if not connected
        if (data.wifiConnected)
        {
            display.drawBitmap(112, 0, wifi_icon16x16, 16, 16, 1);
        }
        else if (data.blink)
        {
            display.drawBitmap(112, 0, wifi_icon16x16, 16, 16, 1);
        }

        // Check if the CO2 value is above 800 to blink the warning icon
        if (data.measured_co2 > 800)
        {
            if (data.blink)
            {
                display.drawBitmap(112, 16, warning_icon16x16, 16, 16, 1);
            }
        }

        data.blink = !data.blink;

        display.display();

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}