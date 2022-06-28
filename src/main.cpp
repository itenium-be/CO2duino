#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Adafruit_SGP30.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <time.h>
#include "icons.h"

// Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 18
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Custom methods must be declared here
void showBootscreen();
void drawProgressBar(int16_t maxValue, int16_t currentValue);

// This code runs once on startup
void setup()
{
    // Start the display
    display.begin();
    display.dim(true);

    showBootscreen();

    display.setFont();
    display.cp437(true);

    WiFi.begin("College", "fuckdeikea");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

bool invertDisplay = true;
int counter = 0;
// This code runs continuously
void loop()
{
    display.clearDisplay();
    display.setTextSize(2);

    display.setCursor(5, 8);

    display.print("Hello");
    if (counter % 2 == 0)
    {
        display.write(0xDB);
    }
    display.println();
    display.display();
    counter++;

    if (counter == 20)
    {
        display.invertDisplay(invertDisplay);
        invertDisplay = !invertDisplay;
        counter = 0;
    }

    delay(1000);
}

void showBootscreen()
{
    display.clearDisplay();

    display.setTextSize(1);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 24);

    display.println(F("itenium"));

    display.display();

    for (size_t i = 0; i < 60; i++)
    {
        drawProgressBar(60, i);
        delay(25);
    }
}

void drawProgressBar(int16_t maxValue, int16_t currentValue)
{
    const float pixels = float(currentValue * (SCREEN_WIDTH / maxValue));
    display.drawRect(0, 25, SCREEN_WIDTH, 7, SSD1306_WHITE);
    display.fillRect(2, 27, pixels, 3, SSD1306_WHITE);
    display.display();
}