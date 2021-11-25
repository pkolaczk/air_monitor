# ESP8266-based Air Quality Monitor

This is source code of the firmware for the air quality / weather station
I've built. The station measures PM1, PM2.5 and PM10 
particulate matter levels and humidity and temperature. 
The software runs on ESP8266. 
I use WeMoS D1 Mini for easy programming through USB.


## Features
- Connects to local Wifi to get on the Internet.
- Automatically synchronizes with Internet time using NTP.
- Reads temperature and humidity from a DHT22 sensor (can be easily modified to work with DHT11).
- Reads particulate matter levels from Plantower PMS7003 laser sensor.
- Displays air parameters on a 4x20 LCD screen attached using SPI bus.
- Supports LCD screen dimming through PWM control.
- Stores sensor readouts in internal flash (1 MB of free space).
- Runs a tiny webserver and displays air parameters on a minimalistic, responsive website.
- Presents graphs with air quality history in the browser, using Javascript and a bit of REST.
- Can optionally read remote data (e.g. outside temperature) from RF-433 radio receiver (unfinished).
- **New! Uploads sensor readouts to DataStax Astra!**

## Installation
- Install VSCode.
- Install PlatformIO plugin.
- Checkout this project from GitHub and open with PlatformIO.
- Create `data/secret/wifi.txt` file and put you network SSID in the first line and your wifi password in the second line.
- Optionally create `data/secret/astra.txt` file with your DataStax astra access information in 4 lines of text:
  1. database UUID
  2. database region
  3. username (account id)
  4. password (access token) 
- Connect WeMos Mini D1 to USB.
- Hit `Ctrl+Shift+P`, select "PlatformIO: Upload" and wait a few seconds.
- Run `pio run -t uploadfs` to flash the UI files and credentials.
- If something goes wrong, open `Serial Monitor` and read debugging information sent there.

