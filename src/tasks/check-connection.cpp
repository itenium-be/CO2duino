#include "check-connection.h"
#include <Arduino.h>
#include <esp_task_wdt.h>
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
    // Add task to watchdog
    esp_task_wdt_add(NULL);

    for (;;)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi not connected, starting autoConnect");

            // WiFi is disconnected
            data.wifiConnected = false;

            // Try autoconnect, it will return after 3 minutes if not connected
            wifiManager.autoConnect(data.accessPointName);

            // Reset watchdog timer
            esp_task_wdt_reset();
        }
        else
        {
            Serial.println("WiFi is connected");
            data.wifiConnected = true;

            // Reset watchdog timer
            esp_task_wdt_reset();

            // Wait one minute and check connection again
            vTaskDelay(60000 / portTICK_PERIOD_MS);
        }
    }
}