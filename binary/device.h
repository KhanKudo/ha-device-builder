#define OTA_UPDATE
// start
#include <Arduino.h>
#ifdef OTA_UPDATE
#include <ArduinoOTA.h>
#endif

#ifdef ESP32
const char root_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";
#endif

#include <WiFiClient.h>
#ifdef ESP32
#include <WiFiClientSecure.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <Updater.h>
#include <Hash.h>
#endif

#ifdef TIME
#include <ezTime.h>
#endif

WiFiClient client;
// WiFiClientSecure wifiClientSecure;

#ifdef OTA_UPDATE
enum ServerType
{
    UPDATE_AVAILABLE,
    UPDATE_SIZE,
    CHUNK,
    RESTART_DEVICE,
    ABORT_UPDATE,
    PING,
    TIME
};

enum ClientType
{
    REGISTER, // MUST BE VALUE 0 (zero, null, '\0')
    GET_UPDATE_SIZE,
    GET_FIRST_CHUNK, // determines chunksize
    GET_CHUNK,
    UPDATE_SUCCESSFUL,
    UPDATE_FAILED,
    PONG,
    GET_TIME
};

bool waitingForUpdateSize = false;
bool isUpdating = false;
const uint16_t chunksize = 2000;
uint16_t chunkIndex = 0;
const uint16_t lenRX = 64;
byte otaRX[lenRX];
byte otaTX[5];
uint32_t remainingBytes = 0;
uint32_t timeoutMillis = 0;
#endif

uint8_t readData(byte *buffer, uint16_t bytes)
{
    bytes--;
    timeoutMillis = millis() + 1000;
    while (client.available() < bytes && millis() < timeoutMillis)
        ;

    if (client.available() < bytes)
        return 1;

    client.read(buffer + 1, bytes);
    return 0;
}

void handleMsg(byte *payload)
{
    if (payload[0] == ServerType::PING)
    {
        otaTX[0] = ClientType::PONG;
        client.write(otaTX, 1);
        return;
    }

#ifdef OTA_UPDATE
    if (payload[0] == ServerType::UPDATE_AVAILABLE)
    {
        if (waitingForUpdateSize || isUpdating)
            goto ota_fail;

        // Serial.println("Updating firmware...");
        waitingForUpdateSize = true;

        otaTX[0] = ClientType::GET_UPDATE_SIZE;
        client.write(otaTX, 1);
    }
    else if (payload[0] == ServerType::UPDATE_SIZE)
    {
        if (readData(payload, 5) || !waitingForUpdateSize)
            goto ota_fail;

        remainingBytes = *(uint32_t *)(payload + 1);

        if (!Update.begin(remainingBytes))
            goto ota_fail;

        isUpdating = true;
        waitingForUpdateSize = false;
        otaTX[0] = ClientType::GET_FIRST_CHUNK;
        chunkIndex = 0;
        *(uint16_t *)(otaTX + 1) = chunksize;
        client.write(otaTX, 3);
    }
    else if (payload[0] == ServerType::CHUNK)
    {
        if (readData(payload, 3) || !isUpdating)
            goto ota_fail;

        if (*(uint16_t *)(payload + 1) != chunkIndex)
            goto ota_fail;

        uint16_t datalen = (remainingBytes < (uint32_t)chunksize) ? remainingBytes : chunksize;
        remainingBytes -= datalen;
        uint16_t readable = 0;
        timeoutMillis = millis() + 5000;
        while (datalen > 0 && millis() < timeoutMillis)
        {
            readable = max(lenRX, (uint16_t)client.available());
            if (readable == 0)
                continue;
            client.read(payload, readable);
            if (Update.write(payload, readable) != readable)
            {
                Update.printError(Serial);
                goto ota_fail;
            }
            datalen -= readable;
        }

        if (datalen > 0)
            goto ota_fail;

        if (remainingBytes == 0)
        {
            if (!Update.end())
                goto ota_fail;

            isUpdating = false;
            otaTX[0] = ClientType::UPDATE_SUCCESSFUL;
            client.write(otaTX, 1);
        }
        else
        {
            otaTX[0] = ClientType::GET_CHUNK;
            chunkIndex++;
            *(uint16_t *)(otaTX + 1) = chunkIndex;
            client.write(otaTX, 3);
        }
    }
    else if (payload[0] == ServerType::RESTART_DEVICE)
    {
#ifdef ESP32
        if (isUpdating)
            Update.abort();
#endif
        // Serial.printf("RESTART_DEVICE command received\n");
        // Serial.printf("Restarting...\n");

        client.stop();
        // WiFi.reconnect();
        ESP.restart();
    }
    else if (payload[0] == ServerType::ABORT_UPDATE)
    {
#ifdef ESP32
        if (isUpdating)
            Update.abort();
#endif
        isUpdating = false;
        waitingForUpdateSize = false;
        // Serial.printf("ABORT_UPDATE command received\n");
    }

    return;
ota_fail:
    if (isUpdating)
    {
#ifdef ESP32
        Update.abort();
#endif
        isUpdating = false;
    }

    waitingForUpdateSize = false;
    otaTX[0] = ClientType::UPDATE_FAILED;
    client.write(otaTX, 1);
    return;
#endif
};

void reconnect()
{
    if (!client.connected())
    {
        client.flush();
        Serial.print("Connecting client...");
        if (!client.connect(IPAddress(192, 168, 0, 20), 3000, 5000))
        {
            Serial.println("failed!");
        }
        else
        {
            Serial.println("success!");
            // register device right away
            // client.write("\0"
            //              "DEVICE_ID_MD5_HASHED_HEXADECIMAL"
            //              "0000000000000000",
            //              49);
            client.write("\0"
                         "65fa9377f6a758474b66e9719f43de39"
                         "0000000000000000",
                         49);
        }
    }
}

const char *_ssid;
const char *_password;
const char *_broker;
bool _isEncrypted;
uint _port;
const char *_user;
const char *_pass;

uint32_t restartTimeout = 0;

#ifdef TIME
Timezone time;
#endif

void device_init(const char *ssid = "wifi-user", const char *password = "wifi-pass", const char *broker = "example.com", bool isEncrypted = true, uint port = 8885, const char *user = "mqtt-user", const char *pass = "mqtt-pass")
{
    _ssid = ssid;
    _password = password;
    _broker = broker;
    _isEncrypted = isEncrypted;
    _port = port;
    _user = user;
    _pass = pass;

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.setHostname("ha-device");
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi.");
    //TODO: add timeout -> restart
    while (!WiFi.isConnected())
    {
        Serial.print(".");
        delay(250);
    }
    Serial.println("done!");

#ifndef TIME
    while (!WiFi.isConnected())
    {
        delay(100);
    }
#else
    waitForSync();
    time.setLocation("Europe/Vienna");
#endif

#ifdef OTA_UPDATE
    // ArduinoOTA.setHostname("ha-device");
    ArduinoOTA.begin();
#endif

    client.setNoDelay(true);

    reconnect();
}

void device_loop()
{
    if (!WiFi.isConnected())
    {
        if (restartTimeout == 0)
        {
            restartTimeout = millis() + 5 * 60 * 1000; // 5 Minutes
        }

        WiFi.begin(_ssid, _password);

        WiFi.waitForConnectResult();
    }

    if (restartTimeout > 0 && millis() > restartTimeout)
    {
        ESP.restart();
    }

    if (!WiFi.isConnected())
        return;

    if (!client.connected())
    {
#ifdef OTA_UPDATE
        if (waitingForUpdateSize || isUpdating)
        {
#ifdef ESP32
            if (isUpdating)
                Update.abort();
#endif
            waitingForUpdateSize = false;
            isUpdating = false;
        }
#endif

        if (restartTimeout == 0)
        {
            restartTimeout = millis() + 5 * 60 * 1000; // 5 Minutes
        }

        reconnect();
    }
    else
    {
        if (restartTimeout != 0)
        {
            restartTimeout = 0;
        }
    }

    if (client.available())
    {
        client.read(otaRX, 1);
        handleMsg(otaRX);
    }

#ifdef OTA_UPDATE
    ArduinoOTA.handle();
#endif

#ifdef TIME
    events();
#endif
}