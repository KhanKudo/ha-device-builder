// start
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

#define MD5_HASH_SIZE 16
#include <esp_https_ota.h>

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

#include <functional>
#include <map>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#ifdef TIME
#include <ezTime.h>
#endif

struct _HA_DEVICE
{
private:
    bool serverStatus = true;

    String name = "NAME";
    String codeName = "CODE_NAME";
    String id = "DEVICE_ID";

    std::function<void(void)> listener = []() {};

    std::map<String, std::function<void(String)>> listeners;

    std::function<void(void)> onServerOnlineListener = []() {};
    std::function<void(void)> onServerOfflineListener = []() {};

    std::map<uint32_t, std::function<void(void)>> timeouts;

    WiFiClient wifiClient;
    WiFiClientSecure wifiClientSecure;
    PubSubClient client = PubSubClient(wifiClient);

    String availabilityTopic = "AVAILABILITY_TOPIC";

    uint32_t timeoutLoopLimiter = 0;

    std::function<void(char *, byte *, unsigned int)> callback = [this](char *char_topic, byte *payload, unsigned int length)
    {
        String topic = char_topic;
        String message = "";

        for (int i = 0; i < length; i++)
        {
            message += (char)payload[i];
        }

        listeners.at(topic)(message);
    };

    void reconnect()
    {
        if (!client.connected())
        {
            if ((availabilityTopic == "" && client.connect(codeName.c_str(), _user, _pass)) ||
                client.connect(codeName.c_str(),
                               _user,
                               _pass,
                               availabilityTopic.c_str(),
                               0,
                               true,
                               "offline"))
            {
                connected();
            }
        }
    }

    void connected()
    {
        client.subscribe("homeassistant/status");

        for (auto it = listeners.cbegin(); it != listeners.cend(); it++)
        {
            client.subscribe(it->first.c_str());
        }

        if (availabilityTopic != "")
        {
            client.publish(availabilityTopic.c_str(), "online", true);
        }
    }

    void updateFirmware()
    {
        Serial.println("Updating firmware...");

        String url = String("https://") + String(_broker) + String("/update?id=") + id;

        esp_http_client_config_t config = {
            .url = url.c_str(),
            .auth_type = HTTP_AUTH_TYPE_NONE,
            .cert_pem = root_ca,
            .method = HTTP_METHOD_GET,
        };

        esp_err_t err = esp_https_ota(&config);

        if (err == ESP_OK)
        {
            Serial.println("OTA update successful");
            esp_restart();
        }
        else
        {
            Serial.println("OTA update failed");
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

public:
#ifdef TIME
    Timezone time;
#endif

    void init(const char *ssid = "wifi-user", const char *password = "wifi-pass", const char *broker = "example.com", bool isEncrypted = true, uint port = 8885, const char *user = "user-fFXBzVQtm9NxQJmjc7F4CCVdowi7sDp4Js7q8g3jxKyZjddeVEUe7vxqxrmQUkDPax7MkJfwLabUHKjzdftuYYdbYavuCsPyJtjvFKfsak5bsksQ4ZPWD3bKb9QU6PZQ", const char *pass = "3oE5thSHgyK6DjMbFSyNCDZUAwrKQp6Q5cL3pEBLGXtzmJDaXm7keYmWi25dRRJUsouxrAN8tjnV4FZu74NbFAgUiAnFkXeiRBqPPgauhdmTbSBbLzZrxPyKv9g4oFwj")
    {
        _ssid = ssid;
        _password = password;
        _broker = broker;
        _isEncrypted = isEncrypted;
        _port = port;
        _user = user;
        _pass = pass;

        if (isEncrypted)
        {
            client.setClient(wifiClientSecure);
            wifiClientSecure.setInsecure();
        }

        WiFi.begin(ssid, password);

#ifndef TIME
        while (!WiFi.isConnected())
        {
            delay(100);
        }
#else
        waitForSync();
        time.setLocation("Europe/Vienna");
#endif

        ArduinoOTA.setHostname(codeName.c_str());
        ArduinoOTA.begin();

        client.setServer(broker, port);
        client.setCallback(callback);

        client.setBufferSize(1023);

        subscribe("homeassistant/status", [this](String status)
                  {
            bool newStatus = false;
            if(status == "online") {
                newStatus = true;
            }
            else if (status == "offline") {
                newStatus = false;
            }

            if (serverStatus != newStatus) {
                serverStatus = newStatus;
                if(newStatus)
                    onServerOnlineListener();
                else
                    onServerOfflineListener();
            }
            /**/ });

        subscribe("device-version-manager/update-available-for", [this](String deviceId)
                  {
            if (!deviceId.equals(id))
                return;

            updateFirmware();
            /**/ });

        reconnect();

        // __insert-discovery-publish

        String hash = String("HA_DEVICE_PLACEHOLDER_HASH");

        publish(("device-version-manager/register/" + id).c_str(), (hash + " " + availabilityTopic).c_str(), true);
    }

    void loop()
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
            if (restartTimeout == 0)
            {
                restartTimeout = millis() + 5 * 60 * 1000; // 5 Minutes
            }

            reconnect();
        }
        else
        {
            if (restartTimeout == 0)
            {
                restartTimeout = 0;
            }
        }

        client.loop();

        ArduinoOTA.handle();

#ifdef TIME
        events();
#endif

        if (timeouts.size() > 0 && millis() > timeoutLoopLimiter)
        {
            uint32_t *passed = new uint32_t(timeouts.size());
            const uint32_t cur_ms = millis();
            timeoutLoopLimiter = cur_ms + 100;
            int count = 0;
            for (auto it = timeouts.cbegin(); it != timeouts.cend(); ++it)
            {
                yield();
                if (cur_ms > it->first)
                {
                    it->second();
                    passed[count] = it->first;
                    count++;
                }
            }

            while (--count >= 0)
            {
                timeouts.erase(passed[count]);
            }
        }
    }

    void subscribe(const char *topic, std::function<void(String)> listener)
    {
        listeners.insert(std::pair<String, std::function<void(String)>>(String(topic), listener));
    }

    void publish(const char *topic, const char *message, bool retain = false)
    {
        client.publish(topic, message, retain);
    }

    void clearRetain(const char *topic)
    {
        client.publish(topic, "", true);
    }

    // returns true for online and false for offline
    bool getServerStatus()
    {
        return serverStatus;
    }

    void onServerOnline(std::function<void(void)> listener)
    {
        onServerOnlineListener = listener;
    }

    void onServerOffline(std::function<void(void)> listener)
    {
        onServerOfflineListener = listener;
    }

    // calls the callback after roughly the specified time in ms has passed
    void setTimeout(uint16_t ms, std::function<void(void)> callback)
    {
        timeouts.insert(std::pair<uint32_t, std::function<void(void)>>(millis() + ms, callback));
    }
} device;