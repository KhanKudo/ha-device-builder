// start
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

struct _HA_DEVICE
{
private:
    String name = "NAME";
    String codeName = "CODE_NAME";

    std::function<void(void)> listener = []() {};

    std::map<String, std::function<void(String)>> listeners;

    WiFiClient wifiClient;
    PubSubClient client = PubSubClient(wifiClient);

    String availabilityTopic = "AVAILABILITY_TOPIC";

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
            if ((availabilityTopic == "" && !client.connect(codeName.c_str(),
                                                            "user-iEQFaFF3N9afa7EkVVv9qTdSjdBLavsxizbr3hGo9eHEWHVqiq8dBdshtvQEL7kGwR6R3jEuh5Scnf7YmZzS4UbsAjqnsdyLio3L8XbHfB9Hqfbm8Q8PTxTRAzm76tH2",
                                                            "w9NHNkrpKwUEeGm9uSE9mH33ci97oQhBrfnjsZb78wEqq7bsnCZkxCcyzJeF6k3u8pedwiHERerCcCaC6NE7oQt9vpEPrpYDLPeQA6QFq8AsKYKBUdCkWFvycAJ5kpdi")) ||
                !client.connect(codeName.c_str(),
                                "user-iEQFaFF3N9afa7EkVVv9qTdSjdBLavsxizbr3hGo9eHEWHVqiq8dBdshtvQEL7kGwR6R3jEuh5Scnf7YmZzS4UbsAjqnsdyLio3L8XbHfB9Hqfbm8Q8PTxTRAzm76tH2",
                                "w9NHNkrpKwUEeGm9uSE9mH33ci97oQhBrfnjsZb78wEqq7bsnCZkxCcyzJeF6k3u8pedwiHERerCcCaC6NE7oQt9vpEPrpYDLPeQA6QFq8AsKYKBUdCkWFvycAJ5kpdi",
                                availabilityTopic.c_str(),
                                0,
                                true,
                                "offline"))
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
            client.subscribe(it->first.c_str());
        }

        if (availabilityTopic != "")
        {
            client.publish(availabilityTopic.c_str(), "online", true);
        }
    }

    IPAddress mqttBrokerIP;

public:
#ifdef TIME
    Timezone time;
#endif

    void init(const char *ssid = "wifi-user", const char *password = "wifi-pass")
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

        WiFi.hostByName("example.com", mqttBrokerIP);
        client.setServer(mqttBrokerIP, 1883);
        client.setCallback(callback);

        client.setBufferSize(1023);

        reconnect();
        ArduinoOTA.setHostname(codeName.c_str());
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
} device;