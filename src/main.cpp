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

// WiFiManager
WiFiManager wm;
char wm_ifttt_url[100];
String ifttt_url = "";
WiFiManagerParameter custom_ifttt_url("ifttturl", "IFTTT Webhook URL", wm_ifttt_url, 100);

// SGP30 CO2 sensor
Adafruit_SGP30 sgp;

// Preferences
Preferences preferences;
String pref_ifttt_url;
uint16_t TVOC_base = 0;
uint16_t eCO2_base = 0;

// Time
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// Methods
void resetScreen();
void measureLoop();
void drawProgressBar(int16_t maxValue, int16_t currentValue);
void showBootscreen();
void configModeCallback(WiFiManager *myWiFiManager);
bool checkConnection();
void sendNotification();
void setIFTTTUrl();

void setup()
{
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    delay(3000);

    // For testing purposes of the WiFiManager uncomment the next line, this gives you a fresh start each boot
    wm.resetSettings();

    // Set up the time on our ESP32
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Initialize the Preferences library and check for existing values
    preferences.begin("co2meter", false);
    TVOC_base = preferences.getUShort("TVOC_base", 0);
    eCO2_base = preferences.getUShort("eCO2_base", 0);
    pref_ifttt_url = preferences.getString("IFTTT_url", "");

    // Add a "IFTTT Webhook URL" option to the WiFiManager
    wm.addParameter(&custom_ifttt_url);

    // Start the display
    display.begin();
    resetScreen();

    // Check if we can talk to our CO2 sensor
    if (!sgp.begin())
    {
        display.clearDisplay();
        display.println("Sensor not found :(");
        display.display();
        while (1)
            ;
    }

    // If we have previously saved baseline values for the TVOC and CO2 values, set them here
    if (TVOC_base != 0 && eCO2_base != 0)
    {
        sgp.setIAQBaseline(eCO2_base, TVOC_base);
    }

    // Bootscreen allows the CO2 sensor some time to warm up
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
        Serial.println("Not connected to the network, entering checkConnection");
        // If we're not connected to the WiFi check for a saved network or open the captive portal
        checkConnection();
        // We should be connected, check if the user passed a IFTTT URL
        setIFTTTUrl();
    }
}

bool checkConnection()
{
    String prefix = "CO2-";
    String hostString = String(WIFI_getChipId(), HEX);
    hostString.toUpperCase();
    prefix.concat(hostString);
    char ap_name[50];
    prefix.toCharArray(ap_name, 50);

    if (wm.getWiFiIsSaved())
    {
        resetScreen();
        display.println("Found saved network");
        display.println(wm.getWiFiSSID());
        display.println("Trying to connect");
        display.display();
        return wm.autoConnect(ap_name);
    }
    else
    {
        resetScreen();
        display.print("Connect to ");
        display.println(ap_name);
        display.println("to continue setup");
        display.display();
        return wm.startConfigPortal(ap_name);
    }
}

void setIFTTTUrl()
{
    // Get value from WiFiManager
    strcpy(wm_ifttt_url, custom_ifttt_url.getValue());
    String wm_ifttt_url_string = String(wm_ifttt_url);

    Serial.println("Setting the IFTTT URL");
    Serial.println("Value from WiFiManager: ");
    Serial.println(wm_ifttt_url_string);
    Serial.println("Value in Preferences: ");
    Serial.println(pref_ifttt_url);

    // The WiFiManager passed us a IFTTT URL
    if (wm_ifttt_url_string != "")
    {
        ifttt_url = wm_ifttt_url_string;
        preferences.putString("IFTTT_url", ifttt_url);
    }
    // If no URL was passed, check for a previously saved one in the Preferences
    else if (pref_ifttt_url != "")
    {
        ifttt_url = pref_ifttt_url;
    }
    Serial.println("Used URL: ");
    Serial.println(ifttt_url);
}

int counter = 0;
ulong lastNotification = 0;
void measureLoop()
{
    counter++;
    resetScreen();

    // Try to get data from the SGP30 sensor
    if (!sgp.IAQmeasure())
    {
        display.println("Measurement failed");
        display.display();
        delay(1000);
        return;
    }

    // Display the data on the screen
    display.drawBitmap(0, 0, CO2_icon, 50, 32, 1);
    display.setTextSize(1);
    display.setFont(&FreeSans12pt7b);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(54, 24);
    display.print(sgp.eCO2);
    display.drawBitmap(112, 0, wifi_icon16x16, 16, 16, 1);

    // Check if the CO2 value is above 800 and maybe send a notification
    if (sgp.eCO2 > 800)
    {
        if (counter % 2 == 0)
        {
            display.drawBitmap(112, 16, warning_icon16x16, 16, 16, 1);
        }

        // Only send notification when the last one is more than 10 minutes ago
        if (lastNotification == 0 || millis() - lastNotification > 600000)
        {
            sendNotification();
            lastNotification = millis();
        }
    }

    // Every minute we read out the baseline values and save them to EEPROM
    if (counter == 60)
    {
        if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
        {
            Serial.println("Failed to get baseline readings");
        }
        else
        {
            preferences.putUShort("TVOC_base", TVOC_base);
            preferences.putUShort("eCO2_base", eCO2_base);
        }
        counter = 0;
    }

    display.display();

    delay(1000);
}

void sendNotification()
{
    // Only send notification if we have a IFTTT url
    if (ifttt_url != "")
    {
        HTTPClient http;

        http.begin(ifttt_url);
        Serial.println("Sending GET request to: ");
        Serial.println(ifttt_url);
        int httpResponseCode = http.GET();

        http.end();
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