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
11. 

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

![Diagram](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ESP32Drum.drawio.svg)

## Materials Needed

We need an ESP32 expansion board so that we have enough pins for the piezoelectric sensor with minimal effort. Each piezoelectric sensor will be connected to the expansion board with two cables. Because GND and analog input are separated by VCC, we need 3x1 dupont headers for each piezoelectric sensor.

![Diagram](https://github.com/kamshory/ESP32DrumPad/blob/main/images/esp32-expansion-board-1.jpg)

![Diagram](https://github.com/kamshory/ESP32DrumPad/blob/main/images/esp32-expansion-board-2.jpg)

We do not need the VCC pin to supply power to the piezoelectric sensor because the piezoelectric sensor will generate an electrical voltage of itself when it receives enough vibrations. Thus, we can remove the VCC jumper pin. However, if we want to add another device, for example a sound generator that will be integrated with the MIDI controller that we are going to make, we can choose to use a jumper and set it according to the voltage we need.