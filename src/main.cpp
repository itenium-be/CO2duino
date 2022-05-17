#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans18pt7b.h>
#include "Adafruit_SGP30.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPConnect.h>

AsyncWebServer server(80);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET 18

void drawProgressBar(int16_t maxValue, int16_t currentValue);
void showBootscreen();

Adafruit_SGP30 sgp;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
    Serial.begin(9600);

    display.begin();
    display.setCursor(0, 0);
    ESPConnect.autoConnect("CO2 Meter");

    if (ESPConnect.begin(&server))
    {
        display.println("Connected to WiFi");
        display.println("IPAddress: " + WiFi.localIP().toString());
        display.display();
        delay(10000);
    }
    else
    {
        display.println("Failed to connect to WiFi");
        display.display();
    }

    server.begin();

    if (!sgp.begin())
    {
        display.clearDisplay();
        display.println("Sensor not found :(");
        display.display();
        while (1)
            ;
    }

    showBootscreen();

    display.setFont();
    display.cp437(true);

    while (1)
        loop();
}

int counter = 0;
uint16_t TVOC_base = 0;
uint16_t eCO2_base = 0;
void loop()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    if (!sgp.IAQmeasure())
    {
        display.println("Measurement failed");
        display.display();
        return;
    }

    display.print("TVOC ");
    display.print(sgp.TVOC);
    display.println(" ppb\t");
    display.print("eCO2 ");
    display.print(sgp.eCO2);
    display.println(" ppm");
    if (counter == 20)
    {
        counter = 0;

        if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
        {
            display.println("Failed to get baseline readings");
            return;
        }
    }
    display.print("CO2 0x");
    display.print(eCO2_base, HEX);
    display.print("TVOC 0x");
    display.println(TVOC_base, HEX);
    display.display();
    counter++;
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
        delay(250);
    }
}

void drawProgressBar(int16_t maxValue, int16_t currentValue)
{
    const float pixels = float(currentValue * (SCREEN_WIDTH / maxValue));
    display.drawRect(0, 25, SCREEN_WIDTH, 7, SSD1306_WHITE);
    display.fillRect(2, 27, pixels, 3, SSD1306_WHITE);
    display.display();
}