#include "send-notification.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include "../data-model.h"
#include <esp_task_wdt.h>

// Data struct
extern CO2Data data;

// Send notification through IFTT, task kills itself
void sendNotification(void *parameter)
{
    // Add task to watchdog
    esp_task_wdt_add(NULL);

    // Only send notification if we have a IFTTT url
    if (data.ifttt_url != "" && data.wifiConnected)
    {
        HTTPClient http;
        http.begin(data.ifttt_url);
        Serial.println("Sending GET request to: ");
        Serial.println(data.ifttt_url);
        int httpResponseCode = http.GET();

        http.end();
    }

    // Reset watchdog timer
    esp_task_wdt_reset();

    vTaskDelete(NULL);
}