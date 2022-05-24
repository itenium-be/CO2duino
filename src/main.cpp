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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET 18

WiFiManager wm;
Adafruit_SGP30 sgp;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void resetScreen();
void measureLoop();
void drawProgressBar(int16_t maxValue, int16_t currentValue);
void showBootscreen();
void configModeCallback(WiFiManager *myWiFiManager);
bool checkConnection();
void sendNotification();

unsigned char wifi_icon16x16[] =
    {
        0b00000000, 0b00000000, //
        0b00000111, 0b11100000, //      ######
        0b00011111, 0b11111000, //    ##########
        0b00111111, 0b11111100, //   ############
        0b01110000, 0b00001110, //  ###        ###
        0b01100111, 0b11100110, //  ##  ######  ##
        0b00001111, 0b11110000, //     ########
        0b00011000, 0b00011000, //    ##      ##
        0b00000011, 0b11000000, //       ####
        0b00000111, 0b11100000, //      ######
        0b00000100, 0b00100000, //      #    #
        0b00000001, 0b10000000, //        ##
        0b00000001, 0b10000000, //        ##
        0b00000000, 0b00000000, //
        0b00000000, 0b00000000, //
        0b00000000, 0b00000000, //
};

unsigned char warning_icon16x16[] =
    {
        0b00000000, 0b10000000, //         #
        0b00000001, 0b11000000, //        ###
        0b00000001, 0b11000000, //        ###
        0b00000011, 0b11100000, //       #####
        0b00000011, 0b01100000, //       ## ##
        0b00000111, 0b01110000, //      ### ###
        0b00000110, 0b00110000, //      ##   ##
        0b00001110, 0b10111000, //     ### # ###
        0b00001100, 0b10011000, //     ##  #  ##
        0b00011100, 0b10011100, //    ###  #  ###
        0b00011000, 0b10001100, //    ##   #   ##
        0b00111000, 0b00001110, //   ###       ###
        0b00110000, 0b10000110, //   ##    #    ##
        0b01111111, 0b11111111, //  ###############
        0b01111111, 0b11111111, //  ###############
        0b00000000, 0b00000000, //
};

// 'CO2 icon', 50x32px
const unsigned char CO2_icon[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0xff,
    0xc0, 0x00, 0x00, 0x0f, 0xff, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x1f, 0x0f, 0x03, 0xe1, 0xf0, 0x00,
    0x00, 0x1c, 0x07, 0x83, 0x80, 0x70, 0x00, 0x00, 0x38, 0x03, 0x87, 0x80, 0x78, 0x00, 0x00, 0x38,
    0x03, 0xc7, 0x00, 0x38, 0x00, 0x00, 0x78, 0x01, 0xcf, 0x00, 0x3c, 0x00, 0x00, 0x70, 0x01, 0x8e,
    0x00, 0x1c, 0x00, 0x00, 0x70, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x00, 0x70, 0x00, 0x0e, 0x00, 0x1c,
    0x00, 0x00, 0x70, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x00, 0x70, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x00,
    0x70, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x00, 0x70, 0x01, 0x8e, 0x00, 0x1c, 0x3c, 0x00, 0x70, 0x01,
    0xce, 0x00, 0x1c, 0x7e, 0x00, 0x78, 0x01, 0xcf, 0x00, 0x3c, 0xe7, 0x00, 0x38, 0x03, 0x87, 0x00,
    0x38, 0xc3, 0x00, 0x38, 0x03, 0x87, 0x80, 0x78, 0x03, 0x00, 0x3c, 0x07, 0x83, 0x80, 0x70, 0x03,
    0x00, 0x1f, 0x0f, 0x03, 0xe1, 0xf0, 0x06, 0x00, 0x0f, 0xfe, 0x01, 0xff, 0xe0, 0x0e, 0x00, 0x07,
    0xfc, 0x00, 0xff, 0xc0, 0x1c, 0x00, 0x01, 0xf0, 0x00, 0x3f, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    delay(3000);

    display.begin();
    resetScreen();

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
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        measureLoop();
    }
    else
    {
        checkConnection();
    }
}

void resetScreen()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setFont();
    display.cp437(true);
    display.setTextSize(1);
}

bool checkConnection()
{
    if (wm.getWiFiSSID() != "")
    {
        resetScreen();
        display.println("Found saved network");
        display.println(wm.getWiFiSSID());
        display.println("Trying to connect");
        display.display();
        return wm.autoConnect();
    }
    else
    {
        resetScreen();
        display.println("Connect to CO-32");
        display.println("to continue setup");
        display.display();
        return wm.startConfigPortal("CO-32");
    }
}

void sendNotification()
{
    HTTPClient http;

    String serverPath = "https://maker.ifttt.com/trigger/co2_too_high/json/with/key/dpXn2QAUsLyEZ_uLHxKHG4";

    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    http.end();
}

int counter = 0;
uint16_t TVOC_base = 0;
uint16_t eCO2_base = 0;
ulong lastNotification = 0;
void measureLoop()
{
    counter++;
    resetScreen();

    if (!sgp.IAQmeasure())
    {
        display.println("Measurement failed");
        display.display();
        return;
    }

    if (counter == 20)
    {
        // SET baseline values
        counter = 0;
    }

    display.drawBitmap(0, 0, CO2_icon, 50, 32, 1);
    display.setTextSize(1);
    display.setFont(&FreeSans12pt7b);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(54, 24);
    display.print(sgp.eCO2);

    display.drawBitmap(112, 0, wifi_icon16x16, 16, 16, 1);
    if (sgp.eCO2 > 800)
    {
        if (counter % 2 == 0)
        {
            display.drawBitmap(112, 16, warning_icon16x16, 16, 16, 1);
        }

        // Only send notification when the last one is more than 10 minutes ago
        if (lastNotification == 0 || millis() - lastNotification > 600000)
        {
            Serial.println("SENDING NOTIFICATION");
            Serial.print("NOW: ");
            Serial.println(millis());
            Serial.print("LAST NOTIFICATION: ");
            Serial.println(lastNotification);
            sendNotification();
            lastNotification = millis();
        }
    }

    display.display();

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