#include "set-urls.h"
#include <Arduino.h>
#include <Preferences.h>
#include <WiFiManager.h>
#include "../data-model.h"

extern char iftttUrlFromWiFiManager[100];
extern char thingspeakUrlFromWiFiManager[100];
extern WiFiManagerParameter custom_ifttt_url;
extern WiFiManagerParameter custom_thingspeak_url;

extern Preferences preferences;
extern String iftttUrlFromPreferences;
extern String thingspeakUrlFromPreferences;

// Data struct
extern CO2Data data;

void setURLs(void *parameter)
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
        data.ifttt_url = iftttUrlFromWiFiManager_string;
        preferences.putString("IFTTT_url", data.ifttt_url);
    }
    // If no URL was passed, check for a previously saved one in the Preferences
    else if (iftttUrlFromPreferences != "")
    {
        data.ifttt_url = iftttUrlFromPreferences;
    }
    Serial.println("Used IFTTT URL: ");
    Serial.println(data.ifttt_url);

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
        data.thingspeak_url = thingspeakUrlFromWiFiManager_string;
        preferences.putString("ThingSpeak_url", data.thingspeak_url);
    }
    // If no URL was passed, check for a previously saved one in the Preferences
    else if (iftttUrlFromPreferences != "")
    {
        data.thingspeak_url = thingspeakUrlFromPreferences;
    }
    Serial.println("Used ThingSpeak URL: ");
    Serial.println(data.thingspeak_url);

    vTaskDelete(NULL);
}