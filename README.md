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

We use at least 500 bytes of EEPROM to store the application configuration. So far we do not need additional storage media in the form of an external EEPROM module or CF card module.

| Data                | Offset | Length | Type   |                                     |
| ------------------- | ------ | ------ | ------ | ----------------------------------- |
| offsetSSID1         | 0      | 20     | String | SSID Wifi                           |
| offsetSSID2         | 20     | 20     | String | SSID AP                             |
| offsetAPIP          | 40     | 15     | String | IP Address AP                       |
| offsetAPGateway     | 55     | 15     | String | Gateway AP                          |
| offsetAPSubnet      | 70     | 15     | String | Subnet AP                           |
| offsetAPHidden      | 85     | 1      | Byte   | Hidden AP                           |
| offsetSSIDPassword1 | 86     | 20     | String | Password Wifi                       |
| offsetSSIDPassword2 | 106    | 20     | String | Password AP                         |
| offsetWSHost        | 126    | 40     | String | Host WS                             |
| offsetWSPort        | 166    | 2      | Word   | Port WS                             |
| offsetWSPath        | 168    | 50     | String | Path WS                             |
| offsetWSUsername    | 218    | 20     | String | Username WS                         |
| offsetWSPassword    | 238    | 20     | String | Password WS                         |
| offsetWSTopic       | 258    | 30     | String | Topic WS                            |
| offsetEnable        | 288    | 1      | Byte   | Enable WS                           |
| offsetConfigured    | 289    | 1      | Byte   | Flag that controller was configured |
| offsetSolo          | 290    | 1      | Byte   | Flag solo channel                   |
| offsetSoloChannel   | 291    | 1      | Byte   | Solo channel number                 |
| offsetCh1           | 292    | 13     | String | Channel 1                           |
| offsetCh2           | 305    | 13     | String | Channel 2                           |
| offsetCh3           | 318    | 13     | String | Channel 3                           |
| offsetCh4           | 331    | 13     | String | Channel 4                           |
| offsetCh5           | 344    | 13     | String | Channel 5                           |
| offsetCh6           | 357    | 13     | String | Channel 6                           |
| offsetCh7           | 370    | 13     | String | Channel 7                           |
| offsetCh8           | 383    | 13     | String | Channel 8                           |
| offsetCh9           | 396    | 13     | String | Channel 9                           |
| offsetCh10          | 409    | 13     | String | Channel 10                          |
| offsetCh11          | 422    | 13     | String | Channel 11                          |
| offsetCh12          | 435    | 13     | String | Channel 12                          |
| offsetCh13          | 448    | 13     | String | Channel 13                          |
| offsetCh14          | 461    | 13     | String | Channel 14                          |
| offsetCh15          | 474    | 13     | String | Channel 15                          |
| offsetCh16          | 487    | 13     | String | Channel 16                          |