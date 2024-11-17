#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <MIDI.h>

int onboardLED = 2;
char *ssid = "PlanetbiruMusic";
char *password = "PlanetbiruMusic";
char *ssid2 = "PlanetbiruMusic";
char *password2 = "PlanetbiruMusic";

int sizeofString50 = 50;
int sizeofString20 = 20;
int sizeofInteger15 = 15;
int sizeofInteger10 = 10;
int sizeofBoolean = 1;

int offsetAPSSID = 0;             //	30	String	SSID AP
int offsetAPSSIDPassword = 30;    //	30	String	Password AP
int offsetAPIP = 60;              //	4	IPAddress	IP Address AP
int offsetAPGateway = 64;         //	4	IPAddress	Gateway AP
int offsetAPSubnet = 68;          //	4	IPAddress	Subnet AP
int offsetAPHidden = 72;          //	1	Byte	Hidden AP
int offsetWifiEnable = 73;        //	1	Byte	Enable Wifi
int offsetWifiSSID = 74;          //	30	String	SSID Wifi
int offsetWifiSSIDPassword = 104; //	30	String	Password Wifi
int offsetWSEnable 134;           //	1	Byte	Enable WS
int offsetWSScheme = 135;         //	5	String	Scheme WS
int offsetWSHost = 140;           //	40	String	Host WS
int offsetWSPort = 180;           //	2	Word	Port WS
int offsetWSPath = 182;           //	50	String	Path WS
int offsetWSUsername = 232;       //	20	String	Username WS
int offsetWSPassword = 252;       //	20	String	Password WS
int offsetConfigured = 272;       //	1	Byte	Flag that controller was configured
int offsetSolo = 273;             //	1	Byte	Flag solo channel
int offsetSoloChannel = 274;      //	1	Byte	Solo channel number
int offsetMCUser = 275;           //	20	String	MIDI Controller username
int offsetMCPassword = 295;       //	20	String	MIDI Controller password
int offsetMidiChannel = 315;      //	1	Byte	MIDI channel
int offsetReadInterval = 316;     //	4	DWord	Read analog interval
int offsetCh1 = 320;              //	10	String	Channel 1
int offsetCh2 = 330;              //	10	String	Channel 2
int offsetCh3 = 340;              //	10	String	Channel 3
int offsetCh4 = 350;              //	10	String	Channel 4
int offsetCh5 = 360;              //	10	String	Channel 5
int offsetCh6 = 370;              //	10	String	Channel 6
int offsetCh7 = 380;              //	10	String	Channel 7
int offsetCh8 = 390;              //	10	String	Channel 8
int offsetCh9 = 400;              //	10	String	Channel 9
int offsetCh10 = 410;             //	10	String	Channel 10
int offsetCh11 = 420;             //	10	String	Channel 11
int offsetCh12 = 430;             //	10	String	Channel 12
int offsetCh13 = 440;             //	10	String	Channel 13
int offsetCh14 = 450;             //	10	String	Channel 14
int offsetCh15 = 460;             //	10	String	Channel 15
int offsetCh16 = 470;             //	10	String	Channel 16
int offsetTestVelocity = 480;     //	1	byte	Test velocity

int memOffset = offsetCh1;
int memSize = 10;

int solo = 0;
int soloChannel == 0;

String savedWSPath = "";
String savedWSUsername = "";
String savedWSPassword = "";
String savedWSTopic = "";
String savedWSHost = "";
String savedWSPort = "";
String savedEnable = "";

String sysEnable = "0";
boolean connected = false;
boolean lastState = false;
long lastDisconnected = millis();
long reconnectWiFiTreshold = 10000;

const char *gTopic = "";
const char *gMessage = "";
String ssidUsed = "";

// ESP32 has only 12 pin ADC but we will only develop MIDI controller with 12 pads
int maxCh = 12;

// we need maxCh + 1 because channel begin with 1
//                              1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
byte channelInstrument[] = {0, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50};
//                                  1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15, 16
uint16_t channelHeadRoom[] = {0, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};

//                                  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
uint16_t channelThreshold[] = {0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

//                      1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
int channelPin[] = {0, 36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 4, 2, 15, 0};

//                       1, 2, 3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
int channelDelay[] = {0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

int solo = 0;
int soloChannel = 0;
int testVelocity = 0;

MIDI_CREATE_DEFAULT_INSTANCE();

/**
 * @class ChannelConfig
 * @brief Represents the configuration for a specific MIDI channel, storing parameters like instrument code, threshold, scale, and duration.
 * 
 * This class is used to manage and read/write the configuration of individual MIDI channels. The configuration includes:
 * - `instrumentCode`: The MIDI instrument code used by the channel.
 * - `threshold`: The sensitivity threshold for triggering events.
 * - `scale`: A scale factor to adjust the event.
 * - `duration`: The duration of the event, typically in milliseconds.
 * 
 * The configuration can be read from EEPROM based on a specific channel index.
 */
class ChannelConfig
{
    /* data */
public:
    byte configured;
    byte instrumentCode;
    uint16_t threshold;
    uint16_t scale;
    uint32_t duration;

    ChannelConfig()
    {
        instrumentCode = 0;
        threshold = 0;
        scale = 1;
        duration = 1;
    }

    ChannelConfig(byte c, byte i, uint16_t t, uint16_t s, uint32_t d)
    {
        configured = c;
        instrumentCode = i;
        threshold = t;
        scale = s;
        duration = d;
    }

    ChannelConfig(int channel)
    {
        int offset = getChannelOffset(channel);
        byte c = readByte(offset);
        byte i = readByte(offset + 1);
        uint16_t t = readWord(offset + 2);
        uint16_t s = readWord(offset + 4);
        uint32_t d = readDoubleWord(offset + 6);

        configured = c;
        instrumentCode = i;
        threshold = t;
        scale = s;
        duration = d;
    }

    void readChannelConfig(int channel)
    {
        int offset = getChannelOffset(channel);
        byte configured2 = readByte(offset);
        byte instrumentCode2 = readByte(offset + 1);
        uint16_t threshold2 = readWord(offset + 2);
        uint16_t scale2 = readWord(offset + 4);
        uint32_t duration2 = readDoubleWord(offset + 6);

        // add code here
        return channelConfig(configured2, instrumentCode2, threshold2, scale2, duration2);
    }
};

IPAddress apLocalID(192, 168, 4, 1);
IPAddress apGateway(192, 168, 4, 2);
IPAddress apSubnet(255, 255, 255, 0);
WebServer server(80);
WebSocketsClient webSocket;

/**
 * @brief Task1 - Handles the main task execution.
 * 
 * This function runs the primary task for the device. It is a FreeRTOS task
 * and will execute in parallel with other tasks.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Task1(void *pvParameters);

/**
 * @brief Task2 - Handles the secondary task execution.
 * 
 * This function runs the secondary task for the device. It is a FreeRTOS task
 * and will execute in parallel with other tasks.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Task2(void *pvParameters);

/**
 * @brief Listen01 - Listens for input on channel 1.
 * 
 * This function is used to listen to input from a device or sensor on channel 1.
 * It runs as a FreeRTOS task and processes data for channel 1.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen01(void *pvParameters);

/**
 * @brief Listen02 - Listens for input on channel 2.
 * 
 * This function is used to listen to input from a device or sensor on channel 2.
 * It runs as a FreeRTOS task and processes data for channel 2.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen02(void *pvParameters);

/**
 * @brief Listen03 - Listens for input on channel 3.
 * 
 * This function is used to listen to input from a device or sensor on channel 3.
 * It runs as a FreeRTOS task and processes data for channel 3.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen03(void *pvParameters);

/**
 * @brief Listen04 - Listens for input on channel 4.
 * 
 * This function is used to listen to input from a device or sensor on channel 4.
 * It runs as a FreeRTOS task and processes data for channel 4.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen04(void *pvParameters);

/**
 * @brief Listen05 - Listens for input on channel 5.
 * 
 * This function is used to listen to input from a device or sensor on channel 5.
 * It runs as a FreeRTOS task and processes data for channel 5.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen05(void *pvParameters);

/**
 * @brief Listen06 - Listens for input on channel 6.
 * 
 * This function is used to listen to input from a device or sensor on channel 6.
 * It runs as a FreeRTOS task and processes data for channel 6.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen06(void *pvParameters);

/**
 * @brief Listen07 - Listens for input on channel 7.
 * 
 * This function is used to listen to input from a device or sensor on channel 7.
 * It runs as a FreeRTOS task and processes data for channel 7.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen07(void *pvParameters);

/**
 * @brief Listen08 - Listens for input on channel 8.
 * 
 * This function is used to listen to input from a device or sensor on channel 8.
 * It runs as a FreeRTOS task and processes data for channel 8.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen08(void *pvParameters);

/**
 * @brief Listen09 - Listens for input on channel 9.
 * 
 * This function is used to listen to input from a device or sensor on channel 9.
 * It runs as a FreeRTOS task and processes data for channel 9.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen09(void *pvParameters);

/**
 * @brief Listen10 - Listens for input on channel 10.
 * 
 * This function is used to listen to input from a device or sensor on channel 10.
 * It runs as a FreeRTOS task and processes data for channel 10.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen10(void *pvParameters);

/**
 * @brief Listen11 - Listens for input on channel 11.
 * 
 * This function is used to listen to input from a device or sensor on channel 11.
 * It runs as a FreeRTOS task and processes data for channel 11.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen11(void *pvParameters);

/**
 * @brief Listen12 - Listens for input on channel 12.
 * 
 * This function is used to listen to input from a device or sensor on channel 12.
 * It runs as a FreeRTOS task and processes data for channel 12.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen12(void *pvParameters);

/**
 * @brief Listen13 - Listens for input on channel 13.
 * 
 * This function is used to listen to input from a device or sensor on channel 13.
 * It runs as a FreeRTOS task and processes data for channel 13.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen13(void *pvParameters);

/**
 * @brief Listen14 - Listens for input on channel 14.
 * 
 * This function is used to listen to input from a device or sensor on channel 14.
 * It runs as a FreeRTOS task and processes data for channel 14.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen14(void *pvParameters);

/**
 * @brief Listen15 - Listens for input on channel 15.
 * 
 * This function is used to listen to input from a device or sensor on channel 15.
 * It runs as a FreeRTOS task and processes data for channel 15.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen15(void *pvParameters);

/**
 * @brief Listen16 - Listens for input on channel 16.
 * 
 * This function is used to listen to input from a device or sensor on channel 16.
 * It runs as a FreeRTOS task and processes data for channel 16.
 *
 * @param pvParameters The parameters passed to the task (if any).
 */
void Listen16(void *pvParameters);


/**
 * @brief Calculates a velocity value based on input, threshold, and headroom values.
 * 
 * This function calculates the velocity by subtracting the threshold from the input if the input is greater than or equal to the threshold. It then scales the result relative to the headroom and threshold. The output is clamped at 127 if it exceeds this value, and the final result is rounded to the nearest integer.
 * 
 * @param inp The input value representing the raw velocity.
 * @param thd The threshold value to compare the input against.
 * @param headRoom The headroom value used for scaling the result.
 * @return uint16_t The calculated velocity value, clamped to the range [0, 127].
 */
int calcVelocity(uint16_t inp, uint16_t thd, uint16_t headRoom)
{
    uint16_t inp2 = inp >= thd ? inp - thd : 0;
    float outp2 = 127 * (float)inp2 / (float)(headRoom - thd);
    if (outp2 > 127)
    {
        outp2 = 127;
    }
    return (uint16_t)round(outp2);
}

/**
 * @brief Loads MIDI configuration for all channels.
 * 
 * This function iterates through all available channels, loading the configuration for each channel if it is configured. It updates the corresponding channel instrument, threshold, headroom, and delay values based on the configuration.
 */
void loadMidiConfig()
{
    for (int channel = 1; channel <= maxCh; channel++)
    {
        ChannelConfig cc(channel);
        if (cc.configured)
        {
            channelInstrument[channel] = cc.instrumentCode;
            channelThreshold[channel] = cc.threshold;
            channelHeadRoom[channel] = cc.scale;
            channelDelay[channel] = cc.duration;
        }
    }
}

/**
 * @brief Decodes a URL-encoded string.
 * 
 * This function takes a URL-encoded string and decodes it by converting percent-encoded characters (%XX) into their respective ASCII values and converting '+' symbols into spaces.
 * 
 * @param str The URL-encoded string to decode.
 * @return String The decoded string.
 */
String urlDecode(String str)
{
    String encodedString = "";
    char c;
    char code0;
    char code1;
    for (int i = 0; i < str.length(); i++)
    {
        c = str.charAt(i);
        if (c == '+')
        {
            encodedString += ' ';
        }
        else if (c == '%')
        {
            i++;
            code0 = str.charAt(i);
            i++;
            code1 = str.charAt(i);
            c = (h2int(code0) << 4) | h2int(code1);
            encodedString += c;
        }
        else
        {
            encodedString += c;
        }
        yield();
    }
    return encodedString;
}

/**
 * @brief URL-encodes a string.
 * 
 * This function takes a regular string and converts it to a URL-encoded string by replacing spaces with '+', and encoding special characters as percent-encoded values.
 * 
 * @param str The string to URL-encode.
 * @return String The URL-encoded string.
 */
String urlEncode(String str)
{
    String encodedString = "";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i = 0; i < str.length(); i++)
    {
        c = str.charAt(i);
        if (c == ' ')
        {
            encodedString += '+';
        }
        else if (isalnum(c))
        {
            encodedString += c;
        }
        else
        {
            code1 = (c & 0xf) + '0';
            if ((c & 0xf) > 9)
            {
                code1 = (c & 0xf) - 10 + 'A';
            }
            c = (c >> 4) & 0xf;
            code0 = c + '0';
            if (c > 9)
            {
                code0 = c - 10 + 'A';
            }
            code2 = '\0';
            encodedString += '%';
            encodedString += code0;
            encodedString += code1;
            // encodedString+=code2;
        }
        yield();
    }
    return encodedString;
}

/**
 * @brief Converts a hexadecimal character to its integer equivalent.
 * 
 * This function takes a single hexadecimal character ('0'-'9', 'a'-'f', 'A'-'F') and returns its integer value.
 * 
 * @param c The hexadecimal character to convert.
 * @return unsigned char The integer value corresponding to the hexadecimal character.
 */
unsigned char h2int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return ((unsigned char)c - '0');
    }
    if (c >= 'a' && c <= 'f')
    {
        return ((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <= 'F')
    {
        return ((unsigned char)c - 'A' + 10);
    }
    return (0);
}

/**
 * @brief Writes an IP address to EEPROM.
 * 
 * This function splits an IP address string into its individual octets and writes each octet to EEPROM at the specified address.
 * 
 * @param address The EEPROM address to start writing from.
 * @param ip The IP address to write (in string format).
 */
void writeIp(int address, String ip)
{
    if (countSplitCharacters(ip, '.') == 4)
    {
        String[] value = splitString(ip, '.');
        byte one = value[0].toInt() & 0xFF;
        byte two = value[1].toInt() & 0xFF;
        byte three = value[2].toInt() & 0xFF;
        byte four = value[3].toInt() & 0xFF;
        EEPROM.write(address, one);
        EEPROM.write(address + 1, two);
        EEPROM.write(address + 2, three);
        EEPROM.write(address + 3, four);
        EEPROM.commit();
    }
}

/**
 * @brief Reads an IP address from EEPROM.
 * 
 * This function reads the four octets of an IP address stored in EEPROM and returns the address as a string.
 * 
 * @param address The EEPROM address to start reading from.
 * @return String The IP address as a string.
 */
String readIp(int address)
{
    byte one = EEPROM.read(address) & 0xFF;
    byte two = EEPROM.read(address + 1) & 0xFF;
    byte three = EEPROM.read(address + 2) & 0xFF;
    byte four = EEPROM.read(address + 3) & 0xFF;
    return sprintf("%d.%d.%d.%d", one, two, three, four);
}

/**
 * @brief Writes a byte value to EEPROM.
 * 
 * This function writes a single byte value to EEPROM at the specified address.
 * 
 * @param address The EEPROM address to write to.
 * @param value The byte value to write to EEPROM.
 */
void writeByte(int address, uint8_t value)
{
    byte one = (value & 0xFF);
    EEPROM.write(address, one);
    EEPROM.commit();
}

/**
 * @brief Reads a byte value from EEPROM.
 * 
 * This function reads a single byte value from EEPROM at the specified address.
 * 
 * @param address The EEPROM address to read from.
 * @return byte The byte value read from EEPROM.
 */
byte readByte(int address)
{
    byte one = EEPROM.read(address);
    return (one & 0xFF);
}

/**
 * @brief Writes a 16-bit word value to EEPROM.
 * 
 * This function writes a 16-bit word (2 bytes) value to EEPROM at the specified address.
 * 
 * @param address The EEPROM address to start writing from.
 * @param value The 16-bit word value to write to EEPROM.
 * @return uint16_t The written word value.
 */
uint16_t writeWord(int address, uint16_t value)
{
    byte two = ((value >> 16) & 0xFF);
    byte one = ((value >> 24) & 0xFF);

    EEPROM.write(address, two);
    EEPROM.write(address + 1, one);
    EEPROM.commit();
}

/**
 * @brief Reads a 16-bit word value from EEPROM.
 * 
 * This function reads a 16-bit word (2 bytes) value from EEPROM at the specified address.
 * 
 * @param address The EEPROM address to start reading from.
 * @return uint16_t The 16-bit word value read from EEPROM.
 */
uint16_t readWord(int address)
{
    byte two = EEPROM.read(address);
    byte one = EEPROM.read(address + 1);

    return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}

/**
 * @brief Writes a 32-bit double word value to EEPROM.
 * 
 * This function writes a 32-bit double word (4 bytes) value to EEPROM at the specified address.
 * 
 * @param address The EEPROM address to start writing from.
 * @param value The 32-bit double word value to write to EEPROM.
 */
void writeDoubleWord(int address, uint32_t value)
{
    byte four = (value & 0xFF);
    byte three = ((value >> 8) & 0xFF);
    byte two = ((value >> 16) & 0xFF);
    byte one = ((value >> 24) & 0xFF);

    EEPROM.write(address, four);
    EEPROM.write(address + 1, three);
    EEPROM.write(address + 2, two);
    EEPROM.write(address + 3, one);
    EEPROM.commit();
}

/**
 * @brief Reads a 32-bit double word value from EEPROM.
 * 
 * This function reads a 32-bit double word (4 bytes) value from EEPROM at the specified address.
 * 
 * @param address The EEPROM address to start reading from.
 * @return uint32_t The 32-bit double word value read from EEPROM.
 */
uint32_t readDoubleWord(int address)
{
    byte four = EEPROM.read(address);
    byte three = EEPROM.read(address + 1);
    byte two = EEPROM.read(address + 2);
    byte one = EEPROM.read(address + 3);

    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

/**
 * @brief Writes a string value to EEPROM.
 * 
 * This function writes a string of specified length to EEPROM, padding with null characters if the string is shorter than the specified length.
 * 
 * @param offset The starting address in EEPROM to write to.
 * @param length The total length to write (including padding if necessary).
 * @param value The string to write to EEPROM.
 */
void writeString(int offset, int length, String value)
{
    int max2 = length;
    int max1 = value.length();
    String result = "";
    for (int l = 0; l < max1; ++l)
    {
        EEPROM.write(offset + l, value[l]);
    }
    for (int l = max1; l < max2; ++l)
    {
        EEPROM.write(offset + l, 0);
    }
    EEPROM.commit();
}

/**
 * @brief Reads a specified amount of data from EEPROM.
 * 
 * This function reads a specified number of characters from EEPROM starting at a given offset and returns the result as a string.
 * 
 * @param offset The starting address in EEPROM to begin reading from.
 * @param length The number of characters to read.
 * @return String The string value read from EEPROM.
 */
String readData(int offset, int length)
{
    int max = offset + length;
    String result = "";
    for (int l = offset; l < max; ++l)
    {
        char chr = EEPROM.read(l);
        if (chr == 0)
        {
            break;
        }
        result += char(chr);
    }
    result += char('\0');
    return result;
}

/**
 * @brief Reads a string from EEPROM.
 * 
 * This function reads a string of characters from EEPROM starting at a specified offset, stopping when a null character is encountered or when the specified length is reached.
 * 
 * @param offset The starting address in EEPROM to begin reading from.
 * @param length The maximum number of characters to read.
 * @return String The string value read from EEPROM.
 */
String readString(int offset, int length)
{
    int max = offset + length;
    String result = "";
    for (int l = offset; l < max; ++l)
    {
        char chr = EEPROM.read(l);
        if (chr == 0)
        {
            break;
        }
        result += char(chr);
    }
    return result;
}

/**
 * @brief Retrieves the AP (Access Point) configuration data.
 * 
 * This function reads the saved WiFi SSID, SSID password, IP address, gateway, subnet, and hidden network status from EEPROM. It then formats these values into a JSON response and sends it to the client.
 * 
 * @note The JSON response includes the fields "ssid_name", "ssid_password", "ip", "gateway", "subnet", and "hidden".
 * 
 * @path /ap-configuration.json
 */
void getAPData()
{
    String savedSSID = readString(offsetSSID1, sizeofString30);
    String savedSSIDPassword = readString(offsetSSIDPassword1, sizeofString30);
    String savedIP = readIp(offsetAPIP);
    String savedGateway = readIp(offsetAPGateway);
    String savedSubnet = readIp(offsetAPSubnet);
    String savedHidden = readByte(offsetAPHidden);
    String response = "";
    response += "{\"ssid_name\":\"";
    response += savedSSID;
    response += "\", \"ssid_password\":\"";
    response += savedSSIDPassword;
    response += "\", \"ip\":\"";
    response += savedIP;
    response += "\", \"gateway\":\"";
    response += savedGateway;
    response += "\", \"subnet\":\"";
    response += savedSubnet;
    response += "\", \"hidden\":\"";
    response += savedHidden;
    response += "\"}";
    server.send(200, "application/json", response);
}

/**
 * @brief Saves the AP (Access Point) configuration data.
 * 
 * This function accepts POST requests and saves the provided SSID, SSID password, IP address, gateway, subnet, and hidden network status to EEPROM. The configuration values are extracted from the HTTP POST parameters.
 * 
 * @note The configuration values are stored in EEPROM at predefined offsets.
 * 
 * @path /save-ap.html
 */
void saveConfigAP()
{
    if (server.method() == HTTP_POST)
    {
        String savedSSID = server.arg("ssid_name");
        writeString(offsetSSID1, sizeofString30, savedSSID);
        String savedSSIDPassword = server.arg("ssid_password");
        writeString(offsetSSIDPassword1, sizeofString30, savedSSIDPassword);
        String savedIP = server.arg("ip");
        writeIp(offsetAPIP, savedIP);
        String savedGateway = server.arg("gateway");
        writeIp(offsetAPGateway, savedGateway);
        String savedSubnet = server.arg("subnet");
        writeIp(offsetAPSubnet, savedSubnet);
        byye savedHidden = (byte) server.arg("hidden").toInt();
        writeByte(offsetAPHidden, savedHidden);
    }
    String message = "{\"status\":\"OK\"}";
    server.send(200, "application/json", message);
}

/**
 * @brief Retrieves the MIDI configuration data.
 * 
 * This function reads the MIDI configuration for all available channels from EEPROM. It then formats the configuration data into a JSON response and sends it to the client.
 * 
 * @note The JSON response includes each channel's configuration in the format "chXX": "config", where XX is the channel number.
 * @note The response also includes the "max" field, representing the total number of channels.
 * 
 * @path /midi-configuration.json
 */
void getMidiConfiguration()
{
    response += "{";
    int i = 0;
    for (int i = 1; i <= maxCh; i++)
    {
        int offset = memOffset + ((i - 1) * memSize);
        String config = readString(offset, memSize);
        String str = sprintf("\"ch%02d\":\"%s\",", i, config);
        response += str;
    }
    response += sprintf("\"max\":%d}", i);
    server.send(200, "application/json", response);
}

/**
 * @brief Saves the MIDI configuration data.
 * 
 * This function accepts POST requests and saves the provided MIDI configurations for each channel to EEPROM. The configuration values are extracted from the HTTP POST parameters, which include "ch01", "ch02", ..., "chXX" for each channel.
 * 
 * @note The configuration values for each channel are written to EEPROM at predefined memory offsets.
 * 
 * @path /save-midi
 */
void saveMidiConfiguration()
{
    if (server.method() == HTTP_POST)
    {
        int i = 0;
        for (int i = 1; i <= maxCh; i++)
        {
            int offset = memOffset + ((i - 1) * memSize);
            String par = sprintf("ch%02d", i);
            String value = server.arg("ssid_password");
            writeString(offset, memSize, value);
        }
    }
    String message = "{\"status\":\"OK\"}";
    server.send(200, "application/json", message);
}

/**
 * @brief Resets the AP (Access Point) configuration to default values.
 * 
 * This function resets the WiFi AP configuration (SSID, password, IP address, gateway, subnet, and hidden status) to default values. The default SSID and password are set to "PlanetbiruMusic", and the IP-related settings are reset to "0.0.0.0". The hidden network flag is set to 0 (disabled).
 * 
 * @note This function modifies EEPROM storage with the default AP configuration values.
 */
void resetAP()
{
    String savedSSID = "PlanetbiruMusic";
    String savedSSIDPassword = "PlanetbiruMusic";
    
    writeString(offsetSSID1, sizeofString30, savedSSID);
    writeString(offsetSSIDPassword1, sizeofString30, savedSSIDPassword);
    String savedIP = "0.0.0.0";
    writeIp(offsetAPIP, savedIP);
    String savedGateway = "0.0.0.0";
    writeIp(offsetAPGateway, savedGateway);
    String savedSubnet = "0.0.0.0";
    writeIp(offsetAPSubnet, savedSubnet);
    byte savedHidden = 0;
    writeByte(offsetAPHidden, savedHidden);
}

/**
 * @brief Resets the STA (Workstation) configuration to default values.
 * 
 * This function resets the WiFi STA configuration (SSID and password) to default values. The default SSID and password are set to "PlanetbiruMusic".
 * 
 * @note This function modifies EEPROM storage with the default STA configuration values.
 */
void resetSTA()
{
    String savedSSID = "PlanetbiruMusic";
    String savedSSIDPassword = "PlanetbiruMusic";
    writeString(offsetSSID2, sizeofString20, savedSSID);
    writeString(offsetSSIDPassword2, sizeofString20, savedSSIDPassword);
}

/**
 * @brief Retrieves WebSocket configuration data.
 * 
 * This function reads the saved WebSocket configuration parameters, including WiFi SSID, SSID password, WebSocket host, port, path, username, password, and enable status from EEPROM. It then formats these values into a JSON response and sends it to the client.
 * 
 * @note The JSON response includes the fields "ssid_name", "ssid_password", "ws_host", "ws_port", "ws_path", "ws_username", "ws_password", and "enable".
 * 
 * @path /ws-configuration.json
 */
void getSubData()
{
    String response = "";
    String savedSSID = readString(offsetSSID2, sizeofString20);
    String savedSSIDPassword = readString(offsetSSIDPassword2, sizeofString20);
    String savedWSHost = readString(offsetWSHost, sizeofString40);
    uint16_t savedWSPort = readWord(offsetWSPort);
    String savedWSPath = readString(offsetWSPath, sizeofString50);
    String savedWSUsername = readString(offsetWSUsername, sizeofString20);
    String savedWSPassword = readString(offsetWSPassword, sizeofString20);
    String savedWSTopic = readString(offsetWSTopic, sizeofString50);
    String savedEnable = readString(offsetEnable, sizeofBoolean);

    response += "{\"ssid_name\":\"";
    response += savedSSID;

    response += "\", \"ssid_password\":\"";
    response += savedSSIDPassword;

    response += "\", \"ws_host\":\"";
    response += savedWSHost;

    response += "\", \"ws_port\":\"";
    response += savedWSPort;

    response += "\", \"ws_path\":\"";
    response += savedWSPath;

    response += "\", \"ws_username\":\"";
    response += savedWSUsername;

    response += "\", \"ws_password\":\"";
    response += savedWSPassword;

    response += "\", \"enable\":\"";
    response += savedEnable;

    response += "\"}";
    server.send(200, "application/json", response);
}

/**
 * @brief Saves WebSocket configuration data.
 * 
 * This function accepts POST requests and saves the provided WebSocket configuration parameters (such as SSID, SSID password, WebSocket host, port, path, username, password, and enable status) to EEPROM. The configuration values are extracted from the HTTP POST parameters.
 * 
 * @note The configuration values are stored in EEPROM at predefined offsets.
 * 
 * @path /save-ws.html
 */
void saveConfigWS()
{
    if (server.method() == HTTP_POST)
    {
        String savedSSID = server.arg("ssid_name");
        writeString(offsetSSID2, sizeofString50, savedSSID);
        delay(1);

        String savedSSIDPassword = server.arg("ssid_password");
        writeString(offsetSSIDPassword2, sizeofString50, savedSSIDPassword);
        delay(1);

        String savedWSHost = server.arg("ws_host");
        writeString(offsetWSHost, sizeofString50, savedWSHost);
        delay(1);

        String savedWSPort = server.arg("ws_port");
        writeString(offsetWSPort, sizeofInteger10, savedWSPort);
        delay(1);

        String savedWSPath = server.arg("ws_path");
        writeString(offsetWSPath, sizeofString50, savedWSPath);
        delay(1);

        String savedWSUsername = server.arg("ws_username");
        writeString(offsetWSUsername, sizeofString50, savedWSUsername);
        delay(1);

        String savedWSPassword = server.arg("ws_password");
        writeString(offsetWSPassword, sizeofString50, savedWSPassword);
        delay(1);

        String savedEnable = server.arg("enable");
        writeString(offsetEnable, sizeofBoolean, savedEnable);
        delay(1);
    }
    String message = "{\"status\":\"OK\"}";
    server.send(200, "application/json", message);
}

/**
 * @brief Retrieves WebSocket configuration values for internal use.
 * 
 * This function reads and stores the WebSocket configuration values (such as path, username, password, host, and port) from EEPROM into the global variables.
 * 
 * @note This function does not return a response to the client; it only updates internal variables.
 */
void getWSConfig()
{
    savedWSPath = readString(offsetWSPath, sizeofString50);
    savedWSUsername = readString(offsetWSUsername, sizeofString50);
    savedWSPassword = readString(offsetWSPassword, sizeofString50);
    savedWSHost = readString(offsetWSHost, sizeofString50);
    savedWSPort = readString(offsetWSPort, sizeofInteger10);
}

/**
 * @brief WebSocket event handler.
 * 
 * This function handles various WebSocket events including connection, disconnection, text messages, ping/pong responses, and errors. It updates the connection state and processes incoming messages based on the event type.
 * 
 * @param type The type of WebSocket event (e.g., `WStype_CONNECTED`, `WStype_DISCONNECTED`, etc.).
 * @param payload The payload (message) received during the event.
 * @param length The length of the payload.
 */
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        connected = false;
        break;
    case WStype_CONNECTED:
    {
        connected = true;
    }
    break;
    case WStype_TEXT:
        handleMessage(payload, length);

        break;
    case WStype_BIN:
        break;
    case WStype_PING:
        // pong will be send automatically
        break;
    case WStype_PONG:
        // answer to a ping we send
        break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}

/**
 * @brief Handles incoming WebSocket text messages.
 * 
 * This function processes incoming WebSocket messages in JSON format. It deserializes the message into a JSON document and extracts relevant information, such as the command and the callback topic. This function can be extended to define custom actions based on the received commands.
 * 
 * @param payload The payload (message) received.
 * @param length The length of the message payload.
 */
void handleMessage(uint8_t *payload, size_t length)
{
    String request = "";
    String response = "";
    int i = 0;
    for (i = 0; i < length; i++)
    {
        request += (char)payload[i];
    }
    DynamicJsonDocument json(1024);
    deserializeJson(json, request);
    const char *command = json["command"];
    const char *responseTopic = json["callback_topic"];
    boolean requireResponse = !(responseTopic == NULL);

    // Define your program here...
}

/**
 * @brief Writes the state of a pin to EEPROM.
 * 
 * This function saves the state (HIGH or LOW) of a specified pin to EEPROM at a calculated offset. The state is saved as a string of two characters.
 * 
 * @param pin The GPIO pin number.
 * @param value The value to write (0 for LOW, 1 for HIGH).
 */
void writePinState(int pin, int value)
{
    char val[2];
    sprintf(val, "%d", value);
    int offset = 700 + (pin * 2);
    String str = String(val);
    writeString(offset, 2, str);
}

/**
 * @brief Sends a WebSocket response.
 * 
 * This function sends a response message to a specified WebSocket topic. It establishes a WebSocket connection, optionally authenticates, and then sends the response. The response is sent after a specified delay.
 * 
 * @param responseTopic The WebSocket topic to which the response will be sent.
 * @param response The response message to be sent.
 * @param callbackDelay The delay in milliseconds before sending the response.
 */
void sendResponse(const char *responseTopic, String response, int callbackDelay)
{
    String path = savedWSPath;

    WebSocketsClient webSocket2;
    webSocket2.begin(savedWSHost.c_str(), savedWSPort.toInt(), path.c_str());
    webSocket2.setAuthorization(savedWSUsername.c_str(), savedWSPassword.c_str());

    int i;
    long lastUpdate = millis();

    while (lastUpdate + callbackDelay >= millis() || WiFi.status() != WL_CONNECTED)
    {
        webSocket2.loop();
        if (webSocket2.sendPing())
        {
            break;
        }
        delay(7);
    }
    i = 0;
    while (!webSocket2.sendTXT(response) && i < 10)
    {
        webSocket2.loop();
        delay(10);
        i++;
    }
}

/**
 * @brief Reconnects to the WebSocket server.
 * 
 * This function attempts to reconnect to the WebSocket server using the stored WebSocket credentials and settings. It also configures the event handler for the WebSocket connection.
 */
void wsReconnect()
{
    Serial.println("wsReconnect()");
    String path = savedWSPath;
    Serial.println(savedWSHost);
    Serial.println(savedWSPort);
    Serial.println(path);

    webSocket.begin(savedWSHost.c_str(), savedWSPort.toInt(), path.c_str());
    webSocket.setAuthorization(savedWSUsername.c_str(), savedWSPassword.c_str());
    webSocket.onEvent(webSocketEvent);
}

/**
 * @brief Task for handling WebSocket communication.
 * 
 * This FreeRTOS task handles WebSocket communication in a loop. It checks if the WebSocket is connected and updates the onboard LED to indicate the connection status.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Task1(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        if (sysEnable == "1")
        {
            webSocket.loop();
            if (lastState != connected)
            {
                if (connected)
                {
                    digitalWrite(onboardLED, HIGH);
                }
                else
                {
                    digitalWrite(onboardLED, LOW);
                    lastDisconnected = millis();
                }
                lastState = connected;
            }
        }
        vTaskDelay(2);
    }
}

/**
 * @brief Task for reconnecting to WiFi.
 * 
 * This FreeRTOS task handles WiFi reconnection. It checks if the system is enabled and if the WiFi connection is lost. If necessary, it attempts to reconnect to the WiFi network.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Task2(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        if (sysEnable == "1" && !connected)
        {
            if (millis() - lastDisconnected > reconnectWiFiTreshold && WiFi.status() != WL_CONNECTED)
            {
                Serial.println("Reconnecting to WiFi...");
                WiFi.disconnect();
                boolean res = WiFi.reconnect();
                if (res)
                {
                    int j = 0;
                    while (WiFi.status() != WL_CONNECTED && j < 10)
                    {
                        delay(500);
                        j++;
                    }
                    if (j < 10)
                    {
                        Serial.println("");
                        Serial.print("Connected to ");
                        Serial.println(ssidUsed);
                        Serial.print("IP address: ");
                        Serial.println(WiFi.localIP());
                        Serial.println("");
                    }
                    lastDisconnected = millis();
                }
            }
        }
        vTaskDelay(5000);
    }
}

/**
 * @brief Listens for events on Drum Pad 1 (Channel 1).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 1 (Channel 1). 
 * The function processes the events by calling `listenChannel(1)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen01(void *pvParameters)
{
    listenChannel(1);
}

/**
 * @brief Listens for events on Drum Pad 2 (Channel 2).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 2 (Channel 2).
 * The function processes the events by calling `listenChannel(2)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen02(void *pvParameters)
{
    listenChannel(2);
}

/**
 * @brief Listens for events on Drum Pad 3 (Channel 3).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 3 (Channel 3).
 * The function processes the events by calling `listenChannel(3)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen03(void *pvParameters)
{
    listenChannel(3);
}

/**
 * @brief Listens for events on Drum Pad 4 (Channel 4).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 4 (Channel 4).
 * The function processes the events by calling `listenChannel(4)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen04(void *pvParameters)
{
    listenChannel(4);
}

/**
 * @brief Listens for events on Drum Pad 5 (Channel 5).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 5 (Channel 5).
 * The function processes the events by calling `listenChannel(5)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen05(void *pvParameters)
{
    listenChannel(5);
}

/**
 * @brief Listens for events on Drum Pad 6 (Channel 6).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 6 (Channel 6).
 * The function processes the events by calling `listenChannel(6)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen06(void *pvParameters)
{
    listenChannel(6);
}

/**
 * @brief Listens for events on Drum Pad 7 (Channel 7).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 7 (Channel 7).
 * The function processes the events by calling `listenChannel(7)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen07(void *pvParameters)
{
    listenChannel(7);
}

/**
 * @brief Listens for events on Drum Pad 8 (Channel 8).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 8 (Channel 8).
 * The function processes the events by calling `listenChannel(8)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen08(void *pvParameters)
{
    listenChannel(8);
}

/**
 * @brief Listens for events on Drum Pad 9 (Channel 9).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 9 (Channel 9).
 * The function processes the events by calling `listenChannel(9)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen09(void *pvParameters)
{
    listenChannel(9);
}

/**
 * @brief Listens for events on Drum Pad 10 (Channel 10).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 10 (Channel 10).
 * The function processes the events by calling `listenChannel(10)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen10(void *pvParameters)
{
    listenChannel(10);
}

/**
 * @brief Listens for events on Drum Pad 11 (Channel 11).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 11 (Channel 11).
 * The function processes the events by calling `listenChannel(11)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen11(void *pvParameters)
{
    listenChannel(11);
}

/**
 * @brief Listens for events on Drum Pad 12 (Channel 12).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 12 (Channel 12).
 * The function processes the events by calling `listenChannel(12)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen12(void *pvParameters)
{
    listenChannel(12);
}

/**
 * @brief Listens for events on Drum Pad 13 (Channel 13).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 13 (Channel 13).
 * The function processes the events by calling `listenChannel(13)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen13(void *pvParameters)
{
    listenChannel(13);
}

/**
 * @brief Listens for events on Drum Pad 14 (Channel 14).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 14 (Channel 14).
 * The function processes the events by calling `listenChannel(14)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen14(void *pvParameters)
{
    listenChannel(14);
}

/**
 * @brief Listens for events on Drum Pad 15 (Channel 15).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 15 (Channel 15).
 * The function processes the events by calling `listenChannel(15)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen15(void *pvParameters)
{
    listenChannel(15);
}

/**
 * @brief Listens for events on Drum Pad 16 (Channel 16).
 * 
 * This FreeRTOS task listens for MIDI or other input events on Drum Pad 16 (Channel 16).
 * The function processes the events by calling `listenChannel(16)`, which handles the input for this specific pad/channel.
 * 
 * @param pvParameters Task parameters (not used in this function).
 */
void Listen16(void *pvParameters)
{
    listenChannel(16);
}

/**
 * @brief Listens to a specific MIDI channel input and sends note on/off messages.
 * 
 * This function continuously listens for an input signal from the specified channel. 
 * It reads the input from the corresponding pin and checks if it exceeds the threshold value. 
 * If the condition is met, a MIDI note-on message is sent, followed by a note-off message 
 * after a specified delay. It also supports "solo mode" where only a specific channel can trigger notes.
 * 
 * @param channel The MIDI channel to listen to.
 */
void listenChannel(int channel)
{
    do
    {
        int pin = channelPin[channel];
        int input = analogRead(pin);
        float total = (float)input;

        uint16_t threshold = channelThreshold[channel];
        uint32_t delayTime = channelDuration[channel];
        if (input > threshold && solo == 0 && channel == soloChannel)
        {
            total += (float)adcRead(pin, 3);
            float average = total / 4;

            uint8_t velocity = calcVelocity((uint16_t)average, threshold, (uint16_t)channelHeadRoom[channel]);

            int instrumentCode = channelInstrument[channel];
            MIDI.sendNoteOn(instrumentCode, velocity, offsetMidiChannel);
            delayMicroseconds(delayTime);
            MIDI.sendNoteOff(instrumentCode, 0, offsetMidiChannel);

            if (offsetReadInterval > 0)
            {
                delayMicroseconds(offsetReadInterval);
            }
        }
    } while (true);
}

/**
 * @brief Reads multiple analog values from a specified pin and returns the total.
 * 
 * This function performs multiple analog reads from the given pin and accumulates the results.
 * It is used to get a more stable reading by averaging over multiple samples.
 * 
 * @param pin The analog pin to read from.
 * @param count The number of times to read the analog value.
 * @return The total of the analog readings.
 */
int adcRead(int pin, int count)
{
    int total = 0;
    while (count > 0)
    {
        int int2 = analogRead(pin);
        total += int2;
        count--;
    }
    return total;
}

/**
 * @brief Reads and parses the MIDI configuration for a specific channel.
 * 
 * This function reads the configuration string for the given MIDI channel and extracts values for 
 * instrument code, delay, and velocity scale. It updates the corresponding channel's configuration
 * values and stores them in global arrays. It also reads the solo mode settings.
 * 
 * @param channel The MIDI channel number to configure.
 * @param config The configuration string in the format "instrumentCode,delay,scale".
 */
void readMidiConfig(int channel, String config)
{
    int cnt = countSplitCharacters(config, ',');
    if (cnt > 2)
    {
        // Example: 120,100,1.2345678901
        // 120 is instrument code
        // 100 is delay. For percussion, delay is less than 1000 milli seconds
        // 1.2345678901 is scale. This scale is used to calculate the velocity. Please note that scale length is less than or equals 12
        String[] configs = splitStringMax(config, ',', cnt);

        String codeStr = configs[0];
        int code = codeStr.toInt();
        channelInstrument[channel] = code;

        String delayStr = configs[1];
        int delay = delayStr.toInt();
        channelDelay[channel] = delay;

        String scaleStr = configs[2];
        float scale = scaleStr.toFloat();
        channelHeadRoom[channel] = scale;
    }
    solo = readByte(offsetSolo);
    soloChannel = readByte(offsetSoloChannel);
    testVelocity = readByte(offsetSoloChannel);
}

/**
 * @brief Counts the occurrences of a specified character in a string.
 * 
 * This function counts how many times a given character appears in the provided string. It is used
 * for determining the number of splits needed when parsing a string.
 * 
 * @param text The string to search.
 * @param splitChar The character to count in the string.
 * @return The number of occurrences of `splitChar` in the string.
 */
int countSplitCharacters(String text, char splitChar)
{
    int returnValue = 0;
    int index = -1;

    while (index > -1)
    {
        index = text.indexOf(splitChar, index + 1);
        if (index > -1)
        {
            returnValue += 1;
        }
    }

    return returnValue;
}

/**
 * @brief Splits a string into an array of substrings based on a specified delimiter.
 * 
 * This function splits a given string into an array of substrings, based on the occurrence of a
 * specified delimiter character. It uses `countSplitCharacters()` to determine the number of splits.
 * 
 * @param text The string to split.
 * @param splitChar The delimiter character to split the string by.
 * @return An array of substrings created from the split operation.
 */
String[] splitString(String text, char splitChar)
{
    int splitCount = countSplitCharacters(text, splitChar);
    String returnValue[splitCount];
    int index = -1;
    int index2;

    for (int i = 0; i < splitCount - 1; i++)
    {
        index = text.indexOf(splitChar, index + 1);
        index2 = text.indexOf(splitChar, index + 1);

        if (index2 < 0)
            index2 = text.length() - 1;
        returnValue[i] = text.substring(index, index2);
    }

    return returnValue;
}

/**
 * @brief Splits a string into an array of substrings, with a maximum number of splits.
 * 
 * This function splits a string into a fixed number of substrings, based on the occurrence of a 
 * specified delimiter character. It ensures that no more than `splitCount` substrings are returned.
 * 
 * @param text The string to split.
 * @param splitChar The delimiter character to split the string by.
 * @param splitCount The maximum number of splits to perform.
 * @return An array of substrings created from the split operation.
 */
String[] splitStringMax(String text, char splitChar, int splitCount)
{
    String returnValue[splitCount];
    int index = -1;
    int index2;

    for (int i = 0; i < splitCount - 1; i++)
    {
        index = text.indexOf(splitChar, index + 1);
        index2 = text.indexOf(splitChar, index + 1);

        if (index2 < 0)
            index2 = text.length() - 1;
        returnValue[i] = text.substring(index, index2);
    }

    return returnValue;
}

/**
 * @brief Handles the root path ("/") request and serves the home page HTML.
 * 
 * This function sends a basic HTML page with buttons to navigate to the other configuration pages.
 * It also sets a cache header to control the caching behavior of the response.
 * 
 * @path /index.html
 */
void handleRoot()
{
    String response = "<!doctype html><html lang=en><head><meta charset=UTF-8><meta http-equiv=X-UA-Compatible content=\"IE=edge\"><meta name=viewport content=\"width=device-width,initial-scale=1,maximum-scale=1,minimum-scale=0,user-scalable=no\"><title>PlanetDrum - Home</title><link rel=stylesheet href=style.css><script src=script.js></script></head><body><div class=all><div class=form-item><div class=row><div class=column><input class=\"btn btn-100 btn-primary\" type=button value=\"General Setting\" onclick='window.location=\"general-configuration.html\"'></div><div class=column><input class=\"btn btn-100 btn-primary\" type=button value=\"Pad Setting\" onclick='window.location=\"pad-configuration.html\"'></div></div><div class=row><div class=column><input class=\"btn btn-100 btn-primary\" type=button value=\"Access Point\" onclick='window.location=\"ap-configuration.html\"'></div><div class=column><input class=\"btn btn-100 btn-primary\" type=button value=\"Web Socket\" onclick='window.location=\"ws-configuration.html\"'></div></div></div></div></body></html>";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/html", response);
}

/**
 * @brief Handles the "/ap-configuration.html" path request and serves the Access Point configuration page.
 * 
 * This function sends an HTML page with a form that allows the user to configure the SSID, password,
 * IP address, gateway, subnet, and hidden status for the Access Point.
 * It includes input validation for the IP address fields.
 * 
 * @path /ap-configuration.html
 */
void handleAP()
{
    String response = "<!doctype html><html lang=en><head><meta charset=UTF-8><meta http-equiv=X-UA-Compatible content=\"IE=edge\"><meta name=viewport content=\"width=device-width,initial-scale=1,maximum-scale=1,minimum-scale=0,user-scalable=no\"><title>PlanetDrum - Access Point Configuration</title><link rel=stylesheet href=style.css><script src=script.js></script></head><body><div class=all><form action=save-ap.html method=post onsubmit=\"return saveConfigAP(),!1\"><div class=form-item><div class=form-label>SSID</div><div class=form-input> <input name=ssid_name maxlength=20 required> </div></div><div class=form-item><div class=form-label>Password</div><div class=form-input> <input type=password name=ssid_password maxlength=20> </div></div><div class=form-item><div class=form-label>IP Address</div><div class=form-input> <input type=ipaddress pattern=^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$ name=ip placeholder=192.168.4.1 required> </div></div><div class=form-item><div class=form-label>Gateway</div><div class=form-input> <input type=ipaddress pattern=^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$ name=gateway placeholder=192.168.4.1 required></div></div><div class=form-item><div class=form-label>Subnet</div><div class=form-input> <input type=ipaddress pattern=^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$ name=subnet placeholder=255.255.255.0 required></div></div><div class=form-item><div class=form-label>Hidden</div><div class=form-input> <select name=hidden><option value=0>No</option><option value=1>Yes</option></select> </div></div><div class=form-item><div class=row><div class=column><input class=\"btn btn-100 btn-success\" type=submit name=save id=save value=Save></div><div class=column><input class=\"btn btn-100 btn-danger\" type=button name=save id=home value=Home onclick='window.location=\"index.html\"'></div></div></div></form></div><div class=popup-shadow><div class=\"popup popup-confirm\"><div class=popup-header>Confirm</div><div class=popup-body><p class=message></p></div><div class=popup-footer></div></div></div><script>window.onload=function(){loadConfigAP(),initIP()}</script></body></html>";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/html", response);
}

/**
 * @brief Handles the "/ws-configuration.html" path request and serves the WebSocket configuration page.
 * 
 * This function sends an HTML page with a form to configure the Wi-Fi connection and WebSocket settings.
 * It allows the user to specify parameters like SSID, password, WebSocket server details, and credentials.
 * 
 * @path /ws-configuration.html
 */
void handleWS()
{
    String response = "<!doctype html><html lang=en><head><meta charset=UTF-8><meta http-equiv=X-UA-Compatible content=\"IE=edge\"><meta name=viewport content=\"width=device-width,initial-scale=1,maximum-scale=1,minimum-scale=0,user-scalable=no\"><title>PlanetDrum - Subscribtion Configuration</title><link rel=stylesheet href=style.css><script src=script.js></script></head><body><div class=all><form action=\"\" method=post onsubmit=\"return saveConfigWS()\"><div class=form-item><div class=form-label>Connect to Wifi</div><div class=form-input><select name=wifi_enable><option value=0>No</option><option value=1>Yes</option></select></div></div><div class=form-item><div class=form-label>Wifi SSID</div><div class=form-input><input name=ssid_name></div></div><div class=form-item><div class=form-label>Wifi Password</div><div class=form-input><input type=password name=ssid_password></div></div><div class=form-item><div class=form-label>Connect to WebSocket</div><div class=form-input><select name=ws_enable><option value=0>No</option><option value=1>Yes</option></select></div></div><div class=form-item><div class=form-label>WebSocket Scheme</div><div class=form-input><select name=ws_scheme><option value=ws>ws</option><option value=wss>wss</option></select></div></div><div class=form-item><div class=form-label>WebSocket Host</div><div class=form-input><input name=ws_host></div></div><div class=form-item><div class=form-label>WebSocket Port</div><div class=form-input><input name=ws_port></div></div><div class=form-item><div class=form-label>WebSocket Path</div><div class=form-input><input name=ws_path></div></div><div class=form-item><div class=form-label>WebSocket Username</div><div class=form-input><input name=ws_username></div></div><div class=form-item><div class=form-label>WebSocket Password</div><div class=form-input><input type=password name=ws_password></div></div><div class=form-item><div class=row><div class=column><input class=\"btn btn-100 btn-success\" type=submit name=save value=Save></div><div class=column><input class=\"btn btn-100 btn-danger\" type=button name=save value=Home onclick='window.location=\"index.html\"'></div></div></div></form></div><div class=popup-shadow><div class=\"popup popup-confirm\"><div class=popup-header>Confirm</div><div class=popup-body><p class=message></p></div><div class=popup-footer></div></div></div><script>window.onload=function(){loadConfigWS()}</script></body></html>";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/html", response);
}

/**
 * @brief Handles the "/style.css" path request and serves the CSS stylesheet.
 * 
 * This function serves the stylesheet for the configuration pages. The styles are designed for forms,
 * buttons, layout, and popups.
 * 
 * @path /style.css
 */
void handleStyle()
{
    String response = ".all,.btn{box-sizing:border-box}.form-input,.pad,.pad-wrapper,body{position:relative}.pad::after,.pad::before{display:block;left:4px;font-size:10px;white-space:nowrap}.btn,a.ctrl{color:#333}body{margin:0;padding:0;font-family:Verdana,Geneva,Tahoma,sans-serif;font-size:13px;color:#555}.popup-footer .btn,h3{margin:4px 0}.all{width:900px;max-width:100%;margin:auto;padding:20px}.form-label{padding:5px 0}.form-input input[type=ipaddress],.form-input input[type=number],.form-input input[type=password],.form-input input[type=text],.form-input select{width:100%;box-sizing:border-box;padding:6px 10px;border-radius:3px;border:1px solid #ccc;background-color:#fff;margin-bottom:2px}.form-input input[type=ipaddress].invalid-ip{border:1px solid #c00}.form-input input[type=ipaddress]:focus,.form-input input[type=number]:focus,.form-input input[type=password]:focus,.form-input input[type=text]:focus,.form-input select:focus{outline:0;transition:border .3s ease-in-out}.form-input input[type=number]:focus-visible,.form-input input[type=password]:focus-visible,.form-input input[type=text]:focus-visible,.form-input select:focus-visible{outline:0;border:1px solid #4d80ce}.btn{min-width:60px;padding:6px 10px;border-radius:3px;border:1px solid #bdbcbc;background-color:#c7c6c6;margin:8px 0}.btn-100,.two-side-table{width:100%}.btn-success{color:#fff;border:1px solid #46992d;background-color:#419129}.btn-primary{color:#fff;border:1px solid #2d5899;background-color:#3464ac}.btn-warning{color:#212529;border:1px solid #ffc107;background-color:#ffc107}.btn-danger{color:#fff;border:1px solid #dc3545;background-color:#dc3545}.row{display:flex;gap:10px}.column,.pad-wrapper{flex:50%;justify-content:space-between}.pad{height:60px;font-size:14px;background-color:#64ffc5;border:1px solid #4ddba6;min-width:20px}.pad::after,.pad::before,.popup,.popup-shadow,a.ctrl::before{position:absolute}.pad:active{background-image:radial-gradient(#98ffd9,#64ffc5)}.pad::before{content:attr(data-number);top:4px}.pad::after{content:attr(data-tt);width:calc(100% - 8px);bottom:4px;text-overflow:ellipsis;overflow-x:hidden}.pad-wrapper .editor,a.ctrl{width:20px;height:20px;display:block}.pad-wrapper .editor{position:absolute;top:10px;right:0}a.ctrl::before{content:'\270E';color:#333;display:block;font-size:15px;transform:rotate(90deg)}.popup-shadow{z-index:1001;min-width:100vw;min-height:100vh;top:0;bottom:0;left:0;right:0;overflow:auto;background-color:rgba(0,0,0,.4);color:#aaa;display:none}.popup,.popup-confirm{left:50%;min-height:100px}.popup{color:#333;background-color:#fff;border:1px solid #7c99c1;display:none;width:480px;margin-left:-240px;top:20px}.popup-confirm{position:fixed;width:320px;margin-left:-160px;top:calc(50vh - 70px);z-index:10}@media screen and (max-width:520px){.popup-w{width:calc(100vw - 40px);left:0;margin-left:20px}}@media screen and (max-width:360px){.popup-confirm{width:calc(100vw - 40px);left:0;margin-left:20px;min-height:100px}}.popup-header{padding:4px 12px;min-height:25px;line-height:25px;background-color:#3464ac;color:#fff}.popup-body{padding:5px 12px;min-height:25px;background-color:#fff}.popup-body p{line-height:1.5}.popup-footer{padding:6px 12px;min-height:25px;background-color:#eee;text-align:end}.two-side-table td{padding:2px 0}.two-side-table tr>td:first-child{width:35%}";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/css", response);
}

/**
 * @brief Handles the "/script.js" path request and serves the JavaScript file.
 * 
 * This script handles client-side interactions, including form validation, saving and loading configuration,
 * and handling user confirmation dialogs for the configuration.
 * 
 * @path /script.js
 */
void handleScript()
{
    String response = "function _ce(e){return document.createElement(e)}function _nm(e){return document.getElementsByName(e)[0]}function _sl(e){return document.querySelector(e)}function _sls(e){return document.querySelectorAll(e)}Element.prototype.matches||(Element.prototype.matches=Element.prototype.msMatchesSelector||Element.prototype.webkitMatchesSelector),Element.prototype.closest||(Element.prototype.closest=function(e){let n=this;do{if(Element.prototype.matches.call(n,e))return n;n=n.parentElement||n.parentNode}while(null!==n&&1===n.nodeType);return null}),Element.prototype.remove||(Element.prototype.remove=function(e){this.parentNode.removeChild(this)}),Element.prototype.popupShow=function(){this.style.display=\"block\",this.closest(\".popup-shadow\").style.display=\"block\"},Element.prototype.attr=function(e,n){return void 0!==n?this.setAttribute(e,n):this.getAttribute(e)},Element.prototype.popupHide=function(){this.style.display=\"none\",this.closest(\".popup-shadow\").style.display=\"none\"},Element.prototype.on=function(e,n,t){return this.addEventListener(e,n,t)},Element.prototype.off=function(e,n,t){return this.removeEventListener(e,n,t)},Element.prototype.find=function(e){return this.querySelector(e)},Element.prototype.findAll=function(e){return this.querySelectorAll(e)};let ajax={},cMsg=\"Are you sure you want to save the config?\",rMsg=\"Are you sure you want to reset the config?\",pSel=\".popup-confirm\",pTtl=\"User Confirmation\";function customConfim(e,n,t,o,a,s,l){n=n||\"Are you sure?\",t=t||\"Confirm\",o=o||\"OK\",a=a||\"Cancel\";let i=_sl(e);i.find(\".popup-header\").innerText=t,i.find(\".popup-body .message\").innerText=n,i.find(\".btn-ok\")&&i.find(\".btn-ok\").remove(),i.find(\".btn-cancel\")&&i.find(\".btn-cancel\").remove();let u=_ce(\"button\");u.attr(\"class\",\"btn btn-success btn-ok\"),u.innerText=o;let r=_ce(\"button\");r.attr(\"class\",\"btn btn-warning btn-cancel\"),r.innerText=a;let p=i.find(\".popup-footer\");p.appendChild(u),p.appendChild(document.createTextNode(\" \")),p.appendChild(r),i.popupShow(),s=s||function(){i.popupHide()},l=l||function(){i.popupHide()};try{u.off(\"click\"),r.off(\"click\")}catch(c){}u.on(\"click\",function(){s(),i.popupHide()}),r.on(\"click\",function(){l(),i.popupHide()})}function saveConfigWS(){return customConfim(pSel,cMsg,pTtl,\"Yes\",\"No\",function(){ajax.post(\"save-ws.html\",{action:\"save-ws\",wifi_enable:_nm(\"wifi_enable\").value,ssid_name:_nm(\"ssid_name\").value,ssid_password:_nm(\"ssid_password\").value,ws_enable:_nm(\"ws_enable\").value,ws_host:_nm(\"ws_host\").value,ws_port:_nm(\"ws_port\").value,ws_path:_nm(\"ws_path\").value,ws_username:_nm(\"ws_username\").value,ws_password:_nm(\"ws_password\").value},function(e,n,t){console.log(t)},!0)},null),!1}function loadConfigWS(){ajax.get(\"ws-configuration.json\",{},function(e,n,t){try{let o=JSON.parse(e);_nm(\"wifi_enable\").value=o.wifi_enable,_nm(\"ssid_name\").value=o.ssid_name,_nm(\"ssid_password\").value=o.ssid_password,_nm(\"ws_enable\").value=o.ws_enable,_nm(\"ws_scheme\").value=o.ws_scheme,_nm(\"ws_host\").value=o.ws_host,_nm(\"ws_port\").value=o.ws_port,_nm(\"ws_path\").value=o.ws_path,_nm(\"ws_username\").value=o.ws_username,_nm(\"ws_password\").value=o.ws_password}catch(a){}},!0)}function loadConfigAP(){ajax.get(\"ap-configuration.json\",{},function(e,n,t){try{let o=JSON.parse(e);_nm(\"ssid_name\").value=o.ssid_name,_nm(\"ssid_password\").value=o.ssid_password,_nm(\"ip\").value=o.ip,_nm(\"gateway\").value=o.gateway,_nm(\"subnet\").value=o.subnet,_nm(\"hidden\").value=o.hidden}catch(a){}},!0)}function saveConfigAP(){return customConfim(pSel,cMsg,pTtl,\"Yes\",\"No\",function(){ajax.post(\"save-ap.html\",{action:\"save-ap\",ssid_name:_nm(\"ssid_name\").value,ssid_password:_nm(\"ssid_password\").value,ip:_nm(\"ip\").value,gateway:_nm(\"gateway\").value,subnet:_nm(\"subnet\").value,hidden:_nm(\"hidden\").value},function(e,n,t){},!0)},null),!1}function loadConfigGeneral(){ajax.get(\"general-configuration.json\",{},function(e,n,t){try{let o=JSON.parse(e);_nm(\"channel\").value=o.channel,_nm(\"read_interval\").value=o.read_interval,_nm(\"solo_pad\").value=o.solo_pad,_nm(\"solo_pad_number\").value=o.solo_pad_number}catch(a){}},!0)}function loadDefaultGeneral(){customConfim(pSel,rMsg,pTtl,\"Yes\",\"No\",function(){_nm(\"channel\").value=10,_nm(\"read_interval\").value=1e3,_nm(\"solo_pad\").value=0,_nm(\"solo_pad_number\").value=1},null)}function saveConfigGeneral(){return customConfim(pSel,cMsg,pTtl,\"Yes\",\"No\",function(){ajax.post(\"save-general.html\",{channel:_nm(\"channel\").value,read_interval:_nm(\"read_interval\").value,solo_pad:_nm(\"solo_pad\").value,solo_pad_number:_nm(\"solo_pad_number\").value},function(e,n,t){},!0)},null),!1}function handleIP(e){let n=e.target;isValidIP(n.value)?n.classList.remove(\"invalid-ip\"):(n.classList.remove(\"invalid-ip\"),n.classList.add(\"invalid-ip\"))}function initIP(){let e=_sls('input[type=\"ipaddress\"]');if(null!=e&&e.length)for(let n=0;n<e.length;n++)e[n].on(\"keyup\",function(e){handleIP(e)}),e[n].on(\"change\",function(e){handleIP(e)})}function isValidIP(e){if(0==e.length)return!0;let n,t=e.split(\".\");if(4!=t.length)return!1;for(n in t)if(isNaN(parseInt(t[n]))||t[n]<0||255<t[n])return!1;return!0}ajax.create=function(){if(\"undefined\"!=typeof XMLHttpRequest)return new XMLHttpRequest;for(let e,n=[\"MSXML2.XmlHttp.6.0\",\"MSXML2.XmlHttp.5.0\",\"MSXML2.XmlHttp.4.0\",\"MSXML2.XmlHttp.3.0\",\"MSXML2.XmlHttp.2.0\",\"Microsoft.XmlHttp\",],t=0;t<n.length;t++)try{e=new ActiveXObject(n[t]);break}catch(o){}return httpRequest},ajax.send=function(e,n,t,o,a){a=a||!1;let s=ajax.create();s.open(t,e,a),s.onreadystatechange=function(){if(4==s.readyState&&null!=n&&\"function\"==typeof n){let e=n.length;1==e?n(s.responseText):2==e?n(s.responseText,s.status):3==e&&n(s.responseText,s.status,s.statusText)}},\"POST\"==t&&s.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\"),s.send(o)},ajax.get=function(e,n,t,o){let a,s=[];for(a in n)n.hasOwnProperty(a)&&s.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(n[a]));ajax.send(e+(s.length?\"?\"+s.join(\"&\"):\"\"),t,\"GET\",null,o)},ajax.post=function(e,n,t,o){let a,s=[];for(a in n)n.hasOwnProperty(a)&&s.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(n[a]));ajax.send(e,t,\"POST\",s.join(\"&\"),o)};";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/javascript", response);
}

/**
 * @brief Handles 404 errors when the requested URL path is not found.
 * 
 * This function is called when the user tries to access a path that is not registered in the server. It generates a 
 * response containing the request's URI, method (GET or POST), and any arguments passed in the request, returning 
 * a detailed error message with the HTTP 404 status code.
 * 
 * @param None
 */
void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

/**
 * @brief Sets up the system configurations, Wi-Fi settings, and server routes.
 * 
 * This function is executed at the start to initialize the system, including loading configuration from EEPROM, setting 
 * up Wi-Fi as either an Access Point (AP) or Workstation (STA), and initializing the server routes. It also starts two 
 * independent tasks and sets up the necessary hardware configurations such as the onboard LED.
 * 
 * @param None
 */
void setup(void)
{
    EEPROM.begin(1024);
    Serial.begin(115200);

    loadMidiConfig();

    String configured = readString(offsetConfigured, sizeofBoolean);
    if (configured != "1")
    {
        resetAP();
    }

    sysEnable = readString(offsetEnable, sizeofBoolean);

    if (sysEnable == "1")
    {
        WiFi.setAutoReconnect(true);
        WiFi.setAutoConnect(true);
        WiFi.mode(WIFI_AP_STA);
    }

    // Configure Wi-Fi as Access Point (AP)
    String ssidAPS = readString(offsetSSID1, sizeofString50);
    String passwordAPS = readString(offsetSSIDPassword1, sizeofString50);
    Serial.println("ssidAPS     = " + ssidAPS);
    Serial.println("passwordAPS = " + passwordAPS);

    if (ssidAPS.length() > 0)
    {
        String sip = readString(offsetAPIP, sizeofString20);
        String sgw = readString(offsetAPGateway, sizeofString20);
        String ssn = readString(offsetAPSubnet, sizeofString20);
        String shd = readString(offsetAPHidden, sizeofBoolean);

        IPAddress test1(10, 10, 10, 10);

        if (sip.length() > 6 && sgw.length() > 6 && ssn.length() > 6 && test1.fromString(sip) && test1.fromString(sgw) && test1.fromString(ssn))
        {
            apLocalID.fromString(sip);
            apGateway.fromString(sgw);
            apSubnet.fromString(ssn);
        }

        WiFi.softAP(ssidAPS.c_str(), passwordAPS.c_str(), 1, (int)shd.toInt(), 10);
        WiFi.softAPConfig(apLocalID, apGateway, apSubnet);
    }
    else
    {
        WiFi.softAP(ssid2, password2, 1, 0, 10);
        WiFi.softAPConfig(apLocalID, apGateway, apSubnet);
    }

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    Serial.println("");

    if (sysEnable == "1")
    {
        // Configure Wi-Fi as Workstation (STA)
        String ssidStaS = readString(offsetSSID2, sizeofString50);
        String passwordStaS = readString(offsetSSIDPassword2, sizeofString50);
        const char *ssidSta = ssidStaS.c_str();
        const char *passwordSta = passwordStaS.c_str();
        if (ssidStaS.length() > 0)
        {
            ssidUsed = ssidSta;
            WiFi.begin(ssidSta, passwordSta);
        }
        else
        {
            ssidUsed = ssid;
            WiFi.begin(ssid, password);
        }

        // Wait for connection
        int trying = 0;
        Serial.println("");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
            trying++;
            if (trying > 20)
            {
                break;
            }
        }
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssidUsed);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        Serial.println("");
    }

    if (MDNS.begin("esp32"))
    {
        Serial.println("MDNS responder started");
    }

    // Set up HTTP server routes
    server.on("/", handleRoot);
    server.on("/index.html", handleRoot);
    server.on("/ap-configuration.html", handleAP);
    server.on("/ws-configuration.html", handleWS);
    server.on("/style.css", handleStyle);
    server.on("/script.js", handleScript);
    server.on("/ws-configuration.json", getSubData);
    server.on("/ap-configuration.json", getAPData);
    server.on("/midi-configuration.json", getMidiConfiguration);
    server.on("/save-ws.html", saveConfigWS);
    server.on("/save-ap.html", saveConfigAP);
    server.on("/test-note.html", handleTestNote);
    server.on("/hit-pad.html", handleHitPad);
    server.on("/inline", []()
              { server.send(200, "text/plain", "this works as well"); });
    server.onNotFound(handleNotFound);
    server.begin();

    getWSConfig();

    // Set up FreeRTOS tasks for listening on multiple channels
    xTaskCreate(
        Task1, "Task1", 32768, NULL, 3, NULL); // Task1 with priority 3
    xTaskCreate(
        Task2, "Task2", 5120, NULL, 2, NULL);  // Task2 with priority 2

    xTaskCreate(Listen01, "Listen01", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen02, "Listen02", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen03, "Listen03", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen04, "Listen04", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen05, "Listen05", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen06, "Listen06", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen07, "Listen07", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen08, "Listen08", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen09, "Listen09", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen10, "Listen10", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen11, "Listen11", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen12, "Listen12", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen13, "Listen13", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen14, "Listen14", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen15, "Listen15", 5120, NULL, 2, NULL);    
    xTaskCreate(Listen16, "Listen16", 5120, NULL, 2, NULL);    

    // Set onboard LED pin
    pinMode(onboardLED, OUTPUT);

    // If system is enabled, attempt WebSocket reconnection
    if (sysEnable == "1")
    {
        wsReconnect();
    }

    Serial.println("Device is ready");
}

/**
 * @brief Handles the "/test-note.html" path request for testing a MIDI note.
 * 
 * This function responds to GET requests by reading the "note" query parameter, converting it to an integer
 * representing a MIDI instrument code, and triggering the corresponding MIDI note on and off with a test velocity.
 * The test note is played for a brief period, controlled by the `delay` value.
 * 
 * @path /test-note.html
 * @param note The MIDI note number (instrument code) to be triggered.
 */
void handleTestNote()
{
    if (server.method() == HTTP_GET)
    {
        String noteStr = server.arg("note");
        int instrumentCode = noteStr.toInt();
        Midi.on(instrumentCode, (int)testVelocity);
        delay(delay);
        Midi.off(instrumentCode);
    }
    server.send(200, "application/json", "{}");
}

/**
 * @brief Handles the "/hit-pad.html" path request for testing a MIDI pad hit.
 * 
 * This function responds to GET requests by reading the "pad" query parameter, which represents the pad channel,
 * and triggers the corresponding MIDI note associated with that channel. The note is played with a test velocity.
 * The pad hit is triggered for a brief period, controlled by the `delay` value.
 * 
 * @path /hit-pad.html
 * @param pad The pad channel number, which corresponds to a specific MIDI instrument.
 */
void handleHitPad()
{
    if (server.method() == HTTP_GET)
    {
        String padStr = server.arg("pad");
        int channel = padStr.toInt();
        int instrumentCode = channelInstrument[channel];
        Midi.on(instrumentCode, (int)testVelocity);
        delay(delay);
        Midi.off(instrumentCode);
    }
    server.send(200, "application/json", "{}");
}

/**
 * @brief Main loop to handle incoming HTTP requests.
 * 
 * The loop function is executed continuously in the main program flow. It processes HTTP requests made to the server by 
 * calling `server.handleClient()` and ensuring that the server can handle incoming client connections. A small delay is 
 * introduced to avoid overloading the microcontroller.
 * 
 * @param None
 */
void loop(void)
{
    // HTTP Client must be handled by main loop
    server.handleClient();
    delay(2);
}