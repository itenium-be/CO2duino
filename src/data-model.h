#ifndef DATA_MODEL
#define DATA_MODEL

#include <Arduino.h>

// Data struct
struct CO2Data
{
    bool wifiConnected = false;
    uint16_t measured_co2 = 400;
    uint16_t measured_tvoc = 0;
    ulong lastNotification = 0;
    bool invertColors = false;
    bool blink = false;
    String ifttt_url = "";
    String thingspeak_url = "";
    char accessPointName[50];
};

#endif