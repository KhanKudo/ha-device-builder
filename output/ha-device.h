#include <Arduino.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <functional>
#include <map>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#ifdef TIME
#include <ezTime.h>
#endif

struct
{
private:
    String name = "Lights";
    String codeName = "lights";

    std::function<void(void)> listener = []() {};

    std::map<String, std::function<void(String)>> listeners;

    WiFiClient wifiClient;
    PubSubClient client = PubSubClient(wifiClient);

    String availabilityTopics[1] = {
        // __insert-availability_topics
    };

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
        while (!client.connected())
        {
            if (!client.connect(codeName, "user-iEQFaFF3N9afa7EkVVv9qTdSjdBLavsxizbr3hGo9eHEWHVqiq8dBdshtvQEL7kGwR6R3jEuh5Scnf7YmZzS4UbsAjqnsdyLio3L8XbHfB9Hqfbm8Q8PTxTRAzm76tH2", "w9NHNkrpKwUEeGm9uSE9mH33ci97oQhBrfnjsZb78wEqq7bsnCZkxCcyzJeF6k3u8pedwiHERerCcCaC6NE7oQt9vpEPrpYDLPeQA6QFq8AsKYKBUdCkWFvycAJ5kpdi"))
            {
                delay(5000);
                ESP.restart();
            }
        }

        connected();
    }

    void connected()
    {
        for (auto it = listeners.cbegin(); it != listeners.cend(); it++)
        {
            client.subscribe(it->first);
        }
    }

    IPAddress mqttBrokerIP;

public:
#ifdef TIME
    Timezone time;
#endif

    void init(String ssid = "wifi-user", String password = "wifi-pass")
    {
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

#ifdef WAN_DEPLOYMENT
        WiFi.hostByName("milenkovic.ddns.net", mqttBrokerIP);
#else
        mqttBrokerIP = IPAddress(192, 168, 0, 51);
#endif
        client.setServer(mqttBrokerIP, 1883);
        client.setCallback(callback);

        client.setBufferSize(511);

        reconnect();
        ArduinoOTA.setHostname(codeName);
        ArduinoOTA.begin();

        // __insert-discovery-publish
    }

    void loop()
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();
        ArduinoOTA.handle();
#ifdef TIME
        events();
#endif
    }

    void subscribe(String topic, std::function<void(String)> listener)
    {
        listeners.insert_or_assign(topic, listener);
    }
} manager;

class _Button
{
private:
    String name = "My Button";

    String commandTopic = "homeassistant/button/my-button-1/command";

    std::function<void(void)> listener = []() {};

public:
    _Button()
    {
        manager.subscribe(commandTopic, [this](String message)
                          { listener(); });
    }

    // only one listener will work, newest overwrites previous
    void onPress(std::function<void(void)> _listener)
    {
        listener = _listener;
    }
} my_button;

