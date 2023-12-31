#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

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
//                                  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
byte channelInstrument[] = {0, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50};
//                                  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
uint16_t channelHeadRoom[] = {0, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};

//                                  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
uint16_t channelThreshold[] = {0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

//                                  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
int channelPin[] = {0, 36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 4, 2, 15, 0};

//                                  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
int channelDelay[] = {0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

int solo = 0;
int soloChannel = 0;
int testVelocity = 0;

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

void Task1(void *pvParameters);
void Task2(void *pvParameters);

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

string readIp(int address)
{
    byte one = EEPROM.read(address) & 0xFF;
    byte two = EEPROM.read(address + 1) & 0xFF;
    byte three = EEPROM.read(address + 2) & 0xFF;
    byte four = EEPROM.read(address + 3) & 0xFF;
    return sprintf("%d.%d.%d.%d", one, two, three, four);
}

void writeByte(int address, uint8_t value)
{
    byte one = (value & 0xFF);
    EEPROM.write(address, one);
    EEPROM.commit();
}

byte readByte(int address)
{
    byte one = EEPROM.read(address);
    return (one & 0xFF);
}

uint16_t writeWord(int address, uint16_t value)
{
    byte two = ((value >> 16) & 0xFF);
    byte one = ((value >> 24) & 0xFF);

    EEPROM.write(address, two);
    EEPROM.write(address + 1, one);
    EEPROM.commit();
}

uint16_t readWord(int address)
{
    byte two = EEPROM.read(address);
    byte one = EEPROM.read(address + 1);

    return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}

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

uint32_t readDoubleWord(int address)
{
    byte four = EEPROM.read(address);
    byte three = EEPROM.read(address + 1);
    byte two = EEPROM.read(address + 2);
    byte one = EEPROM.read(address + 3);

    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

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

// /ap-configuration.json
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

// /save-ap.html
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

// /midi-configuration.json
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

// /save-midi
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

void resetSTA()
{
    String savedSSID = "PlanetbiruMusic";
    String savedSSIDPassword = "PlanetbiruMusic";
    writeString(offsetSSID2, sizeofString20, savedSSID);
    writeString(offsetSSIDPassword2, sizeofString20, savedSSIDPassword);
}

// /ws-configuration.json
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

// /save-ws.html
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

void getWSConfig()
{
    savedWSPath = readString(offsetWSPath, sizeofString50);
    savedWSUsername = readString(offsetWSUsername, sizeofString50);
    savedWSPassword = readString(offsetWSPassword, sizeofString50);
    savedWSHost = readString(offsetWSHost, sizeofString50);
    savedWSPort = readString(offsetWSPort, sizeofInteger10);
}

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

void writePinState(int pin, int value)
{
    char val[2];
    sprintf(val, "%d", value);
    int offset = 700 + (pin * 2);
    String str = String(val);
    writeString(offset, 2, str);
}

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

// Drum Pad 1
void Listen01(void *pvParameters)
{
    listenChannel(1);
}

// Drum Pad 2
void Listen02(void *pvParameters)
{
    listenChannel(2);
}

// Drum Pad 3
void Listen03(void *pvParameters)
{
    listenChannel(3);
}

// Drum Pad 4
void Listen04(void *pvParameters)
{
    listenChannel(4);
}

// Drum Pad 5
void Listen05(void *pvParameters)
{
    listenChannel(5);
}

// Drum Pad 6
void Listen06(void *pvParameters)
{
    listenChannel(6);
}

// Drum Pad 7
void Listen07(void *pvParameters)
{
    listenChannel(7);
}

// Drum Pad 8
void Listen08(void *pvParameters)
{
    listenChannel(8);
}

// Drum Pad 9
void Listen09(void *pvParameters)
{
    listenChannel(9);
}

// Drum Pad 10
void Listen10(void *pvParameters)
{
    listenChannel(10);
}

// Drum Pad 11
void Listen11(void *pvParameters)
{
    listenChannel(11);
}

// Drum Pad 12
void Listen12(void *pvParameters)
{
    listenChannel(12);
}

// Drum Pad 13
void Listen13(void *pvParameters)
{
    listenChannel(13);
}

// Drum Pad 14
void Listen14(void *pvParameters)
{
    listenChannel(14);
}

// Drum Pad 15
void Listen15(void *pvParameters)
{
    listenChannel(15);
}

// Drum Pad 16
void Listen16(void *pvParameters)
{
    listenChannel(16);
}

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

            uint8_t velocity = calcVelocity((uint16_t)average, threshold, (uint16_t)channelHeadRoom[channel])

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

// /
// /index.html
void handleRoot()
{
    String response = "<!doctype html><html lang=en><head><meta charset=UTF-8><meta http-equiv=X-UA-Compatible content=\"IE=edge\"><meta name=viewport content=\"width=device-width,initial-scale=1,maximum-scale=1,minimum-scale=0,user-scalable=no\"><title>PlanetDrum - Home</title><link rel=stylesheet href=style.css><script src=script.js></script></head><body><div class=all><div class=form-item><div class=row><div class=column><input class=\"btn btn-100 btn-primary\" type=button value=\"General Setting\" onclick='window.location=\"general-configuration.html\"'></div><div class=column><input class=\"btn btn-100 btn-primary\" type=button value=\"Pad Setting\" onclick='window.location=\"pad-configuration.html\"'></div></div><div class=row><div class=column><input class=\"btn btn-100 btn-primary\" type=button value=\"Access Point\" onclick='window.location=\"ap-configuration.html\"'></div><div class=column><input class=\"btn btn-100 btn-primary\" type=button value=\"Web Socket\" onclick='window.location=\"ws-configuration.html\"'></div></div></div></div></body></html>";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/html", response);
}

// /ap-configuration.html
void handleAP()
{
    String response = "<!doctype html><html lang=en><head><meta charset=UTF-8><meta http-equiv=X-UA-Compatible content=\"IE=edge\"><meta name=viewport content=\"width=device-width,initial-scale=1,maximum-scale=1,minimum-scale=0,user-scalable=no\"><title>PlanetDrum - Access Point Configuration</title><link rel=stylesheet href=style.css><script src=script.js></script></head><body><div class=all><form action=save-ap.html method=post onsubmit=\"return saveConfigAP(),!1\"><div class=form-item><div class=form-label>SSID</div><div class=form-input> <input name=ssid_name maxlength=20 required> </div></div><div class=form-item><div class=form-label>Password</div><div class=form-input> <input type=password name=ssid_password maxlength=20> </div></div><div class=form-item><div class=form-label>IP Address</div><div class=form-input> <input type=ipaddress pattern=^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$ name=ip placeholder=192.168.4.1 required> </div></div><div class=form-item><div class=form-label>Gateway</div><div class=form-input> <input type=ipaddress pattern=^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$ name=gateway placeholder=192.168.4.1 required></div></div><div class=form-item><div class=form-label>Subnet</div><div class=form-input> <input type=ipaddress pattern=^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$ name=subnet placeholder=255.255.255.0 required></div></div><div class=form-item><div class=form-label>Hidden</div><div class=form-input> <select name=hidden><option value=0>No</option><option value=1>Yes</option></select> </div></div><div class=form-item><div class=row><div class=column><input class=\"btn btn-100 btn-success\" type=submit name=save id=save value=Save></div><div class=column><input class=\"btn btn-100 btn-danger\" type=button name=save id=home value=Home onclick='window.location=\"index.html\"'></div></div></div></form></div><div class=popup-shadow><div class=\"popup popup-confirm\"><div class=popup-header>Confirm</div><div class=popup-body><p class=message></p></div><div class=popup-footer></div></div></div><script>window.onload=function(){loadConfigAP(),initIP()}</script></body></html>";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/html", response);
}

// /ws-configuration.html
void handleWS()
{
    String response = "<!doctype html><html lang=en><head><meta charset=UTF-8><meta http-equiv=X-UA-Compatible content=\"IE=edge\"><meta name=viewport content=\"width=device-width,initial-scale=1,maximum-scale=1,minimum-scale=0,user-scalable=no\"><title>PlanetDrum - Subscribtion Configuration</title><link rel=stylesheet href=style.css><script src=script.js></script></head><body><div class=all><form action=\"\" method=post onsubmit=\"return saveConfigWS()\"><div class=form-item><div class=form-label>Connect to Wifi</div><div class=form-input><select name=wifi_enable><option value=0>No</option><option value=1>Yes</option></select></div></div><div class=form-item><div class=form-label>Wifi SSID</div><div class=form-input><input name=ssid_name></div></div><div class=form-item><div class=form-label>Wifi Password</div><div class=form-input><input type=password name=ssid_password></div></div><div class=form-item><div class=form-label>Connect to WebSocket</div><div class=form-input><select name=ws_enable><option value=0>No</option><option value=1>Yes</option></select></div></div><div class=form-item><div class=form-label>WebSocket Scheme</div><div class=form-input><select name=ws_scheme><option value=ws>ws</option><option value=wss>wss</option></select></div></div><div class=form-item><div class=form-label>WebSocket Host</div><div class=form-input><input name=ws_host></div></div><div class=form-item><div class=form-label>WebSocket Port</div><div class=form-input><input name=ws_port></div></div><div class=form-item><div class=form-label>WebSocket Path</div><div class=form-input><input name=ws_path></div></div><div class=form-item><div class=form-label>WebSocket Username</div><div class=form-input><input name=ws_username></div></div><div class=form-item><div class=form-label>WebSocket Password</div><div class=form-input><input type=password name=ws_password></div></div><div class=form-item><div class=row><div class=column><input class=\"btn btn-100 btn-success\" type=submit name=save value=Save></div><div class=column><input class=\"btn btn-100 btn-danger\" type=button name=save value=Home onclick='window.location=\"index.html\"'></div></div></div></form></div><div class=popup-shadow><div class=\"popup popup-confirm\"><div class=popup-header>Confirm</div><div class=popup-body><p class=message></p></div><div class=popup-footer></div></div></div><script>window.onload=function(){loadConfigWS()}</script></body></html>";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/html", response);
}

// /style.css
void handleStyle()
{
    String response = ".all,.btn{box-sizing:border-box}.form-input,.pad,.pad-wrapper,body{position:relative}.pad::after,.pad::before{display:block;left:4px;font-size:10px;white-space:nowrap}.btn,a.ctrl{color:#333}body{margin:0;padding:0;font-family:Verdana,Geneva,Tahoma,sans-serif;font-size:13px;color:#555}.popup-footer .btn,h3{margin:4px 0}.all{width:900px;max-width:100%;margin:auto;padding:20px}.form-label{padding:5px 0}.form-input input[type=ipaddress],.form-input input[type=number],.form-input input[type=password],.form-input input[type=text],.form-input select{width:100%;box-sizing:border-box;padding:6px 10px;border-radius:3px;border:1px solid #ccc;background-color:#fff;margin-bottom:2px}.form-input input[type=ipaddress].invalid-ip{border:1px solid #c00}.form-input input[type=ipaddress]:focus,.form-input input[type=number]:focus,.form-input input[type=password]:focus,.form-input input[type=text]:focus,.form-input select:focus{outline:0;transition:border .3s ease-in-out}.form-input input[type=number]:focus-visible,.form-input input[type=password]:focus-visible,.form-input input[type=text]:focus-visible,.form-input select:focus-visible{outline:0;border:1px solid #4d80ce}.btn{min-width:60px;padding:6px 10px;border-radius:3px;border:1px solid #bdbcbc;background-color:#c7c6c6;margin:8px 0}.btn-100,.two-side-table{width:100%}.btn-success{color:#fff;border:1px solid #46992d;background-color:#419129}.btn-primary{color:#fff;border:1px solid #2d5899;background-color:#3464ac}.btn-warning{color:#212529;border:1px solid #ffc107;background-color:#ffc107}.btn-danger{color:#fff;border:1px solid #dc3545;background-color:#dc3545}.row{display:flex;gap:10px}.column,.pad-wrapper{flex:50%;justify-content:space-between}.pad{height:60px;font-size:14px;background-color:#64ffc5;border:1px solid #4ddba6;min-width:20px}.pad::after,.pad::before,.popup,.popup-shadow,a.ctrl::before{position:absolute}.pad:active{background-image:radial-gradient(#98ffd9,#64ffc5)}.pad::before{content:attr(data-number);top:4px}.pad::after{content:attr(data-tt);width:calc(100% - 8px);bottom:4px;text-overflow:ellipsis;overflow-x:hidden}.pad-wrapper .editor,a.ctrl{width:20px;height:20px;display:block}.pad-wrapper .editor{position:absolute;top:10px;right:0}a.ctrl::before{content:'\270E';color:#333;display:block;font-size:15px;transform:rotate(90deg)}.popup-shadow{z-index:1001;min-width:100vw;min-height:100vh;top:0;bottom:0;left:0;right:0;overflow:auto;background-color:rgba(0,0,0,.4);color:#aaa;display:none}.popup,.popup-confirm{left:50%;min-height:100px}.popup{color:#333;background-color:#fff;border:1px solid #7c99c1;display:none;width:480px;margin-left:-240px;top:20px}.popup-confirm{position:fixed;width:320px;margin-left:-160px;top:calc(50vh - 70px);z-index:10}@media screen and (max-width:520px){.popup-w{width:calc(100vw - 40px);left:0;margin-left:20px}}@media screen and (max-width:360px){.popup-confirm{width:calc(100vw - 40px);left:0;margin-left:20px;min-height:100px}}.popup-header{padding:4px 12px;min-height:25px;line-height:25px;background-color:#3464ac;color:#fff}.popup-body{padding:5px 12px;min-height:25px;background-color:#fff}.popup-body p{line-height:1.5}.popup-footer{padding:6px 12px;min-height:25px;background-color:#eee;text-align:end}.two-side-table td{padding:2px 0}.two-side-table tr>td:first-child{width:35%}";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/css", response);
}

// /script.js
void handleScript()
{
    String response = "function _ce(e){return document.createElement(e)}function _nm(e){return document.getElementsByName(e)[0]}function _sl(e){return document.querySelector(e)}function _sls(e){return document.querySelectorAll(e)}Element.prototype.matches||(Element.prototype.matches=Element.prototype.msMatchesSelector||Element.prototype.webkitMatchesSelector),Element.prototype.closest||(Element.prototype.closest=function(e){let n=this;do{if(Element.prototype.matches.call(n,e))return n;n=n.parentElement||n.parentNode}while(null!==n&&1===n.nodeType);return null}),Element.prototype.remove||(Element.prototype.remove=function(e){this.parentNode.removeChild(this)}),Element.prototype.popupShow=function(){this.style.display=\"block\",this.closest(\".popup-shadow\").style.display=\"block\"},Element.prototype.attr=function(e,n){return void 0!==n?this.setAttribute(e,n):this.getAttribute(e)},Element.prototype.popupHide=function(){this.style.display=\"none\",this.closest(\".popup-shadow\").style.display=\"none\"},Element.prototype.on=function(e,n,t){return this.addEventListener(e,n,t)},Element.prototype.off=function(e,n,t){return this.removeEventListener(e,n,t)},Element.prototype.find=function(e){return this.querySelector(e)},Element.prototype.findAll=function(e){return this.querySelectorAll(e)};let ajax={},cMsg=\"Are you sure you want to save the config?\",rMsg=\"Are you sure you want to reset the config?\",pSel=\".popup-confirm\",pTtl=\"User Confirmation\";function customConfim(e,n,t,o,a,s,l){n=n||\"Are you sure?\",t=t||\"Confirm\",o=o||\"OK\",a=a||\"Cancel\";let i=_sl(e);i.find(\".popup-header\").innerText=t,i.find(\".popup-body .message\").innerText=n,i.find(\".btn-ok\")&&i.find(\".btn-ok\").remove(),i.find(\".btn-cancel\")&&i.find(\".btn-cancel\").remove();let u=_ce(\"button\");u.attr(\"class\",\"btn btn-success btn-ok\"),u.innerText=o;let r=_ce(\"button\");r.attr(\"class\",\"btn btn-warning btn-cancel\"),r.innerText=a;let p=i.find(\".popup-footer\");p.appendChild(u),p.appendChild(document.createTextNode(\" \")),p.appendChild(r),i.popupShow(),s=s||function(){i.popupHide()},l=l||function(){i.popupHide()};try{u.off(\"click\"),r.off(\"click\")}catch(c){}u.on(\"click\",function(){s(),i.popupHide()}),r.on(\"click\",function(){l(),i.popupHide()})}function saveConfigWS(){return customConfim(pSel,cMsg,pTtl,\"Yes\",\"No\",function(){ajax.post(\"save-ws.html\",{action:\"save-ws\",wifi_enable:_nm(\"wifi_enable\").value,ssid_name:_nm(\"ssid_name\").value,ssid_password:_nm(\"ssid_password\").value,ws_enable:_nm(\"ws_enable\").value,ws_host:_nm(\"ws_host\").value,ws_port:_nm(\"ws_port\").value,ws_path:_nm(\"ws_path\").value,ws_username:_nm(\"ws_username\").value,ws_password:_nm(\"ws_password\").value},function(e,n,t){console.log(t)},!0)},null),!1}function loadConfigWS(){ajax.get(\"ws-configuration.json\",{},function(e,n,t){try{let o=JSON.parse(e);_nm(\"wifi_enable\").value=o.wifi_enable,_nm(\"ssid_name\").value=o.ssid_name,_nm(\"ssid_password\").value=o.ssid_password,_nm(\"ws_enable\").value=o.ws_enable,_nm(\"ws_scheme\").value=o.ws_scheme,_nm(\"ws_host\").value=o.ws_host,_nm(\"ws_port\").value=o.ws_port,_nm(\"ws_path\").value=o.ws_path,_nm(\"ws_username\").value=o.ws_username,_nm(\"ws_password\").value=o.ws_password}catch(a){}},!0)}function loadConfigAP(){ajax.get(\"ap-configuration.json\",{},function(e,n,t){try{let o=JSON.parse(e);_nm(\"ssid_name\").value=o.ssid_name,_nm(\"ssid_password\").value=o.ssid_password,_nm(\"ip\").value=o.ip,_nm(\"gateway\").value=o.gateway,_nm(\"subnet\").value=o.subnet,_nm(\"hidden\").value=o.hidden}catch(a){}},!0)}function saveConfigAP(){return customConfim(pSel,cMsg,pTtl,\"Yes\",\"No\",function(){ajax.post(\"save-ap.html\",{action:\"save-ap\",ssid_name:_nm(\"ssid_name\").value,ssid_password:_nm(\"ssid_password\").value,ip:_nm(\"ip\").value,gateway:_nm(\"gateway\").value,subnet:_nm(\"subnet\").value,hidden:_nm(\"hidden\").value},function(e,n,t){},!0)},null),!1}function loadConfigGeneral(){ajax.get(\"general-configuration.json\",{},function(e,n,t){try{let o=JSON.parse(e);_nm(\"channel\").value=o.channel,_nm(\"read_interval\").value=o.read_interval,_nm(\"solo_pad\").value=o.solo_pad,_nm(\"solo_pad_number\").value=o.solo_pad_number}catch(a){}},!0)}function loadDefaultGeneral(){customConfim(pSel,rMsg,pTtl,\"Yes\",\"No\",function(){_nm(\"channel\").value=10,_nm(\"read_interval\").value=1e3,_nm(\"solo_pad\").value=0,_nm(\"solo_pad_number\").value=1},null)}function saveConfigGeneral(){return customConfim(pSel,cMsg,pTtl,\"Yes\",\"No\",function(){ajax.post(\"save-general.html\",{channel:_nm(\"channel\").value,read_interval:_nm(\"read_interval\").value,solo_pad:_nm(\"solo_pad\").value,solo_pad_number:_nm(\"solo_pad_number\").value},function(e,n,t){},!0)},null),!1}function handleIP(e){let n=e.target;isValidIP(n.value)?n.classList.remove(\"invalid-ip\"):(n.classList.remove(\"invalid-ip\"),n.classList.add(\"invalid-ip\"))}function initIP(){let e=_sls('input[type=\"ipaddress\"]');if(null!=e&&e.length)for(let n=0;n<e.length;n++)e[n].on(\"keyup\",function(e){handleIP(e)}),e[n].on(\"change\",function(e){handleIP(e)})}function isValidIP(e){if(0==e.length)return!0;let n,t=e.split(\".\");if(4!=t.length)return!1;for(n in t)if(isNaN(parseInt(t[n]))||t[n]<0||255<t[n])return!1;return!0}ajax.create=function(){if(\"undefined\"!=typeof XMLHttpRequest)return new XMLHttpRequest;for(let e,n=[\"MSXML2.XmlHttp.6.0\",\"MSXML2.XmlHttp.5.0\",\"MSXML2.XmlHttp.4.0\",\"MSXML2.XmlHttp.3.0\",\"MSXML2.XmlHttp.2.0\",\"Microsoft.XmlHttp\",],t=0;t<n.length;t++)try{e=new ActiveXObject(n[t]);break}catch(o){}return httpRequest},ajax.send=function(e,n,t,o,a){a=a||!1;let s=ajax.create();s.open(t,e,a),s.onreadystatechange=function(){if(4==s.readyState&&null!=n&&\"function\"==typeof n){let e=n.length;1==e?n(s.responseText):2==e?n(s.responseText,s.status):3==e&&n(s.responseText,s.status,s.statusText)}},\"POST\"==t&&s.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\"),s.send(o)},ajax.get=function(e,n,t,o){let a,s=[];for(a in n)n.hasOwnProperty(a)&&s.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(n[a]));ajax.send(e+(s.length?\"?\"+s.join(\"&\"):\"\"),t,\"GET\",null,o)},ajax.post=function(e,n,t,o){let a,s=[];for(a in n)n.hasOwnProperty(a)&&s.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(n[a]));ajax.send(e,t,\"POST\",s.join(\"&\"),o)};";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/javascript", response);
}

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

    // Configuration WiFi as Access Point
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
        // Configuration WiFi as Workstation
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

    // Now set up two tasks to run independently.
    xTaskCreate(
        Task1, "Task1" // A name just for humans
        ,
        32768 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 3 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL);

    xTaskCreate(
        Task2, "Task2" // A name just for humans
        ,
        5120 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL);

    pinMode(onboardLED, OUTPUT);

    if (sysEnable == "1")
    {
        wsReconnect();
    }

    Serial.println("Device is ready");
}

// /test-note.html
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
// /hit-pad.html
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

void loop(void)
{
    // HTTP Client must be handled by main loop
    server.handleClient();
    delay(2);
}