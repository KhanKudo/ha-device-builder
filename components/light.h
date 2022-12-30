#include "manager.h"
#include <Arduino.h>

#define RETAIN false
#define NUMBER_OF_EFFECTS 0

// start

#include <ArduinoJson.h>

// start-if rgb_supported
#ifndef DEF_RGB
#define DEF_RGB
struct Color_RGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
#endif
// end-if rgb_supported

// start-if rgbw_supported
#ifndef DEF_RGBW
#define DEF_RGBW
struct Color_RGBW
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t w;
};
#endif
// end-if rgbw_supported

// start-if rgbww_supported
#ifndef DEF_RGBWW
#define DEF_RGBWW
struct Color_RGBWW
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t c;
    uint8_t w;
};
#endif
// end-if rgbww_supported

// start-if hs_supported
#ifndef DEF_HS
#define DEF_HS
struct HS
{
    double h;
    double s;
};
#endif
// end-if hs_supported

// start-if xy_supported
#ifndef DEF_XY
#define DEF_XY
struct XY
{
    double x;
    double y;
};
#endif
// end-if xy_supported

class _VAR_NAME
{
private:
    const String name = "NAME";

    const String commandTopic = "COMMAND_TOPIC";
    const String stateTopic = "STATE_TOPIC";

    const bool retain = RETAIN;

    bool initialSetup = true;

    std::function<void(bool)> state_listener = [](bool) {};

    bool state = false;

    // start-if brightness_supported
    uint8_t brightness = 0;
    std::function<void(uint8_t)> brightness_listener = [](uint8_t) {};
    // end-if brightness_supported

    // start-if color_temp_supported
    uint16_t color_temp = 0;
    std::function<void(uint16_t)> color_temp_listener = [](uint16_t) {};
    // end-if color_temp_supported

    // start-if rgb_supported
    Color_RGB rgb = {0, 0, 0};
    std::function<void(Color_RGB)> rgb_listener = [](Color_RGB) {};
    // end-if rgb_supported

    // start-if rgbw_supported
    Color_RGBW rgbw = {0, 0, 0, 0};
    std::function<void(Color_RGBW)> rgbw_listener = [](Color_RGBW) {};
    // end-if rgbw_supported

    // start-if rgbww_supported
    Color_RGBWW rgbww = {0, 0, 0, 0, 0};
    std::function<void(Color_RGBWW)> rgbww_listener = [](Color_RGBWW) {};
    // end-if rgbww_supported

    // start-if hs_supported
    HS hs = {0, 0};
    std::function<void(HS)> hs_listener = [](HS) {};
    // end-if hs_supported

    // start-if xy_supported
    XY xy = {0, 0};
    std::function<void(XY)> xy_listener = [](XY) {};
    // end-if xy_supported

    // start-if effects_supported
public:
    enum Effect
    {
        // __insert-effect-list-enum
    };

private:
    Effect effect;
    std::function<void(Effect)> effect_listener = [](Effect) {};
    // end-if effects_supported

    StaticJsonDocument<256> jsonMsg;

    StaticJsonDocument<256> jsonState;
    StaticJsonDocument<256> jsonRetainedCommand;

public:
    _VAR_NAME()
    {
        if (!retain)
        {
            device.clearRetain(commandTopic.c_str());
            device.clearRetain(stateTopic.c_str());
        }

        device.subscribe(commandTopic.c_str(), [this](String message)
                         {
                            Serial.println(message);
                            deserializeJson(jsonMsg, message);

                            jsonState.clear();

                            if(jsonMsg.containsKey("state")){
                                if(jsonMsg["state"] != "ON" && jsonMsg["state"] != "OFF") return;

                                state = jsonMsg["state"] == "ON";
                                jsonState["state"] = jsonMsg["state"];
                            }

                            // start-if brightness_supported
                            if(jsonMsg.containsKey("brightness")){
                                brightness = jsonMsg["brightness"];
                                jsonState["brightness"] = jsonMsg["brightness"];
                            }
                            // end-if brightness_supported

                            // start-if color_temp_supported
                            if(jsonMsg.containsKey("color_temp")){
                                color_temp = jsonMsg["color_temp"];
                                jsonState["color_temp"] = jsonMsg["color_temp"];
                                jsonState["color_mode"] = "color_temp";
                            }
                            // end-if color_temp_supported

                            // start-if rgb_supported
                            if(jsonMsg.containsKey("color")){
                                rgb = {jsonMsg["color"]["r"], jsonMsg["color"]["g"], jsonMsg["color"]["b"]};
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "rgb";
                            }
                            // end-if rgb_supported

                            // start-if rgbw_supported
                            if(jsonMsg.containsKey("color")){
                                rgbw = {jsonMsg["color"]["r"], jsonMsg["color"]["g"], jsonMsg["color"]["b"], jsonMsg["color"]["w"]};
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "rgbw";
                            }
                            // end-if rgbw_supported

                            // start-if rgbww_supported
                            if(jsonMsg.containsKey("color")){
                                rgbww = {jsonMsg["color"]["r"], jsonMsg["color"]["g"], jsonMsg["color"]["b"], jsonMsg["color"]["c"], jsonMsg["color"]["w"]};
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "rgbww";
                            }
                            // end-if rgbww_supported

                            // start-if hs_supported
                            if(jsonMsg.containsKey("color")){
                                hs = {jsonMsg["color"]["h"], jsonMsg["color"]["s"]};
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "hs";
                            }
                            // end-if hs_supported

                            // start-if xy_supported
                            if(jsonMsg.containsKey("color")){
                                xy = {jsonMsg["color"]["x"], jsonMsg["color"]["y"]};
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "xy";
                            }
                            // end-if xy_supported

                            // start-if effects_supported
                            if(jsonMsg.containsKey("effect")){
                                effect = stringToEffect(jsonMsg["effect"]);
                                jsonState["effect"] = jsonMsg["effect"];
                            }
                            // end-if effects_supported

                            if(jsonMsg.containsKey("retain-recovery") && jsonMsg["retain-recovery"] == true){
                                if(initialSetup)
                                    initialSetup = false;
                                else
                                    return;
                            }

                            // start-if color_temp_supported
                            if(jsonMsg.containsKey("color_temp")){
                                color_temp_listener(color_temp);
                            }
                            // end-if color_temp_supported

                            // start-if rgb_supported
                            if(jsonMsg.containsKey("color")){
                                rgb_listener(rgb);
                            }
                            // end-if rgb_supported

                            // start-if rgbw_supported
                            if(jsonMsg.containsKey("color")){
                                rgbw_listener(rgbw);
                            }
                            // end-if rgbw_supported

                            // start-if rgbww_supported
                            if(jsonMsg.containsKey("color")){
                                rgbww_listener(rgbww);
                            }
                            // end-if rgbww_supported

                            // start-if hs_supported
                            if(jsonMsg.containsKey("color")){
                                hs_listener(hs);
                            }
                            // end-if hs_supported

                            // start-if xy_supported
                            if(jsonMsg.containsKey("color")){
                                xy_listener(xy);
                            }
                            // end-if xy_supported

                            // start-if effects_supported
                            if(jsonMsg.containsKey("effect")){
                                effect_listener(effect);
                            }
                            // end-if effects_supported

                            // start-if brightness_supported
                            if(jsonMsg.containsKey("brightness")){
                                brightness_listener(brightness);
                            }
                            // end-if brightness_supported

                            if(jsonMsg.containsKey("state")){
                                state_listener(state);
                            }

                            char responseMsg[256];
                            serializeJson(jsonState, responseMsg, 256);
                            device.publish(stateTopic.c_str(), responseMsg, retain);

                            if(retain){
                                jsonRetainedCommand["state"] = state ? "ON" : "OFF";
                            // start-if brightness_supported
                                jsonRetainedCommand["brightness"] = brightness;
                            // end-if brightness_supported
                            // start-if color_temp_supported
                                jsonRetainedCommand["color_temp"] = color_temp;
                            // end-if color_temp_supported
                            // start-if rgb_supported
                                jsonRetainedCommand["color"]["r"] = rgb.r;
                                jsonRetainedCommand["color"]["g"] = rgb.g;
                                jsonRetainedCommand["color"]["b"] = rgb.b;
                            // end-if rgb_supported
                            // start-if rgbw_supported
                                jsonRetainedCommand["color"]["r"] = rgbw.r;
                                jsonRetainedCommand["color"]["g"] = rgbw.g;
                                jsonRetainedCommand["color"]["b"] = rgbw.b;
                                jsonRetainedCommand["color"]["w"] = rgbw.w;
                            // end-if rgbw_supported
                            // start-if rgbww_supported
                                jsonRetainedCommand["color"]["r"] = rgbww.r;
                                jsonRetainedCommand["color"]["g"] = rgbww.g;
                                jsonRetainedCommand["color"]["b"] = rgbww.b;
                                jsonRetainedCommand["color"]["c"] = rgbww.c;
                                jsonRetainedCommand["color"]["w"] = rgbww.w;
                            // end-if rgbww_supported
                            // start-if hs_supported
                                jsonRetainedCommand["color"]["h"] = hs.h;
                                jsonRetainedCommand["color"]["s"] = hs.s;
                            // end-if hs_supported
                            // start-if xy_supported
                                jsonRetainedCommand["color"]["x"] = xy.x;
                                jsonRetainedCommand["color"]["y"] = xy.y;
                            // end-if xy_supported
                            // start-if effects_supported
                                jsonRetainedCommand["effect"] = effectToString(effect);
                            // end-if effects_supported

                                jsonRetainedCommand["retain-recovery"] = true;
                                serializeJson(jsonRetainedCommand, responseMsg, 256);
                                device.publish(commandTopic.c_str(), responseMsg, true);
                            }

                            if(jsonMsg.containsKey("flash")){
                                device.setTimeout(((uint32_t)jsonMsg["flash"]) * 1000, [this](){
                                    device.publish(commandTopic.c_str(), "{\"state\":\"OFF\"}", retain);
                                });
                            } });
    }

    bool getState()
    {
        return state;
    }

    void setState(bool newState)
    {
        StaticJsonDocument<32> jsonDoc;
        jsonDoc["state"] = newState ? "ON" : "OFF";

        char message[32];
        serializeJson(jsonDoc, message, 32);
        jsonDoc.clear();
        device.publish(commandTopic.c_str(), message, retain);
    }

    // only one listener will work, newest overwrites previous
    void onState(std::function<void(bool)> _listener)
    {
        state_listener = _listener;
    }

    // start-if brightness_supported
    uint8_t getBrightness()
    {
        return brightness;
    }

    void setBrightness(uint8_t newBrightness)
    {
        bool newState = newBrightness > 0;

        StaticJsonDocument<64> jsonDoc;
        jsonDoc["state"] = newState ? "ON" : "OFF";

        jsonDoc["brightness"] = newBrightness;

        char message[64];
        serializeJson(jsonDoc, message, 64);
        jsonDoc.clear();
        device.publish(commandTopic.c_str(), message, retain);
    }

    // only one listener will work, newest overwrites previous
    void onBrightness(std::function<void(uint8_t)> _listener)
    {
        brightness_listener = _listener;
    }
    // end-if brightness_supported

    // start-if color_temp_supported
    uint16_t getColorTemp()
    {
        return color_temp;
    }

    // does account for brightness
    uint8_t getCold()
    {
        int ratio = map(color_temp, 153, 500, 0, 511);
        return (float)min(255, 511 - ratio) * (float)brightness / 255.0;
    }

    // does account for brightness
    uint8_t getWarm()
    {
        int ratio = map(color_temp, 153, 500, 0, 511);
        return (float)min(255, ratio) * (float)brightness / 255.0;
    }

    void setColorTemp(uint16_t newColorTemp)
    {
        bool newState = newColorTemp > 0;

        StaticJsonDocument<64> jsonDoc;
        jsonDoc["state"] = newState ? "ON" : "OFF";

        jsonDoc["color_mode"] = "color_temp";

        jsonDoc["color_temp"] = newColorTemp;

        char message[64];
        serializeJson(jsonDoc, message, 64);
        jsonDoc.clear();
        device.publish(commandTopic.c_str(), message, retain);
    }

    // only one listener will work, newest overwrites previous
    void onColorTemp(std::function<void(uint16_t)> _listener)
    {
        color_temp_listener = _listener;
    }
    // end-if color_temp_supported

    // start-if rgb_supported
    Color_RGB getRGB()
    {
        return rgb;
    }

    // brightness already accounted for
    Color_RGB getFinalRGB()
    {
        return {(uint8_t)((float)brightness * ((float)rgb.r / 255.0f)), (uint8_t)((float)brightness * ((float)rgb.g / 255.0f)), (uint8_t)((float)brightness * ((float)rgb.b / 255.0f))};
    }

    void setRGB(Color_RGB _rgb)
    {
        bool newState = _rgb.r > 0 || _rgb.g > 0 || _rgb.b > 0;

        StaticJsonDocument<96> jsonDoc;
        jsonDoc["state"] = newState ? "ON" : "OFF";

        jsonDoc["color_mode"] = "rgb";

        jsonDoc["color"]["r"] = _rgb.r;
        jsonDoc["color"]["g"] = _rgb.g;
        jsonDoc["color"]["b"] = _rgb.b;

        char message[96];
        serializeJson(jsonDoc, message, 96);
        jsonDoc.clear();
        device.publish(commandTopic.c_str(), message, retain);
    }

    // only one listener will work, newest overwrites previous
    void onRGB(std::function<void(Color_RGB)> _listener)
    {
        rgb_listener = _listener;
    }
    // end-if rgb_supported

    // start-if rgbw_supported
    Color_RGBW getRGBW()
    {
        return rgbw;
    }

    // brightness already accounted for
    Color_RGBW getFinalRGBW()
    {
        return {(uint8_t)((float)brightness * ((float)rgbw.r / 255.0f)), (uint8_t)((float)brightness * ((float)rgbw.g / 255.0f)), (uint8_t)((float)brightness * ((float)rgbw.b / 255.0f)), (uint8_t)((float)brightness * ((float)rgbw.w / 255.0f))};
    }

    void setRGBW(Color_RGBW _rgbw)
    {
        bool newState = _rgbw.r > 0 || _rgbw.g > 0 || _rgbw.b > 0 || _rgbw.w > 0;

        StaticJsonDocument<96> jsonDoc;
        jsonDoc["state"] = newState ? "ON" : "OFF";

        jsonDoc["color_mode"] = "rgbw";

        jsonDoc["color"]["r"] = _rgbw.r;
        jsonDoc["color"]["g"] = _rgbw.g;
        jsonDoc["color"]["b"] = _rgbw.b;
        jsonDoc["color"]["w"] = _rgbw.w;

        char message[96];
        serializeJson(jsonDoc, message, 96);
        jsonDoc.clear();
        device.publish(commandTopic.c_str(), message, retain);
    }

    // only one listener will work, newest overwrites previous
    void onRGBW(std::function<void(Color_RGBW)> _listener)
    {
        rgbw_listener = _listener;
    }
    // end-if rgbw_supported

    // start-if rgbww_supported
    Color_RGBWW getRGBWW()
    {
        return rgbww;
    }

    // brightness already accounted for
    Color_RGBWW getFinalRGBWW()
    {
        return {(uint8_t)((float)brightness * ((float)rgbww.r / 255.0f)), (uint8_t)((float)brightness * ((float)rgbww.g / 255.0f)), (uint8_t)((float)brightness * ((float)rgbww.b / 255.0f)), (uint8_t)((float)brightness * ((float)rgbww.c / 255.0f)), (uint8_t)((float)brightness * ((float)rgbww.w / 255.0f))};
    }

    void setRGBWW(Color_RGBWW _rgbww)
    {
        bool newState = _rgbww.r > 0 || _rgbww.g > 0 || _rgbww.b > 0 || _rgbww.c > 0 || _rgbww.w > 0;

        StaticJsonDocument<96> jsonDoc;
        jsonDoc["state"] = newState ? "ON" : "OFF";

        jsonDoc["color_mode"] = "rgbww";

        jsonDoc["color"]["r"] = _rgbww.r;
        jsonDoc["color"]["g"] = _rgbww.g;
        jsonDoc["color"]["b"] = _rgbww.b;
        jsonDoc["color"]["c"] = _rgbww.c;
        jsonDoc["color"]["w"] = _rgbww.w;

        char message[96];
        serializeJson(jsonDoc, message, 96);
        jsonDoc.clear();
        device.publish(commandTopic.c_str(), message, retain);
    }

    // only one listener will work, newest overwrites previous
    void onRGBWW(std::function<void(Color_RGBWW)> _listener)
    {
        rgbww_listener = _listener;
    }
    // end-if rgbww_supported

    // start-if hs_supported
    HS getHS()
    {
        return hs;
    }

    void setHS(HS _hs)
    {
        bool newState = _hs.h > 0 || _hs.s;

        StaticJsonDocument<96> jsonDoc;
        jsonDoc["state"] = newState ? "ON" : "OFF";

        jsonDoc["color_mode"] = "hs";

        jsonDoc["color"]["h"] = _hs.h;
        jsonDoc["color"]["s"] = _hs.s;

        char message[96];
        serializeJson(jsonDoc, message, 96);
        jsonDoc.clear();
        device.publish(commandTopic.c_str(), message, retain);
    }

    // only one listener will work, newest overwrites previous
    void onHS(std::function<void(HS)> _listener)
    {
        hs_listener = _listener;
    }
    // end-if hs_supported

    // start-if xy_supported
    XY getXY()
    {
        return xy;
    }

    void setXY(XY _xy)
    {
        bool newState = _xy.x > 0 || _xy.y > 0;

        StaticJsonDocument<96> jsonDoc;
        jsonDoc["state"] = newState ? "ON" : "OFF";

        jsonDoc["color_mode"] = "xy";

        jsonDoc["color"]["x"] = _xy.x;
        jsonDoc["color"]["y"] = _xy.y;

        char message[96];
        serializeJson(jsonDoc, message, 96);
        jsonDoc.clear();
        device.publish(commandTopic.c_str(), message, retain);
    }

    // only one listener will work, newest overwrites previous
    void onXY(std::function<void(XY)> _listener)
    {
        xy_listener = _listener;
    }
    // end-if xy_supported

    // start-if effects_supported

    const String effect_list[NUMBER_OF_EFFECTS] = {
        // __insert-effect-list
    };

    Effect getEffect()
    {
        return effect;
    }

    String effectToString(Effect effect)
    {
        return effect_list[effect];
    }

    Effect stringToEffect(String str_effect)
    {
        for (int i = 0; i < NUMBER_OF_EFFECTS; i++)
        {
            if (effect_list[i].equals(str_effect))
            {
                return (Effect)i;
            }
        }

        Serial.println("Invalid stringToEffect value: " + str_effect);

        delay(5000);
        return (Effect)0;
    }

    void setEffect(Effect _effect)
    {
        StaticJsonDocument<96> jsonDoc;
        jsonDoc["state"] = "ON";

        jsonDoc["effect"] = effectToString(_effect);

        char message[96];
        serializeJson(jsonDoc, message, 96);
        jsonDoc.clear();
        device.publish(commandTopic.c_str(), message, retain);
    }

    // only one listener will work, newest overwrites previous
    void onEffect(std::function<void(Effect)> _listener)
    {
        effect_listener = _listener;
    }
    // end-if effects_supported
} VAR_NAME;