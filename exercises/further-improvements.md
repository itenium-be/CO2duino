| Index                                       |
| :------------------------------------------ |
| [Back to exercises](../README.md#exercises) |

# Further improvements

Wow, you've built a complete CO2 meter with smart capabilities, all from scratch! I'm super proud of you!

If this workshop has scratched an itch for you and you'd like to go a bit further down the rabbit hole I've listed some ideas and resources here to get you started.

Some of them are implemented in the final firmware you can find on the `master` branch of this repository. So don't hesitate to have a look!

## Tasks and threading

Did you notice your device seemed to hang when it was sending an HTTP request? This is because all the code we've written runs one after another. It might have even felt a bit iffy while writing it. To fix this problem our microcontroller runs a [RTOS (real time OS)](https://en.wikipedia.org/wiki/Real-time_operating_system). This gives us many possibilities to fix this problem. Try to offload the sending of notifications and logging of data to a separate task. Check out the following materials:

- [Multitasking on an ESP32 with Arduino and FreeRTOS](https://savjee.be/2020/01/multitasking-esp32-arduino-freertos/)
- [Introduction to FreeRTOS Video tutorials](https://www.youtube.com/watch?v=F321087yYy4&list=PLEBQazB0HUyQ4hAPU1cJED6t3DU0h34bz)

## Mutexes and Semaphores

What if we put everything the device does into separate tasks? Reading data, updating the display, saving the calibration data, ... . We might run into problems when for example the "Read data" task and the "Save calibration data" task both try to access the CO2 sensor at the same time. This is where mutexes and semaphores come in. Try to request access to the sensor in both tasks using a mutex:

- [Introduction to FreeRTOS: Mutexes](https://www.youtube.com/watch?v=I55auRpbiTs&list=PLEBQazB0HUyQ4hAPU1cJED6t3DU0h34bz&index=6)
- [Introduction to FreeRTOS Github](https://github.com/ShawnHymel/introduction-to-rtos)

## Code style

Up to now, most of our code exists in one huge `main.cpp` file. Read up on C++ code organization to make everything tidy.

- [Source File Organization for C++ blog post](https://arne-mertz.de/2016/06/organizing-headers-and-sources/)

## Monitor WiFi connection

What if our WiFi password changes? The device has no way of reconnecting or notifying the user something is wrong. Write a task to see if our connection is still alive and allows the user to reconnect or reconfigure the WiFi.

- [Keep WiFi alive using a FreeRTOS task](https://savjee.be/2020/02/esp32-keep-wifi-alive-with-freertos-task/)

## Watchdog

What if our device hangs on a specific function? Maybe a reboot would get everything running smoothly again. This is where Watchdogs come in. Implement a Task Watchdog that monitors for long running tasks and reboots the device if something is taking far too long.

- [ESP32 Task watchdog documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/wdts.html#task-watchdog-timer-twdt)

## Change parameters at runtime

What if we'd like to change the URL the notification gets sent to without resetting the device or rewriting a part of our program? Maybe the device listens on it's USB Serial port for a message to change it? Maybe it can spin up a web server where you can update this information? Get creative!

- [Listening to USB serial on a web page](https://learn.adafruit.com/using-webusb-with-arduino-and-tinyusb)
- [ESP32 Web server](https://randomnerdtutorials.com/esp32-web-server-arduino-ide/)
