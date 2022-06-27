#ifndef DRAW_FUNCTIONS
#define DRAW_FUNCTIONS

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans18pt7b.h>

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#endif

// Display
extern Adafruit_SSD1306 display;

void resetScreen()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setFont();
    display.cp437(true);
    display.setTextSize(1);
}

void drawProgressBar(int16_t maxValue, int16_t currentValue)
{
    const float pixels = float(currentValue * (SCREEN_WIDTH / maxValue));
    display.drawRect(0, 25, SCREEN_WIDTH, 7, SSD1306_WHITE);
    display.fillRect(2, 27, pixels, 3, SSD1306_WHITE);
    display.display();
}

#endif