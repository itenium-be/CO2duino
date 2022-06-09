# CO2duino

[[_TOC_]]

A DIY CO2 meter based on the [Wemos S2 Pico](https://www.wemos.cc/en/latest/s2/s2_pico.html) dev board and a [SGP30 CO2 sensor](https://www.tinytronics.nl/shop/nl/sensoren/lucht/gas/sgp30-tvoc-en-eco2-sensor-module). It's programmed using [PlatformIO](https://platformio.org/) and the Arduino framework.


![A photo of the end result](/assets/co2-meter.jpeg "Photo")


# Initial setup

First we'll clone this repository:

```sh
git clone https://gitlab.com/michielr/co2duino.git
```

Next we have to install the PlatformIO extension into Visual Studio Code. Go to extensions, search for `PlatformIO` and press install. The initial install takes some time, you'll get notified after about 2 minutes to reload your VS Code window.

After VS Code has reloaded you should see an icon of a house in the bottom of your VS Code window:

![Home icon](/assets/home-icon.png "Home Icon")

Click on this icon and you'll be greeted by the PlatformIO Home screen. On this screen select 'Open Project' and navigate to your cloned repository to open it.

After pressing `Open`, PlatformIO will begin installing all the dependencies of the project. This will take another couple of minutes. You'll know when it's done when you see the Build icon appear in your VS code bottom bar:

![Build icon](/assets/build-icon.png "Build Icon")


# Exercises

In these exercises we'll build up to a fully functioning CO2 meter, the exercises aren't fleshed out but provide some reading material to get started. As a starting point the program (found in `main.cpp`) shows a bootscreen and some text on the display.

1. [Initialize the CO2 sensor and read data from it](exercises/initialize-sensor.md)
2. [Display sensor data on the screen](exercises/display-sensor-data.md)
3. Save the calibration data to the device
4. Connect to a known WiFi network
5. Set up an access point so the user can choose their WiFi network and enter its password
6. Send a notification to your phone using IFTTT
7. Log data to ThingSpeak
8. Make it pretty

# Resources

## Enclosure

- [Assembly guide](assembly.md)

## CO2 Sensor (Sensirion SGP30)

- [SGP30 Datasheet](https://sensirion.com/media/documents/984E0DD5/61644B8B/Sensirion_Gas_Sensors_Datasheet_SGP30.pdf)
- [Adafruit SGP30 library](https://github.com/adafruit/Adafruit_SGP30)

## Display

- [Adafruit SSD1306 OLED driver](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit Graphics library](https://learn.adafruit.com/adafruit-gfx-graphics-library)

## WiFi connectivity

- [WiFiManager](https://github.com/tzapu/WiFiManager)

## Utility

- [Preferences](https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/preferences.html)