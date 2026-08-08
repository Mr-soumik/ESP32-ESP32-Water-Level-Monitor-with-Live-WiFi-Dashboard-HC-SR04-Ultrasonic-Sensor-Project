# ESP32 Water Level Monitor

A self-hosted WiFi dashboard that shows real-time water level using an ESP32 and an HC-SR04 ultrasonic sensor. No app, no cloud — the ESP32 creates its own WiFi network and serves a live dashboard you can open in any phone browser.

Built by [TechTadka360](https://github.com/Mr-soumik)

## Connect with TechTadka360

- YouTube: [@techtadka360official](https://youtube.com/@techtadka360official?si=GdlIntZKv30kPgBk)
- Instagram: [@techtadka360official](https://www.instagram.com/techtadka360official?igsh=cWR4bnhjdWw1MHdh)
- Facebook: [TechTadka360](https://www.facebook.com/share/1EkKAJNLdB/)

## Features

- Live water level % with animated tank fill visual
- Real-time updates every second over WebSocket
- LOW / FULL alert states
- Works fully offline — ESP32 hosts its own access point
- No app installation needed, just connect to WiFi and open a browser

## Hardware Required

- ESP32 dev board
- HC-SR04 ultrasonic sensor
- 1kΩ resistor
- 1.8kΩ resistor (or 2kΩ)
- External 5V power source
- Jumper wires

## Wiring

| HC-SR04 Pin | Connects To |
|---|---|
| VCC | External 5V+ |
| GND | External 5V GND + ESP32 GND (common ground) |
| TRIG | ESP32 GPIO5 |
| ECHO | Voltage divider → ESP32 GPIO22 |

**Voltage divider (required for ECHO):**
ECHO → 1kΩ resistor → junction → 1.8kΩ resistor → GND
Junction point → ESP32 GPIO22

The HC-SR04 outputs 5V on ECHO, but the ESP32's GPIO pins only tolerate 3.3V. The divider steps it down safely.

## Setup

1. Wire the circuit as shown above
2. Open `water_level_monitor.ino` in Arduino IDE
3. Select your ESP32 board under **Tools > Board**
4. Install libraries: `ESPAsyncWebServer`, `AsyncTCP`
5. Upload the sketch

## Calibration

Before use, measure the distance from your sensor to the container:

1. With the container **empty**, note the distance shown
2. Fill it **completely**, note the distance shown
3. Update these two lines in the code:

```cpp
const float EMPTY_DISTANCE_CM = 9.86;
const float FULL_DISTANCE_CM = 4.60;
```

## Usage

1. Power on the ESP32
2. On your phone, connect to WiFi network: **TechTadka360_WaterLevel**
3. Password: **12345678**
4. Open a browser and go to: **192.168.4.1**
5. Watch your live water level dashboard

## License

Free to use, modify, and share. Attribution appreciated but not required.

## Connect with TechTadka360

- YouTube: [@techtadka360official](https://youtube.com/@techtadka360official?si=GdlIntZKv30kPgBk)
- Instagram: [@techtadka360official](https://www.instagram.com/techtadka360official?igsh=cWR4bnhjdWw1MHdh)
- Facebook: [TechTadka360](https://www.facebook.com/share/1EkKAJNLdB/)

---

⭐ If this helped you, consider starring the repo and following [TechTadka360](https://github.com/Mr-soumik) for more ESP32 projects.
