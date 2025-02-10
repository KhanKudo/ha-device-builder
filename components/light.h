#include "manager.h"
#include <Arduino.h>

#define RETAIN false
#define NUMBER_OF_EFFECTS 0
#define MIN_MIREDS 0
#define MAX_MIREDS 0
#define RESOLUTION 0
#define UINT_RESOLUTION_T uint8_t

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

    const uint16_t intervalFreqHz = 100;

    bool initialSetup = true;

    std::function<void(bool)> state_listener = [](bool) {};

    bool state = false;

    // start-if brightness_supported
    UINT_RESOLUTION_T brightnessTarget = 0;
    int32_t brightnessStep = 0;
    uint16_t brightnessDeltaMs = 0;
    uint32_t brightnessNextMillis = 0;
    UINT_RESOLUTION_T brightness = 0;
    std::function<void(UINT_RESOLUTION_T)> brightness_listener = [](UINT_RESOLUTION_T) {};
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
    // start-if color_temp_supported
    const unsigned int min_mireds = MIN_MIREDS;
    const unsigned int max_mireds = MAX_MIREDS;
    // end-if color_temp_supported

    // start-if brightness_supported
    const uint8_t resolution = RESOLUTION;
    // end-if brightness_supported

    _VAR_NAME()
    {
        if (!retain)
        {
            device.clearRetain(commandTopic.c_str());
            device.clearRetain(stateTopic.c_str());
        }

        device.subscribe(commandTopic.c_str(), [this](String message)
                         {
                            deserializeJson(jsonMsg, message);

                            jsonState.clear();

                            // start-if effects_supported
                            if(jsonMsg.containsKey("effect") && !jsonMsg.containsKey("state"))
                                return;
                            // end-if effects_supported

                            if(jsonMsg.containsKey("retain-recovery") && jsonMsg["retain-recovery"] == true){
                                if(initialSetup)
                                    initialSetup = false;
                                else
                                    return;
                            }

                            if(jsonMsg.containsKey("state")){
                                if(jsonMsg["state"] != "ON" && jsonMsg["state"] != "OFF") return;
                                
                                bool newState = jsonMsg["state"] == "ON";
                                if(state != newState){
                                    state = newState;

                                    // start-if brightness_supported
                                    if(!newState || jsonRetainedCommand.containsKey("brightness")){
                                        if(jsonMsg.containsKey("transition")){
                                            if(!newState && brightness != 0)
                                                state = true;
                                            
                                            brightnessTarget = newState ? jsonRetainedCommand["brightness"] : 0;
                                            if(brightness != brightnessTarget){
                                                brightnessStep = max(1, (int)min(abs((float)brightnessTarget - (float)brightness), round(abs((float)brightnessTarget - (float)brightness) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                                brightnessDeltaMs = round((1000.0f * (float)jsonMsg["transition"] * (float)brightnessStep) / abs((float)brightnessTarget - (float)brightness));
                                                brightnessNextMillis = millis() + brightnessDeltaMs;
                                                if(brightnessTarget < brightness)
                                                    brightnessStep = -brightnessStep;
                                            }
                                            jsonState["brightness"] = brightnessTarget;
                                        }
                                        else{
                                            brightness = newState ? jsonRetainedCommand["brightness"] : 0;
                                            jsonState["brightness"] = brightness;
                                        }
                                    }
                                    // end-if brightness_supported
                                }
                                jsonState["state"] = jsonMsg["state"];
                            }

                            // start-if brightness_supported
                            if(jsonMsg.containsKey("brightness")){
                                if(jsonMsg.containsKey("transition")){
                                    brightnessTarget = jsonMsg["brightness"];
                                    if(brightness != brightnessTarget){
                                        brightnessStep = max(1, (int)min(abs((float)brightnessTarget - (float)brightness), round(abs((float)brightnessTarget - (float)brightness) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        brightnessDeltaMs = round((1000.0f * (float)jsonMsg["transition"] * (float)brightnessStep) / abs((float)brightnessTarget - (float)brightness));
                                        brightnessNextMillis = millis() + brightnessDeltaMs;
                                        if(brightnessTarget < brightness)
                                            brightnessStep = -brightnessStep;
                                    }
                                    else if(brightness == 0){
                                        state = false;
                                    }
                                }
                                else{
                                    brightness = jsonMsg["brightness"];
                                    if(brightness == 0)
                                        state = false;
                                    brightnessStep = 0;
                                }
                                
                                if(jsonMsg["brightness"] == 0)
                                    jsonState["state"] = "OFF";
                                else if(!state){
                                    state = true;
                                    jsonState["state"] = "ON";
                                }
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

                            // start-if color_temp_supported
                            if(jsonState.containsKey("color_temp")){
                                color_temp_listener(color_temp);
                            }
                            // end-if color_temp_supported

                            // start-if rgb_supported
                            if(jsonState.containsKey("color")){
                                rgb_listener(rgb);
                            }
                            // end-if rgb_supported

                            // start-if rgbw_supported
                            if(jsonState.containsKey("color")){
                                rgbw_listener(rgbw);
                            }
                            // end-if rgbw_supported

                            // start-if rgbww_supported
                            if(jsonState.containsKey("color")){
                                rgbww_listener(rgbww);
                            }
                            // end-if rgbww_supported

                            // start-if hs_supported
                            if(jsonState.containsKey("color")){
                                hs_listener(hs);
                            }
                            // end-if hs_supported

                            // start-if xy_supported
                            if(jsonState.containsKey("color")){
                                xy_listener(xy);
                            }
                            // end-if xy_supported

                            // start-if effects_supported
                            if(jsonState.containsKey("effect")){
                                effect_listener(effect);
                            }
                            // end-if effects_supported

                            // start-if brightness_supported
                            if(jsonState.containsKey("brightness") && !jsonMsg.containsKey("transition")){
                                brightness_listener(brightness);
                            }
                            // end-if brightness_supported

                            if(jsonState.containsKey("state")){
                                state_listener(state);
                            }

                            char responseMsg[256];
                            serializeJson(jsonState, responseMsg, 256);
                            device.publish(stateTopic.c_str(), responseMsg, retain);

                            if(retain){
                                if(jsonMsg.containsKey("state"))
                                    jsonRetainedCommand["state"] = jsonMsg["state"];
                            // start-if brightness_supported
                                if(jsonMsg.containsKey("brightness"))
                                    jsonRetainedCommand["brightness"] = jsonMsg["brightness"];
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

        // start-if brightness_supported
        device.setLooper([this](void)
                         {
            if(brightnessStep != 0 && millis()>=brightnessNextMillis){
                // mult to compensate for potential loop-lag, causing multiple trigger skips
                const float step = (float)brightnessStep*max(1.0f,floor((float)(millis()-brightnessNextMillis)/(float)brightnessDeltaMs));
                if(abs((float)brightnessTarget - (float)brightness) <= abs(step)){
                    brightness=brightnessTarget;
                    brightnessStep=0;
                }
                else{
                    brightness+=step;
                    brightnessNextMillis = millis() + brightnessDeltaMs;
                }
                brightness_listener(brightness);
                
                if(brightness == 0){
                    state = false;
                    state_listener(state);
                }
            } });
        // end-if brightness_supported
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
    UINT_RESOLUTION_T getBrightness()
    {
        return brightness;
    }

    void setBrightness(UINT_RESOLUTION_T newBrightness)
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
    void onBrightness(std::function<void(UINT_RESOLUTION_T)> _listener)
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
        StaticJsonDocument<64> jsonDoc;
        jsonDoc["state"] = state ? "ON" : "OFF";

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
        bool newState = _hs.h > 0 || _hs.s > 0;

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
        StaticJsonDocument<96> jsonDoc;
        jsonDoc["state"] = state ? "ON" : "OFF";

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