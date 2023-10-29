# ESP32DrumPad

ESP32DrumPad

## Features

1. 15 instument pads (16 for ESP32 S3)
2. enable velocity
3. low latency
4. configurable
5. WIFI access point
6. WIFI client
7. built-in web server
8. support USB
9. support websocket
10. support bluetooth low energy (BLE)

### 15 Instrument Pad

ESP32 has 15 pin ADC and each pin can read analog signal to be converted into velocity.

### Enable Velocity

ESP32 read signal as analog so it can be converted into velocity. ESP32 maybe read more than one sample of signal to get more accurate value.

### Low Latency

ESP32 read each ADC asynchronously so it has low latency.

### Configurable

User can configure instrument, threshold, and velocity to each pads.

### WIFI Access Point

WIFI access poin is used to connect to ESP32 before it connected to other WIFI.

### WIFI Client

ESP32 can connect to other WIFI so it can send MIDI signal over internet.

### Built-In Web Server

Web server is used to configure MIDI controller.

### Support USB

ESP32 can send MIDI signal over USB and it recognized as MIDI controller.

### Support WebSocket

ESP32 can send MIDI signal over LAN using WebSocket protocol.

### Support BLE

ESP32 can send MIDI signal over bluetooth and it recognized as MIDI controller.

## Diagram

![Diagram](https://github.com/kamshory/ESP32DrumPad/blob/main/ESP32Drum.drawio.svg)
