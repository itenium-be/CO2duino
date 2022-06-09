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
WiFiManager wifiManager;
char iftttUrlFromWiFiManager[100];
char thingspeakUrlFromWiFiManager[100];
WiFiManagerParameter custom_ifttt_url("ifttturl", "IFTTT Webhook URL", iftttUrlFromWiFiManager, 100);
WiFiManagerParameter custom_thingspeak_url("thingspeakurl", "Thingspeak URL", thingspeakUrlFromWiFiManager, 100);

// URL's for API calls
String ifttt_url = "";
String thingspeak_url = "";

// SGP30 CO2 sensor
Adafruit_SGP30 sgp;

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

// Methods
void resetScreen();
void measureLoop();
void drawProgressBar(int16_t maxValue, int16_t currentValue);
void showBootscreen();
void configModeCallback(WiFiManager *myWiFiManager);
bool checkConnection();
void sendNotificationNonBlocking(void *parameter);
void setURLs();
void sendDataNonBlocking(void *parameter);

void setup()
{
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    delay(3000);

    // Start the display
    display.begin();
    resetScreen();

    // Bootscreen allows the CO2 sensor some time to warm up
    showBootscreen();

    // For testing purposes of the WiFiManager uncomment the next line, this gives you a fresh start each boot
    // wifiManager.resetSettings();

    // Set up the time on our ESP32
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Initialize the Preferences library and check for existing values
    preferences.begin("co2meter", false);
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

    display.setFont();
    display.cp437(true);
    display.dim(true);
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
        setURLs();
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

    if (wifiManager.getWiFiIsSaved())
    {
        resetScreen();
        display.println("Found saved network");
        display.println(wifiManager.getWiFiSSID());
        display.println("Trying to connect");
        display.display();
        return wifiManager.autoConnect(ap_name);
    }
    else
    {
        resetScreen();
        display.print("Connect to ");
        display.println(ap_name);
        display.println("to continue setup");
        display.display();
        return wifiManager.startConfigPortal(ap_name);
    }
}

void setURLs()
{
    // Get value from WiFiManager
    strcpy(iftttUrlFromWiFiManager, custom_ifttt_url.getValue());
    String iftttUrlFromWiFiManager_string = String(iftttUrlFromWiFiManager);

    Serial.println("Setting the IFTTT URL");
    Serial.println("Value from WiFiManager: ");
    Serial.println(iftttUrlFromWiFiManager_string);
    Serial.println("Value in Preferences: ");
    Serial.println(iftttUrlFromPreferences);

    // The WiFiManager passed us a IFTTT URL
    if (iftttUrlFromWiFiManager_string != "")
    {
        ifttt_url = iftttUrlFromWiFiManager_string;
        preferences.putString("IFTTT_url", ifttt_url);
    }
    // If no URL was passed, check for a previously saved one in the Preferences
    else if (iftttUrlFromPreferences != "")
    {
        ifttt_url = iftttUrlFromPreferences;
    }
    Serial.println("Used IFTTT URL: ");
    Serial.println(ifttt_url);

    // Get value from WiFiManager
    strcpy(thingspeakUrlFromWiFiManager, custom_thingspeak_url.getValue());
    String thingspeakUrlFromWiFiManager_string = String(thingspeakUrlFromWiFiManager);

    Serial.println("Setting the Thingspeak URL");
    Serial.println("Value from WiFiManager: ");
    Serial.println(thingspeakUrlFromWiFiManager_string);
    Serial.println("Value in Preferences: ");
    Serial.println(thingspeakUrlFromPreferences);

    // The WiFiManager passed us a Thingspeak URL
    if (thingspeakUrlFromWiFiManager_string != "")
    {
        thingspeak_url = thingspeakUrlFromWiFiManager_string;
        preferences.putString("ThingSpeak_url", ifttt_url);
    }
    // If no URL was passed, check for a previously saved one in the Preferences
    else if (iftttUrlFromPreferences != "")
    {
        thingspeak_url = thingspeakUrlFromPreferences;
    }
    Serial.println("Used ThingSpeak URL: ");
    Serial.println(thingspeak_url);
}

int counter = 0;
ulong lastNotification = 0;
bool invertColors = false;
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
            // We send the notification using a task so it doesn't block the main loop
            // the task kills itself
            xTaskCreate(
                sendNotificationNonBlocking, // Function that should be called
                "Send GET request to IFTT",  // Name of the task (for debugging)
                10000,                       // Stack size (bytes)
                NULL,                        // Parameter to pass
                1,                           // Task priority
                NULL                         // Task handle
            );

            lastNotification = millis();
        }
    }

    // Every 30 seconds we send data to thingspeak
    if (counter % 30 == 0)
    {
        xTaskCreate(
            sendDataNonBlocking,              // Function that should be called
            "Send GET request to thingspeak", // Name of the task (for debugging)
            10000,                            // Stack size (bytes)
            NULL,                             // Parameter to pass
            1,                                // Task priority
            NULL                              // Task handle
        );
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
            Serial.println("Saving baseline values to EEPROM");
            Serial.print("TVOC_base: ");
            Serial.println(TVOC_base);
            Serial.print("eCO2_base: ");
            Serial.println(eCO2_base);
            preferences.putUShort("TVOC_base", TVOC_base);
            preferences.putUShort("eCO2_base", eCO2_base);
        }
        // We invert the colors of the display to reduce burn in
        display.invertDisplay(invertColors);
        invertColors = !invertColors;
        counter = 0;
    }

    display.display();

    delay(1000);
}

void sendNotificationNonBlocking(void *parameter)
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
    vTaskDelete(NULL);
}

void sendDataNonBlocking(void *parameter)
{
    if (thingspeak_url != "")
    {

        HTTPClient http;
        String url = thingspeak_url + "&field1=" + sgp.eCO2 + "&field2=" + sgp.TVOC;
        http.begin(url);
        Serial.println("Sending GET request to: ");
        Serial.println(url);
        int httpResponseCode = http.GET();
        http.end();
    }
    vTaskDelete(NULL);
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