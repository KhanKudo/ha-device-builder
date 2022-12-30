// start
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

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

    std::function<void(void)> listener = []() {};

    std::map<String, std::function<void(String)>> listeners;

    std::function<void(void)> onServerOnlineListener = []() {};
    std::function<void(void)> onServerOfflineListener = []() {};

    std::map<uint32_t, std::function<void(void)>> timeouts;

    WiFiClient wifiClient;
    WiFiClientSecure wifiClientSecure;
    PubSubClient client;

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
            client = PubSubClient(wifiClientSecure);
            wifiClientSecure.setInsecure();
        }
        else
        {
            client = PubSubClient(wifiClient);
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

        reconnect();

        // __insert-discovery-publish
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