#include "send-data.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include "../data-model.h"

// Data struct
extern CO2Data data;

// Every minute send sensor data to ThingSpeak
void sendData(void *parameter)
{
    for (;;)
    {
        if (data.thingspeak_url != "" && data.wifiConnected)
        {

            HTTPClient http;
            String url = data.thingspeak_url + "&field1=" + data.measured_co2 + "&field2=" + data.measured_tvoc;
            http.begin(url);
            Serial.println("Sending GET request to: ");
            Serial.println(url);
            int httpResponseCode = http.GET();
            http.end();
        }

        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}
