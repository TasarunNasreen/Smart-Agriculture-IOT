# 🌱 Smart Plant IoT — Automated Irrigation System

An IoT-based smart irrigation prototype that monitors soil moisture in real time and
automatically (or manually) controls a water pump through a black/green live dashboard.

Built as part of the **Industrial Internet of Things (IIoT)** coursework at
**KIIT Deemed to be University** — Group-4 (Arghya Mitra, Biswajit Sahoo, Sonalika Das, Tasarun Nasreen).

**[Live Demo →](https://smart-agriculture-iot-pjk7.vercel.app/)** 

![Dashboard](docs/images/dashboard-live-demo.png)

## How it works

```
[Soil Moisture Sensor] --> [ESP8266 NodeMCU] --> [Vercel API] --> [Web Dashboard]
                                  |
                                  v
                          [Relay Module] --> [Water Pump]
```

The ESP8266 reads the capacitive soil sensor every 10 seconds, posts the reading to a
serverless API hosted on Vercel, and pulls the latest pump command on the same cycle.
The dashboard polls the same API to show live data, history, and a manual irrigation
toggle. If the device is offline, the dashboard automatically switches to a **Live Demo**
mode with simulated readings — so the project always looks alive, even at a viva/demo
without the hardware plugged in.

## The physical prototype

| | |
|---|---|
| ![Top view](docs/images/prototype-top-view.png) | ![Wiring](docs/images/prototype-closeup.png) |
| ![Full setup](docs/images/prototype-full-setup.png) | ![Readings table](docs/images/dashboard-readings-table.png) |

## Hardware

- NodeMCU ESP8266
- Capacitive soil moisture sensor
- 1-channel relay module
- Mini submersible water pump
- Silicone tubing + water reservoir
- Breadboard + jumper wires

## Project structure

```
smart-plant-iot/
├── index.html              # Black/green dashboard (static, deploys as-is)
├── api/
│   ├── data.js              # POST sensor readings / GET latest reading
│   └── irrigation.js        # GET/POST pump command
├── firmware/
│   └── smart_plant_esp8266.ino
├── docs/images/             # Prototype + dashboard photos
├── vercel.json
└── package.json
```

## Deploy the dashboard (Vercel)

1. Push this folder to a new GitHub repo.
2. Go to [vercel.com/new](https://vercel.com/new) and import the repo.
3. Leave all build settings as default (no build step needed) → **Deploy**.
4. Vercel gives you a URL like `https://smart-plant-iot.vercel.app` — that's your live dashboard, working immediately in **Live Demo** mode.

Or via CLI:

```bash
npm i -g vercel
cd smart-plant-iot
vercel --prod
```

## Connect the real hardware

1. Open `firmware/smart_plant_esp8266.ino` in Arduino IDE.
2. Install the **ESP8266 board package** + **ArduinoJson** library.
3. Update:
   - `ssid` / `password` → your Wi-Fi
   - `host` → your deployed Vercel domain (no `https://`, no trailing slash)
4. Dip the sensor in dry air and in water to read raw values from Serial Monitor, then set `DRY_VALUE` / `WET_VALUE` accordingly.
5. Flash to the NodeMCU. Once connected, the dashboard automatically switches from **Live Demo** to **Device Connected** and shows real readings.

### Wiring

| ESP8266 Pin | Connects to |
|---|---|
| A0 | Soil sensor analog out |
| D1 | Relay IN |
| 3V3 / GND | Sensor + relay power |

## Going to production

The included API uses in-memory storage, which is enough for a class demo but resets
on a serverless cold start. For a persistent deployment, swap the `store` object in
`api/data.js`/`api/irrigation.js` for [Vercel KV](https://vercel.com/docs/storage/vercel-kv)
or a small database.

## Future scope

- AI-based irrigation prediction
- Weather API integration
- Mobile push alerts
- Solar-powered node

## License

MIT
