# ESP32DrumPad

ESP32DrumPad is drum pad MIDI controller.

For those of you who are still new to MIDI, please read the following references:
https://www.music.mcgill.ca/~ich/classes/mumt306/StandardMIDIfileformat.html

A MIDI controller is any hardware or software that generates and transmits Musical Instrument Digital Interface (MIDI) data to MIDI-enabled devices, typically to trigger sounds and control parameters of an electronic music performance. They most often use a musical keyboard to send data about the pitch of notes to play, although a MIDI controller may trigger lighting and other effects. A wind controller has a sensor that converts breath pressure to volume information and lip pressure to control pitch. Controllers for percussion and stringed instruments exist, as well as specialized and experimental devices. Some MIDI controllers are used in association with specific digital audio workstation software. The original MIDI specification has been extended to include a greater range of control features.

Keyboards can be used to trigger drum sounds, but are impractical for playing repeated patterns such as rolls, due to the length of key travel. After keyboards, drum pads are the next most significant MIDI performance controllers. Drum controllers may be built into drum machines, may be standalone control surfaces, or may emulate the look and feel of acoustic percussion instruments. MIDI triggers can also be installed into acoustic drum and percussion instruments. The pads built into drum machines are typically too small and fragile to be played with sticks, and are played with fingers.

In this project, we will make a drum pad MIDI controller with piezoelectric ceramic as a transducer. This transducer will convert the pressure received into electrical voltage. The electric voltage is converted by an analog to digital converter (ADC) into an integer. This number will be operated with logic and mathematical formulas to get the velocity value of the blows received by the drum pad. Velocity values between 0 and 127 according to MIDI standards. A drum pad consists of several pads, each pad will represent a certain instrument such as kick drum, snare drum, hat, tom, cymbals, and so on.

![Flow](https://github.com/kamshory/ESP32DrumPad/blob/main/images/flow.drawio.svg)

## Features

1. 15 instument pads (16 for ESP32 S3). We will only develop MIDI controller with 12 pads
2. enable velocity
3. low latency
4. WIFI access point
5. WIFI client
6. built-in web server
7. configurable via web application
8. support USB
9. support websocket
10. support bluetooth low energy (BLE)

### 15 Instrument Pad

ESP32 has 15 pin ADC and each pin can read analog signal to be converted into velocity.

### Enable Velocity

ESP32 read signal as analog so it can be converted into velocity. ESP32 maybe read more than one sample of signal to get more accurate value.

### Low Latency

ESP32 read each ADC asynchronously so it has low latency.

### WIFI Access Point

WIFI access poin is used to connect to ESP32 before it connected to other WIFI.

### WIFI Client

ESP32 can connect to other WIFI so it can send MIDI signal over internet.

### Built-In Web Server

Web server is used to configure MIDI controller.

### Configurable via Web Application

User can configure instrument, threshold, and velocity to each pads via web application. User can access an URL after connected to access MIDI controller point.

HTML, CSS and JavaScript code must be written in such a way that it can be compressed into the smallest possible size. One byte of resource on the ESP32 means so much that inefficient code becomes a very big problem, even fatal. Images, whether JPEG, GIF or PNG, are not permitted at all because they will take up so many resources unnecessarily.

**Screenshot**

![SS01](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss01.png)

![SS02](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss02.png)

![SS03](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss03.png)

![SS04](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss04.png)

![SS05](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss05.png)

The arrangement of the pads on the configuration page depends on the arrangement set by the developer before the program is deployed into the ESP32. This arrangement is fixed and cannot be changed by the user. Users can only change the instrument, threshold, headroom and duration on each pad.

![SS06](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss06.png)

![SS06](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss07.png)

![SS06](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss08.png)

![SS06](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss09.png)

![SS06](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss10.png)

![SS06](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss11.png)

![SS06](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ss12.png)

### Support USB

ESP32 can send MIDI signal over USB and it recognized as MIDI controller.

### Support WebSocket

ESP32 can send MIDI signal over LAN using WebSocket protocol.

### Support BLE

ESP32 can send MIDI signal over bluetooth and it recognized as MIDI controller.

## Diagram

![Diagram](https://github.com/kamshory/ESP32DrumPad/blob/main/images/ESP32Drum.drawio.svg)

## MIDI Isntrument Code

| Key | Note | Drum Sound         |
| --- | ---- | ------------------ |
| 35  | B1   | Acoustic Bass Drum |
| 36  | C2   | Bass Drum 1        |
| 37  | C#2  | Side Stick         |
| 38  | D2   | Acoustic Snare     |
| 39  | D#2  | Hand Clap          |
| 40  | E2   | Electric Snare     |
| 41  | F2   | Low Floor Tom      |
| 42  | F#2  | Closed Hi Hat      |
| 43  | G2   | High Floor Tom     |
| 44  | G#2  | Pedal Hi-Hat       |
| 45  | A2   | Low Tom            |
| 46  | A#2  | Open Hi-Hat        |
| 47  | B2   | Low-Mid Tom        |
| 48  | C3   | Hi Mid Tom         |
| 49  | C#3  | Crash Cymbal 1     |
| 50  | D3   | High Tom           |
| 51  | D#3  | Ride Cymbal 1      |
| 52  | E3   | Chinese Cymbal     |
| 53  | F3   | Ride Bell          |
| 54  | F#3  | Tambourine         |
| 55  | G3   | Splash Cymbal      |
| 56  | G#3  | Cowbell            |
| 57  | A3   | Crash Cymbal 2     |
| 58  | A#3  | Vibraslap          |
| 59  | B3   | Ride Cymbal 2      |
| 60  | C4   | Hi Bongo           |
| 61  | C#4  | Low Bongo          |
| 62  | D4   | Mute Hi Conga      |
| 63  | D#4  | Open Hi Conga      |
| 64  | E4   | Low Conga          |
| 65  | F4   | High Timbale       |
| 66  | F#4  | Low Timbale        |
| 67  | G4   | High Agogo         |
| 68  | G#4  | Low Agogo          |
| 69  | A4   | Cabasa             |
| 70  | A#4  | Maracas            |
| 71  | B4   | Short Whistle      |
| 72  | C5   | Long Whistle       |
| 73  | C#5  | Short Guiro        |
| 74  | D5   | Long Guiro         |
| 75  | D#5  | Claves             |
| 76  | E5   | Hi Wood Block      |
| 77  | F5   | Low Wood Block     |
| 78  | F#5  | Mute Cuica         |
| 79  | G5   | Open Cuica         |
| 80  | G#5  | Mute Triangle      |
| 81  | A5   | Open Triangle      |

`1000nnnn0kkkkkkk0vvvvvvv` is note off event where nnnn is channel number (0 - 15), `kkkkkkk` is key (0 to 127), `vvvvvvv` is velocity (0 - 127).

`1001nnnn0kkkkkkk0vvvvvvv` is note on event where nnnn is channel number (0 - 15), `kkkkkkk` is key (0 to 127), `vvvvvvv` is velocity (0 - 127).

So, to send Bass Drum 1 with velocity 90 percent, MIDI controller will send `1001100100010010001110010`

where first `1001` represent note on, second `1001` represent channel number (channel 10), `00100100` represent Bass Drum 1 code (36), and `01110010` represent velocity 114 (90 percent).

To send note off event, MIDI controller can send `1000100100010010001110010`.

## Materials Needed

We need an ESP32 expansion board so that we have enough pins for the piezoelectric sensor with minimal effort. Each piezoelectric sensor will be connected to the expansion board with two cables. Because GND and analog input are separated by VCC, we need 3x1 dupont headers for each piezoelectric sensor.

![Image1](https://github.com/kamshory/ESP32DrumPad/blob/main/images/esp32-expansion-board-1.jpg)

![Image2](https://github.com/kamshory/ESP32DrumPad/blob/main/images/esp32-expansion-board-2.jpg)

![Image3](https://github.com/kamshory/ESP32DrumPad/blob/main/images/piezoelectric-sensor.jpg)

We do not need the VCC pin to supply power to the piezoelectric sensor because the piezoelectric sensor will generate an electrical voltage of itself when it receives enough vibrations. Thus, we can remove the VCC jumper pin. However, if we want to add another device, for example a sound generator that will be integrated with the MIDI controller that we are going to make, we can choose to use a jumper and set it according to the voltage we need.

We still use the built-in USB on the ESP32 board to connect it to the PC because we will use this USB as a MIDI controller. The USB on the expansion board is only used as a power supply if needed or in scenarios using BLE (Bluetooth Low Energy) to transmit MIDI signals to the PC.

## Wiring

| GPIO | Input     | Output | Pad CH | Notes                                                                          |
| ---- | --------- | ------ | ------ | ------------------------------------------------------------------------------ |
| 0    | pulled up | OK     |        | outputs PWM signal at boot, must be LOW to enter flashing mode                 |
| 1    | TX pin    | OK     |        | debug output at boot                                                           |
| 2    | OK        | OK     | CH 14  | connected to on-board LED, must be left floating or LOW to enter flashing mode |
| 3    | OK        | RX pin |        | HIGH at boot                                                                   |
| 4    | OK        | OK     | CH 15  |                                                                                |
| 5    | OK        | OK     |        | outputs PWM signal at boot, strapping pin                                      |
| 6    | x         | x      |        | connected to the integrated SPI flash                                          |
| 7    | x         | x      |        | connected to the integrated SPI flash                                          |
| 8    | x         | x      |        | connected to the integrated SPI flash                                          |
| 9    | x         | x      |        | connected to the integrated SPI flash                                          |
| 10   | x         | x      |        | connected to the integrated SPI flash                                          |
| 11   | x         | x      |        | connected to the integrated SPI flash                                          |
| 12   | OK        | OK     | CH 11  | boot fails if pulled high, strapping pin                                       |
| 13   | OK        | OK     | CH 12  |                                                                                |
| 14   | OK        | OK     | CH 10  | outputs PWM signal at boot                                                     |
| 15   | OK        | OK     | CH 13  | outputs PWM signal at boot, strapping pin                                      |
| 16   | OK        | OK     |        | (U2_RXD)                                                                       |
| 17   | OK        | OK     |        | (U2_TXD)                                                                       |
| 18   | OK        | OK     |        | (SCK / VSPI_SCL)                                                               |
| 19   | OK        | OK     |        | (MISO)                                                                         |
| 21   | OK        | OK     |        | (SDA)                                                                          |
| 22   | OK        | OK     |        | (SCL)                                                                          |
| 23   | OK        | OK     |        | (MOSI)                                                                         |
| 25   | OK        | OK     | CH 7   |                                                                                |
| 26   | OK        | OK     | CH 8   |                                                                                |
| 27   | OK        | OK     | CH 9   |                                                                                |
| 32   | OK        | OK     | CH 5   |                                                                                |
| 33   | OK        | OK     | CH 6   |                                                                                |
| 34   | OK        |        | CH 3   | input only                                                                     |
| 35   | OK        |        | CH 4   | input only                                                                     |
| 36   | OK        |        | CH 1   | input only                                                                     |
| 39   | OK        |        | CH 2   | input only                                                                     |

![Wiring](https://github.com/kamshory/ESP32DrumPad/blob/main/images/wiring.drawio.svg)

The ESP32 WROOM-32 only allows us to create drum pads with 15 channels. This is because there are only 15 pins that can function as ADC (Analog to Digital Converter). However, our application allows the use of an ESP32 S3 which has a 16 pin ADC.

When using the ESP32 WROOM-32, the application must be limited so that it does not scan pin 16, causing the application to become unstable. This setting is hard coded in the application before flashing it to the ESP32.

Keep in mind that GPIO 2 is connected to the onboard LED. The onboard LED can no longer be used as an indicator because it is already used as an ADC input. In addition, reading the ADC input must also be done after the ESP32 has finished booting. We also have to remove the onboard LED so that GPIO 2 can be used effectively for the ADC thereby preventing a voltage drop entering the ADC and causing reading errors.

## Pad Pin Mapping

### Full pin Usage

| Pad | Pin     |
| --- | ------- |
| 1   | GPIO 36 |
| 2   | GPIO 39 |
| 3   | GPIO 34 |
| 4   | GPIO 35 |
| 5   | GPIO 32 |
| 6   | GPIO 33 |
| 7   | GPIO 25 |
| 8   | GPIO 26 |
| 9   | GPIO 27 |
| 10  | GPIO 14 |
| 11  | GPIO 12 |
| 12  | GPIO 13 |
| 13  | GPIO 15 |
| 14  | GPIO 2  |
| 15  | GPIO 4  |

### Minimum Pin Usage

| Pad | Pin     |
| --- | ------- |
| 1   | GPIO 36 |
| 2   | GPIO 39 |
| 3   | GPIO 34 |
| 4   | GPIO 35 |
| 5   | GPIO 32 |
| 6   | GPIO 33 |
| 7   | GPIO 25 |
| 8   | GPIO 26 |
| 9   | GPIO 27 |
| 10  | GPIO 14 |
| 11  | GPIO 12 |
| 12  | GPIO 13 |

With 12 pads, we can use GPIO 2 as a system indicator and don't need to remove the onboard LED.

## EEPROM Data Structure

The MIDI controller configuration is stored in EEPROM which ensures that the configuration is maintained even if the MIDI controller is not powered. This data will be read every time the MIDI controller is turned on and when the configuration is required by the software. The configuration will be rewritten to EEPROM every time the user makes configuration changes.

### EEPROM Data Offset

Each channel has an EEPROM address allocation to store the channel configuration. Even though the perchannel configuration data type is string, this data is divided into several segments that have different data types.

We only have 512 bytes (4096 bits) of EEPROM space as storage media. We have to manage it so that it can accommodate dozens of data of various types and sizes. This allocation is not ideal for normal use.

With the internal EEPROM built in on the ESP32, we can map data addresses as follows:

| Data                   | Offset | Length | Type       | Description                         |
| ---------------------- | ------ | ------ | ---------- | ----------------------------------- |
| offsetAPSSID           | 0      | 30     | String     | SSID AP                             |
| offsetAPSSIDPassword   | 30     | 30     | String     | Password AP                         |
| offsetAPIP             | 60     | 4      | IPAddress  | IP Address AP                       |
| offsetAPGateway        | 64     | 4      | IPAddress  | Gateway AP                          |
| offsetAPSubnet         | 68     | 4      | IPAddress  | Subnet AP                           |
| offsetAPHidden         | 72     | 1      | Byte       | Hidden AP                           |
| offsetWifiEnable       | 73     | 1      | Byte       | Enable Wifi                         |
| offsetWifiSSID         | 74     | 30     | String     | SSID Wifi                           |
| offsetWifiSSIDPassword | 104    | 30     | String     | Password Wifi                       |
| offsetWSEnable         | 134    | 1      | Byte       | Enable WS                           |
| offsetWSScheme         | 135    | 5      | String     | Scheme WS                           |
| offsetWSHost           | 140    | 40     | String     | Host WS                             |
| offsetWSPort           | 180    | 2      | Word       | Port WS                             |
| offsetWSPath           | 182    | 50     | String     | Path WS                             |
| offsetWSUsername       | 232    | 20     | String     | Username WS                         |
| offsetWSPassword       | 252    | 20     | String     | Password WS                         |
| offsetConfigured       | 272    | 1      | Byte       | Flag that controller was configured |
| offsetSolo             | 273    | 1      | Byte       | Flag solo channel                   |
| offsetSoloChannel      | 274    | 1      | Byte       | Solo channel number                 |
| offsetMCUser           | 275    | 20     | String     | MIDI Controller username            |
| offsetMCPassword       | 295    | 20     | String     | MIDI Controller password            |
| midiChannel            | 315    | 1      | Byte       | MIDI channel                        |
| readInterval           | 316    | 4      | DWord      | Read analog interval                |
| offsetCh1              | 320    | 10     | String     | Channel 1                           |
| offsetCh2              | 330    | 10     | String     | Channel 2                           |
| offsetCh3              | 340    | 10     | String     | Channel 3                           |
| offsetCh4              | 350    | 10     | String     | Channel 4                           |
| offsetCh5              | 360    | 10     | String     | Channel 5                           |
| offsetCh6              | 370    | 10     | String     | Channel 6                           |
| offsetCh7              | 380    | 10     | String     | Channel 7                           |
| offsetCh8              | 390    | 10     | String     | Channel 8                           |
| offsetCh9              | 400    | 10     | String     | Channel 9                           |
| offsetCh10             | 410    | 10     | String     | Channel 10                          |
| offsetCh11             | 420    | 10     | String     | Channel 11                          |
| offsetCh12             | 430    | 10     | String     | Channel 12                          |
| offsetCh13             | 440    | 10     | String     | Channel 13                          |
| offsetCh14             | 450    | 10     | String     | Channel 14                          |
| offsetCh15             | 460    | 10     | String     | Channel 15                          |
| offsetCh16             | 470    | 10     | String     | Channel 16                          |
| offsetTestVelocity     | 480    | 1      | Byte       | Channel 16                          |

So far we do not need additional storage media.

If we want ideal space for storage, we can add an external EEPROM module connected to the ESP32 via SPI or I2C. For example, the AT24C256 Module can provide a much larger storage capacity of up to 256 kilo bytes with I2C communication.

![AT24C256](https://github.com/kamshory/ESP32DrumPad/blob/main/images/at24C256.png)

The wiring of AT24C256 is as follows:

| AT24C256 | ESP32         |
| -------- | ------------- |
| VCC      | VIN 3.3V      |
| GND      | GND           |
| SCL      | SCL (GPIO 22) |
| SDA      | SDA (GPIO 21) |

We can use 8 AT24C256 simultaneously on the same bus. By configuring the address pins, we can determine the AT24C256 addresses ranging from 80 to 87 with the following configuration:

| A2 | A1 | A0 | Hexadecimal | Decimal |
| -- | -- | -- | ----------- | ------- |
| 0  | 0  | 0  | 50          | 80      |
| 0  | 0  | 1  | 51          | 81      |
| 0  | 1  | 0  | 52          | 82      |
| 0  | 1  | 1  | 53          | 83      |
| 1  | 0  | 0  | 54          | 84      |
| 1  | 0  | 1  | 55          | 85      |
| 1  | 1  | 0  | 56          | 86      |
| 1  | 1  | 1  | 57          | 87      |

The AT24C128/256 has a hardware data protection scheme that allows the user to write protect the whole memory when the WP pin is at VCC.

### Channel Configuration

| Offset | Length | Type  | Range        | Usage                     |
| ------ | ------ | ----- | ------------ | ------------------------- |
| 0      | 1      | Byte  | 0-127        | Configuration flag        |
| 1      | 1      | Byte  | 0-127        | Instrument code           |
| 2      | 2      | Word  | 0-65535      | Threshold (minimum value) |
| 4      | 2      | Word  | 0-65535      | Head room                 |
| 6      | 4      | DWord | 0-4294967295 | Duration (micro second)   |

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

The sampling interval must be precise. A small interval will reduce the latency between the time the pad is hit and the time the note ON signal is sent. Conversely, if the interval is too small, then the ESP32 cannot capture the maximum voltage which represents the player's speed when hitting the pad.

The duration must be set in such a way that when the next voltage is read, the voltage value is below the threshold. This is intended to prevent the ESP32 from sending the ON note twice when the player hits the pad once. Thus, the number of note ON signals sent by the ESP32 is equal to the number of pad strokes made by the player.

Default values which are test results must be provided by the application.

![Velocity](https://github.com/kamshory/ESP32DrumPad/blob/main/images/velocity.drawio.svg)

The ESP32 takes several voltage samples and then averages them. This average value is then converted into velocity.

The average voltage is not the peak voltage but rather the average value taken from several samples when a voltage above the threshold is detected. This voltage may be far from the peak value depending on the sampling time. If the sampling time is close to the peak voltage, then the data-average voltage will be close to the peak voltage value. On the other hand, if the sampling time is far from the peak voltage, then the average data voltage will be far from the peak voltage value.

In the example above, it is known that the threshold is equal to 383 and the head room is equal to 2059. The voltage that will be converted to velocity is above 0.309450549 volts while voltages below 0.309450549 volts will be ignored. Meanwhile, any voltage will be reduced by 0.309450549 volts. A voltage of 2.0625 volts will be converted to a velocity of 127 while a voltage of 0.309450549 volts will be converted to a velocity of 0. The relationship between voltage and velocity is as follows:

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
    return (uint8_t) round(outp2);
}
```

### Duration

Duration in general is the time calculated from the `note on` signal to the next `note off` signal on the same channel and instrument or note. When it comes to drum pads, duration is very important because this will affect the speed at which the player plays the drums. The default value of the duration must be provided by the application.

# TFT Display

Drum pads can be equipped with TFT displays that allow users to see the status of the drum pad. However, the display is not an important feature considering the limited resources of the ESP 32.

Some modules that can be used for displays are as follows:

| Module  | Com | Resolution | Color | Touch | SD Card |
| ------- | --- | ---------- | ----- | ----- | ------- |
| OLED    | IIC | 128 x 64   | White | No    | No      |
| ST7735  | SPI | 128 x 160  | RGB   | No    | Yes     |
| ili9341 | SPI | 240 x 320  | RGB   | Yes   | Yes     |


## Wiring

### OLED

| OLED     | ESP32         |
| -------- | ------------- |
| VCC      | VIN 3.3V      |
| GND      | GND           |
| SCL      | SCL (GPIO 22) |
| SDA      | SDA (GPIO 21) |

### ST7735

| ST7735   | ESP32          |
| -------- | -------------- |
| LED      | VIN 3.3V       |
| SCK      | CLK (GPIO 18)  |
| SDA      | MOSI (GPIO 23) |
| A0       | GPIO 4         |
| RESET    | RESET          |
| CS       | CSO (GPIO 5)   |
| GND      | GND            |
| VCC      | VIN 3.3V       |

### ili9341

| ili9341  | ESP32          |
| -------- | -------------- |
| LED      | VIN 3.3V       |
| SCK      | CLK (GPIO 18)  |
| MOSI     | MOSI (GPIO 23) |
| MISO     | MISO (GPIO 19) |
| D/C      | GPIO 4         |
| RESET    | VIN 3.3V       |
| CS       | CSO (GPIO 5)   |
| GND      | GND            |
| VCC      | VIN 3.3V       |

# Need Support

Please support our YouTube channel by subsribe it at https://www.youtube.com/@MusicPlanetbiru
