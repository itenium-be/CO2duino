| Index                                       |                                               Next exercise |
| :------------------------------------------ | ----------------------------------------------------------: |
| [Back to exercises](../README.md#exercises) | [Display sensor data on the screen](display-sensor-data.md) |

# Initializing the sensor

Check the [Adafruit SGP30](https://github.com/adafruit/Adafruit_SGP30) github repo to read about the sensor we're going to use.

The library is already installed in the project, so check out the provided example in the GitHub repository and start coding!

Set up the sensor in the `setup()` function and read it out in the `loop()` function. Write out the data each second to the serial monitor using `Serial.println(data)` and a `delay(1000)` at the end of your loop.

When building and uploading your program to the microcontroller, make sure to use the `Upload and Monitor` option in the PlatformIO Project tasks. This will automatically open the serial monitor after the program is flashed.

> **Note** if the sensor only outputs a value of 400 for the CO2 and 0 for the TVOC, don't panic, this is normal in the first ~30s of polling.

## Expected result

![Result](/assets/initialize-sensor-result.png "Result")

Everything works? Great! Head on over to the next exercise, [displaying this data on the display](display-sensor-data.md)!
