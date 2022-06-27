#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SGP30.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <time.h>
#include "icons.h"

#include "data-model.h"
#include "draw-functions.h"

#include "tasks/send-notification.h"
#include "tasks/read-sensor-data.h"
#include "tasks/send-data.h"
#include "tasks/update-display.h"
#include "tasks/reduce-burn-in.h"
#include "tasks/save-baseline-data.h"
#include "tasks/check-connection.h"
#include "tasks/set-urls.h"

// Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 18
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFiManager
WiFiManager wifiManager;
char iftttUrlFromWiFiManager[100];
char thingspeakUrlFromWiFiManager[100];
WiFiManagerParameter custom_ifttt_url("ifttturl", "IFTTT Webhook URL", iftttUrlFromWiFiManager, 100);
WiFiManagerParameter custom_thingspeak_url("thingspeakurl", "Thingspeak URL", thingspeakUrlFromWiFiManager, 100);

// SGP30 CO2 sensor
Adafruit_SGP30 sgp;

// Mutex for the sensor
SemaphoreHandle_t sgp30Mutex;

// Preferences
Preferences preferences;
String iftttUrlFromPreferences;
String thingspeakUrlFromPreferences;
uint16_t TVOC_base = 0;
uint16_t eCO2_base = 0;

// Time
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// Sensor and connection data
CO2Data data;

// Methods
void showBootscreen();
void saveParamsCallback();
void printLocalTime();
void setAccessPointName();

void setup()
{
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    delay(3000);

    // Start the display
    display.begin();
    resetScreen();

    // For testing purposes of the WiFiManager uncomment the next line, this gives you a fresh start each boot
    wifiManager.resetSettings();

    // Initialize the Preferences library and check for existing values
    preferences.begin("co2meter", false);

    // For testing purposes of the Preferences uncomment the next line, this gives you a fresh start each boot
    // preferences.clear();

    TVOC_base = preferences.getUShort("TVOC_base", 0);
    eCO2_base = preferences.getUShort("eCO2_base", 0);
    iftttUrlFromPreferences = preferences.getString("IFTTT_url", "");
    thingspeakUrlFromPreferences = preferences.getString("ThingSpeak_url", "");

    Serial.println("Got values from Preferences: ");
    Serial.print("TVOC_base: ");
    Serial.println(TVOC_base);
    Serial.print("eCO2_base: ");
    Serial.println(eCO2_base);
    Serial.print("iftttUrlFromPreferences: ");
    Serial.println(iftttUrlFromPreferences);
    Serial.print("thingspeakUrlFromPreferences: ");
    Serial.println(thingspeakUrlFromPreferences);

    // Add a "IFTTT Webhook URL" and "Thingspeak URL" option to the WiFiManager
    wifiManager.addParameter(&custom_ifttt_url);
    wifiManager.addParameter(&custom_thingspeak_url);

    wifiManager.setConfigPortalTimeout(180);

    // Set the URLs when WiFi is connected
    wifiManager.setSaveParamsCallback(saveParamsCallback);

    // Set the access point name we are going to use
    setAccessPointName();

    // Check if we can talk to our CO2 sensor
    if (!sgp.begin())
    {
        display.clearDisplay();
        display.println("Sensor not found :(");
        Serial.println("Could not begin the SGP30 sensor");
        display.display();
        while (1)
            ;
    }

    // If we have previously saved baseline values for the TVOC and CO2 values, set them here
    if (TVOC_base != 0 && eCO2_base != 0)
    {
        Serial.println("Setting previously stored baseline values");
        sgp.setIAQBaseline(eCO2_base, TVOC_base);
    }

    // Setup a mutex for the SGP30 sensor, so only  one task can access it at a time
    sgp30Mutex = xSemaphoreCreateMutex();

    // Bootscreen allows the CO2 sensor some time to warm up
    showBootscreen();

    display.setFont();
    display.cp437(true);
    display.dim(true);

    // Initialize our continuously running tasks
    xTaskCreate(readSensorData, "Read sensor data", 10000, NULL, 1, NULL);
    xTaskCreate(updateDisplay, "Update display", 10000, NULL, 1, NULL);
    xTaskCreate(checkConnection, "Check connection", 10000, NULL, 1, NULL);
    xTaskCreate(sendData, "Send data to ThingSpeak", 10000, NULL, 1, NULL);
    xTaskCreate(saveBaselineData, "Save baseline data", 10000, NULL, 1, NULL);
    xTaskCreate(reduceBurnIn, "Reduce burn in", 10000, NULL, 1, NULL);
}

// Since we check all our data in long running tasks, our loop is empty
void loop()
{
}

// This gets called after a user has succesfully connected to the WiFi from our configuration portal
void saveParamsCallback()
{
    // Set the URL's either from preferences or from the wifiManager
    // the task kills itself
    xTaskCreate(setURLs, "Set URL's", 10000, NULL, 1, NULL);

    // Set up the time on our ESP32
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
}

void printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setAccessPointName()
{
    String prefix = "CO2-";
    String hostString = String(WIFI_getChipId(), HEX);
    hostString.toUpperCase();
    prefix.concat(hostString);
    prefix.toCharArray(data.accessPointName, 50);
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

    display.clearDisplay();
    display.display();
}