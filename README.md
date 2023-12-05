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

![Image1](https://github.com/kamshory/ESP32DrumPad/blob/main/images/esp32-expansion-board-1.jpg)

![Image2](https://github.com/kamshory/ESP32DrumPad/blob/main/images/esp32-expansion-board-2.jpg)

We do not need the VCC pin to supply power to the piezoelectric sensor because the piezoelectric sensor will generate an electrical voltage of itself when it receives enough vibrations. Thus, we can remove the VCC jumper pin. However, if we want to add another device, for example a sound generator that will be integrated with the MIDI controller that we are going to make, we can choose to use a jumper and set it according to the voltage we need.

We still use the built-in USB on the ESP32 board to connect it to the PC because we will use this USB as a MIDI controller. The USB on the expansion board is only used as a power supply if needed or in scenarios using BLE (Bluetooth Low Energy) to transmit MIDI signals to the PC.

## Wiring

![Wiring](https://github.com/kamshory/ESP32DrumPad/blob/main/images/wiring.drawio.svg)

The ESP32 WROOM-32 only allows us to create drum pads with 15 channels. This is because there are only 15 pins that can function as ADC (Analog to Digital Converter). However, our application allows the use of an ESP32 S3 which has a 16 pin ADC.

When using the ESP32 WROOM-32, the application must be limited so that it does not scan pin 16, causing the application to become unstable. This setting is hard coded in the application before flashing it to the ESP32.

## EEPROM Data Structure

| Data                   | Offset | Length |
| ---------------------- | ------ | ------ |
| offsetSSID1            | 0      | 50     |
| offsetSSID2            | 50     | 50     |
| offsetAPIP             | 100    | 20     |
| offsetAPGateway        | 120    | 20     |
| offsetAPSubnet         | 140    | 20     |
| offsetAPHidden         | 160    | 1      |
| offsetSSIDPassword1    | 200    | 50     |
| offsetSSIDPassword2    | 250    | 50     |
| offsetWSHost           | 300    | 50     |
| offsetWSPort           | 350    | 10     |
| offsetWSPath           | 360    | 50     |
| offsetWSUsername       | 410    | 50     |
| offsetWSPassword       | 460    | 50     |
| offsetWSTopic          | 510    | 50     |
| offsetEnable           | 570    | 1      |
| offsetConfigured       | 571    | 1      |
| offsetSolo             | 572    | 1      |
| offsetSoloChannel      | 573    | 2      |
| offsetCh1              | 600    | 20     |
| offsetCh2              | 620    | 20     |
| offsetCh3              | 640    | 20     |
| offsetCh4              | 660    | 20     |
| offsetCh5              | 680    | 20     |
| offsetCh6              | 700    | 20     |
| offsetCh7              | 720    | 20     |
| offsetCh8              | 740    | 20     |
| offsetCh9              | 760    | 20     |
| offsetCh10             | 780    | 20     |
| offsetCh11             | 800    | 20     |
| offsetCh12             | 820    | 20     |
| offsetCh13             | 840    | 20     |
| offsetCh14             | 860    | 20     |
| offsetCh15             | 880    | 20     |
| offsetCh16             | 900    | 20     |