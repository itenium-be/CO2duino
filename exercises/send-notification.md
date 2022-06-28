| Index                                       |                         Next exercise |
| :------------------------------------------ | ------------------------------------: |
| [Back to exercises](../README.md#exercises) | [Log data to ThingSpeak](log-data.md) |

# Send a notification to your phone using IFTTT

A high concentration of CO2 in the air is detrimental to a lot of things, we have a harder time concentrating, diseases get spread more efficiently and it can lead to headaches. To remind us to open up a window once in a while we'll set up an [IFTTT](https://ifttt.com/explore) action that sends us a notification whenever the measured CO2 in the air is above 800. Check out the tutorial linked below to see how to do this. Make sure you only send a notification when the last one is at least 10 minutes ago.

> **Note** To receive notifications from IFTTT on your phone you have to install their app.

> **Note** A hardcoded IFTTT URL is fine for the moment, we'll make it configurable in a next exercise

## Useful links

- [ESP32 - IFTTT tutorial](https://esp32io.com/tutorials/esp32-ifttt)
- [Arduino millis() method, see how much time has passed](https://www.arduino.cc/reference/en/language/functions/time/millis/)

## Expected result

![Result](/assets/notification-result.png "Result")

Everything works? Great! Head on over to the next exercise, [Log data to ThingSpeak](log-data.md)!
