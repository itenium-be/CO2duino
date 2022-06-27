#ifndef TASK_CHECK_CONNECTION
#define TASK_CHECK_CONNECTION

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "../data-model.h"

// Data struct
extern CO2Data data;

// WiFiManager
extern WiFiManager wifiManager;

void checkConnection(void *parameter)
{
    for (;;)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi not connected, starting autoConnect");

            // WiFi is disconnected
            data.wifiConnected = false;

            // Try autoconnect, it will return after 3 minutes if not connected
            wifiManager.autoConnect(data.accessPointName);
        }
        else
        {
            Serial.println("WiFi is connected");
            data.wifiConnected = true;

            // Wait one minute and check connection again
            vTaskDelay(60000 / portTICK_PERIOD_MS);
        }
    }
}

#endif