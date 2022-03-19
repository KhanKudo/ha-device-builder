#define String char *
#define IPAddress char *
#define byte char
#define NUMBER_OF_AVAILABILITY_TOPICS 1

class WiFiClient
{
    WiFiClient()
    {
    }
};

class PubSubClient
{
public:
    PubSubClient() {}
    PubSubClient(WiFiClient wifiClient) {}

    void publish(String topic, String message) {}
    void subscribe(String topic) {}
    bool connect(String id, String user, String pass) { return false; }
    bool connected() { return false; }
    void loop() {}
    void setServer(IPAddress, int) {}
    void setCallback(std::function<void(char *, byte *, unsigned int)>) {}
    void setBufferSize(unsigned int) {}
};

void delay(int ms)
{
}

struct
{
    void restart() {}
} ESP;

struct
{
    void hostByName(String hostname, IPAddress destinationIP) {}
    void begin(String ssid, String password) {}
    bool isConnected() { return false; }
} WiFi;

struct
{
    String c_str() { return ""; }
} codeName;

#define WAN_DEPLOYMENT

// start
// uncomment:#include <Arduino.h>
// uncomment:#include <ArduinoOTA.h>
// uncomment:#include <PubSubClient.h>
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
    String name = "NAME";
    // uncomment:String codeName = "CODE_NAME";

    std::function<void(void)> listener = []() {};

    std::map<String, std::function<void(String)>> listeners;

    WiFiClient wifiClient;
    PubSubClient client = PubSubClient(wifiClient);

    String availabilityTopics[NUMBER_OF_AVAILABILITY_TOPICS] = {
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
            if (!client.connect(codeName.c_str(), "user-iEQFaFF3N9afa7EkVVv9qTdSjdBLavsxizbr3hGo9eHEWHVqiq8dBdshtvQEL7kGwR6R3jEuh5Scnf7YmZzS4UbsAjqnsdyLio3L8XbHfB9Hqfbm8Q8PTxTRAzm76tH2", "w9NHNkrpKwUEeGm9uSE9mH33ci97oQhBrfnjsZb78wEqq7bsnCZkxCcyzJeF6k3u8pedwiHERerCcCaC6NE7oQt9vpEPrpYDLPeQA6QFq8AsKYKBUdCkWFvycAJ5kpdi"))
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
            // uncomment:client.subscribe(it->first.c_str());
        }
    }

    IPAddress mqttBrokerIP;

public:
#ifdef TIME
    Timezone time;
#endif

    void init(char *ssid = "wifi-user", char *password = "wifi-pass")
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
        // uncomment:ArduinoOTA.setHostname(codeName.c_str());
        // uncomment:ArduinoOTA.begin();

        // __insert-discovery-publish
    }

    void loop()
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();
        // uncomment:ArduinoOTA.handle();
#ifdef TIME
        events();
#endif
    }

    void subscribe(char *topic, std::function<void(String)> listener)
    {
        // uncomment:listeners.insert(std::pair<String, std::function<void(String)>>(String(topic), listener));
    }
} manager;