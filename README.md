# CO2duino

A DIY CO2 meter based on the [Wemos S2 Pico](https://www.wemos.cc/en/latest/s2/s2_pico.html) dev board and a [SGP30 CO2 sensor](https://www.tinytronics.nl/shop/nl/sensoren/lucht/gas/sgp30-tvoc-en-eco2-sensor-module). It's programmed using [PlatformIO](https://platformio.org/) using the Arduino framework.


![A 3D render of the end result](/assets/co2-meter-render.jpeg "Render")


## Initial setup

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