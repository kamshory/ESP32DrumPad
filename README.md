# ESP32DrumPad

ESP32DrumPad is drum pad MIDI controller

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

![Diagram](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ESP32Drum.drawio.svg)

## Materials Needed

We need an ESP32 expansion board so that we have enough pins for the piezoelectric sensor with minimal effort. Each piezoelectric sensor will be connected to the expansion board with two cables. Because GND and analog input are separated by VCC, we need 3x1 dupont headers for each piezoelectric sensor.

![Image1](https://github.com/kamshory/ESP32DrumPad/blob/main/images/esp32-expansion-board-1.jpg)

![Image2](https://github.com/kamshory/ESP32DrumPad/blob/main/images/esp32-expansion-board-2.jpg)

![Image3](https://github.com/kamshory/ESP32DrumPad/blob/main/images/piezoelectric-sensor.jpg)

We do not need the VCC pin to supply power to the piezoelectric sensor because the piezoelectric sensor will generate an electrical voltage of itself when it receives enough vibrations. Thus, we can remove the VCC jumper pin. However, if we want to add another device, for example a sound generator that will be integrated with the MIDI controller that we are going to make, we can choose to use a jumper and set it according to the voltage we need.

We still use the built-in USB on the ESP32 board to connect it to the PC because we will use this USB as a MIDI controller. The USB on the expansion board is only used as a power supply if needed or in scenarios using BLE (Bluetooth Low Energy) to transmit MIDI signals to the PC.

## Wiring

![Wiring](https://github.com/kamshory/ESP32DrumPad/blob/main/images/wiring.drawio.svg)

The ESP32 WROOM-32 only allows us to create drum pads with 15 channels. This is because there are only 15 pins that can function as ADC (Analog to Digital Converter). However, our application allows the use of an ESP32 S3 which has a 16 pin ADC.

When using the ESP32 WROOM-32, the application must be limited so that it does not scan pin 16, causing the application to become unstable. This setting is hard coded in the application before flashing it to the ESP32.

## EEPROM Data Structure

### EEPROM Data Offset

| Data                | Offset | Length | Type   | Description                         |
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
| offsetCh1           | 292    | 10     | String | Channel 1                           |
| offsetCh2           | 302    | 10     | String | Channel 2                           |
| offsetCh3           | 312    | 10     | String | Channel 3                           |
| offsetCh4           | 322    | 10     | String | Channel 4                           |
| offsetCh5           | 332    | 10     | String | Channel 5                           |
| offsetCh6           | 342    | 10     | String | Channel 6                           |
| offsetCh7           | 352    | 10     | String | Channel 7                           |
| offsetCh8           | 362    | 10     | String | Channel 8                           |
| offsetCh9           | 372    | 10     | String | Channel 9                           |
| offsetCh10          | 382    | 10     | String | Channel 10                          |
| offsetCh11          | 392    | 10     | String | Channel 11                          |
| offsetCh12          | 402    | 10     | String | Channel 12                          |
| offsetCh13          | 412    | 10     | String | Channel 13                          |
| offsetCh14          | 422    | 10     | String | Channel 14                          |
| offsetCh15          | 432    | 10     | String | Channel 15                          |
| offsetCh16          | 442    | 10     | String | Channel 16                          |
| offsetMCUser        | 452    | 20     | String | MIDI Controller username            |
| offsetMCPassword    | 472    | 32     | String | MIDI Controller password            |

We use at least 504 bytes of EEPROM to store the application configuration. So far we do not need additional storage media in the form of an external EEPROM module or CF card module.

Each channel has an EEPROM address allocation to store the channel configuration. Even though the perchannel configuration data type is string, this data is divided into several segments that have different data types.

### Channel Configuration

| Offset | Length | Type       | Range        | Usage                          |
| ------ | ------ | ---------- | ------------ | ------------------------------ |
| 0      | 1      | Byte       | 0-127        | Configuration flag             |
| 1      | 1      | Byte       | 0-127        | Instrument code                |
| 2      | 2      | Word       | 0-65535      | Threshold (minimum value)      |
| 4      | 2      | Word       | 0-65535      | Head room                      |
| 6      | 4      | DWord      | 0-4294967295 | Duration (micro second)        |

Configuration flag, instrument code, threshold, head room and duration are read separately by different functions. To read data with the `byte` type, the function will read one byte of data at the specified address. To read data with type `word`, the function will read two bytes of data starting from the specified address. To read data with `double word` type, the function will read four bytes of data starting from the specified address.

```c
void readChannelConfig(int channel)
{
    int offset = getChannelOffset(channel);
    byte configured = readByte(offset);
    byte instrumentCode = readByte(offset + 1);
    uint16_t threshold = readWord(offset + 2);
    uint16_t headRoom = readWord(offset + 4);
    uint32_t duration = readDoubleWord(offset + 6);
    
    // add code here
}

int getChannelOffset(int channel)
{
    return memOffset + ((channel - 1) * memSize);
} 
```

### Convert Voltage to Velocity

To convert average stress into velocity, the calculation uses the following parameters:

1. 12 bit voltage input (0 - 4095) is obtained from the average voltage on the ADC PIN
2. 12 bit threshold (0 - 4095) which will reduce the original average voltage to prevent vibrations from one pad propagating to other pads. The smaller the threshold, the weaker the strike will produce vibrations on other pads that are not hit. The greater the threshold, the less sensitive the pad is, which causes the player to have to hit faster to get high velocity.
3. head room (0 - 4095) is the maximum tension produced by the fastest shot of the average player. The smaller the headroom, the smaller the head will produce high velocity.

The threshold will basically have almost the same value for all pads because the threshold prevents nearby pads from being accidentally triggered if the pad next to it is hit at very high speed. Meanwhile, head room can have very varying values depending on the instrument used. For example, the snare may require a harder hit than the hat.

Users must enter the correct threshold and head room values to get the best results. Default values which are test results must be provided by the application.


![Velocity](https://github.com/kamshory/ESP32DrumPad/blob/main/images/velocity.drawio.svg)

The ESP32 takes several voltage samples and then averages them. This average value is then converted into velocity. In the example above, it is known that the threshold is equal to 383 and the head room is equal to 2059. The voltage that will be converted to velocity is above 0.309450549 volts while voltages below 0.309450549 volts will be ignored. Meanwhile, any voltage will be reduced by 0.309450549 volts. A voltage of 2.0625 volts will be converted to a velocity of 127 while a voltage of 0.309450549 volts will be converted to a velocity of 0. The relationship between voltage and velocity is as follows:

```
velocity = 127 * (voltage - 0.309450549 / (2.0625 - 0.309450549))
```

where `voltage` is the average voltage of several voltage samples taken.

We can write the diagram above into the following function:

```c
uint8_t calcVelocity(uint16_t inp, uint16_t thd, uint16_t headRoom)
{
    uint16_t inp2 = inp >= thd ? inp - thd : 0;
    float outp2 = 127 * (float) inp2 / (float)(headRoom - thd);
    if(outp2 > 127)
    {
        outp2 = 127;
    } 
    return uint8_t round(outp2);
}
```

### Duration

Duration in general is the time calculated from the `note on` signal to the next `note off` signal on the same channel and instrument or note. When it comes to drum pads, duration is very important because this will affect the speed at which the player plays the drums. The default value of the duration must be provided by the application.

## Need Support

Please support our YouTube channel by subsribe it at https://www.youtube.com/@MusicPlanetbiru
