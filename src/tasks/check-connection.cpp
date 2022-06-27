#include "check-connection.h"
#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "../data-model.h"
#include "set-urls.h"

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
            if (wifiManager.autoConnect(data.accessPointName))
            {
                // Set the URL's either from preferences or from the wifiManager
                // the task kills itself
                xTaskCreate(setURLs, "Set URL's", 10000, NULL, 1, NULL);
            }
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