| [Back to exercises](../README.md#exercises) | [Display sensor data on the screen](display-sensor-data.md) |
| :--- | ---: |

# Initializing the sensor

Check the [Adafruit SGP30](https://github.com/adafruit/Adafruit_SGP30) github repo to read about the sensor we're going to use.

The library is already installed in the project, so check out the provided example in the GitHub repository and start coding!

Set up the sensor in the `setup()` function and read it out in the `loop()` function. Write out the data each second to the serial monitor using `Serial.println(data)` and a `delay(1000)` at the end of your loop.
