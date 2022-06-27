#ifndef TASK_SEND_NOTIFICATION
#define TASK_SEND_NOTIFICATION

#include <Arduino.h>
#include <HTTPClient.h>
#include "../data-model.h"

// Data struct
extern CO2Data data;

// Send notification through IFTT, task kills itself
void sendNotification(void *parameter)
{
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

    vTaskDelete(NULL);
}

#endif