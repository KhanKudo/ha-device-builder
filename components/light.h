#include "manager.h"
#include <Arduino.h>

#define RETAIN false
#define NUMBER_OF_EFFECTS 0
#define MIN_KELVIN 2700
#define MAX_KELVIN 6500
#define RESOLUTION 8
#define UINT_RESOLUTION_T uint8_t

#define OUTPUT_HANDLE
// #define OUTPUT_DATA //TODO later, when FastLED auto-handling gets implemented
#define OUTPUT_MODE_ONOFF

#define OUTPUT_PIN GPIO_NUM_8
#define OUTPUT_PIN_R GPIO_NUM_6
#define OUTPUT_PIN_G GPIO_NUM_7
#define OUTPUT_PIN_B GPIO_NUM_8
#define OUTPUT_PIN_W GPIO_NUM_9
#define OUTPUT_PIN_C GPIO_NUM_10

#define INITIAL_BRIGHTNESS 0
#define INITIAL_TEMP 4000

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
struct DeltaMs_RGB
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
};
struct NextMillis_RGB
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
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
struct DeltaMs_RGBW
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t w;
};
struct NextMillis_RGBW
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t w;
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
struct DeltaMs_RGBWW
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t c;
    uint16_t w;
};
struct NextMillis_RGBWW
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t c;
    uint32_t w;
};
#endif
// end-if rgbww_supported

// start-if hs_supported
#ifndef DEF_HS
#define DEF_HS
struct Color_HS
{
    double h;
    double s;
};
struct DeltaMs_HS
{
    uint16_t h;
    uint16_t s;
};
struct NextMillis_HS
{
    uint32_t h;
    uint32_t s;
};
#endif
// end-if hs_supported

// start-if xy_supported
#ifndef DEF_XY
#define DEF_XY
struct Color_XY
{
    double x;
    double y;
};
struct DeltaMs_XY
{
    uint16_t x;
    uint16_t y;
};
struct NextMillis_XY
{
    uint32_t x;
    uint32_t y;
};
#endif
// end-if xy_supported

struct _VAR_NAME
{
private:
    const String name = "NAME";

    const String commandTopic = "COMMAND_TOPIC";
    const String stateTopic = "STATE_TOPIC";

    const bool retain = RETAIN;

    const uint16_t intervalFreqHz = 100;
    const uint8_t intervalDeltaMs = 1000 / intervalFreqHz;

    bool initialSetup = true;

#ifdef OUTPUT_MODE_ONOFF
    std::function<void(bool)> state_listener = [](bool state)
    {
        digitalWrite(OUTPUT_PIN, state);
    };
#elif !defined(OUTPUT_HANDLE)
    std::function<void(bool)> state_listener = [](bool) {};
#endif
    bool state = false;

    // start-if brightness_supported
    UINT_RESOLUTION_T brightnessTarget = 0;
    UINT_RESOLUTION_T brightnessStep = 0;
    uint16_t brightnessDeltaMs = 0;
    uint32_t brightnessNextMillis = 0;
    UINT_RESOLUTION_T brightness = 0;
#ifdef OUTPUT_HANDLE
    std::function<void(UINT_RESOLUTION_T)> brightness_listener = [this](UINT_RESOLUTION_T brightness)
    {
#ifdef OUTPUT_PIN
        ledcWrite(OUTPUT_PIN, brightness);
#elif defined(OUTPUT_PIN_W) && !defined(OUTPUT_PIN_R)
        ledcWrite(OUTPUT_PIN_W, getWarm());
        ledcWrite(OUTPUT_PIN_C, getCold());
#endif
    };
#else
    std::function<void(UINT_RESOLUTION_T)> brightness_listener = [](UINT_RESOLUTION_T) {};
#endif
    // end-if brightness_supported

    // start-if color_temp_supported
    uint16_t color_temp_target = 0;
    uint16_t color_temp_step = 0;
    uint16_t color_temp_delta_ms = 0;
    uint32_t color_temp_next_millis = 0;
    uint16_t color_temp = 0;
#ifdef OUTPUT_HANDLE
    std::function<void(uint16_t)> color_temp_listener = [this](uint16_t color_temp)
    {
        brightness_listener(0);
    };
#else
    std::function<void(uint16_t)> color_temp_listener = [](uint16_t) {};
#endif
    // end-if color_temp_supported

    // start-if rgb_supported
    Color_RGB rgbTarget = {0, 0, 0};
    Color_RGB rgbStep = {0, 0, 0};
    DeltaMs_RGB rgbDeltaMs = {0, 0, 0};
    NextMillis_RGB rgbNextMillis = {0, 0, 0};
    bool rgbTriggerListener = false;
    uint32_t rgbNextTriggerMillis = 0;
    Color_RGB rgb = {0, 0, 0};
#ifdef OUTPUT_HANDLE
    std::function<void(Color_RGB)> rgb_listener = [](Color_RGB rgb)
    {
        // TODO handle brightness somehow
        ledcWrite(OUTPUT_PIN_R, rgb.r);
        ledcWrite(OUTPUT_PIN_G, rgb.g);
        ledcWrite(OUTPUT_PIN_B, rgb.b);
    };
#else
    std::function<void(Color_RGB)> rgb_listener = [](Color_RGB) {};
#endif
    // end-if rgb_supported

    // start-if rgbw_supported
    Color_RGBW rgbwTarget = {0, 0, 0, 0};
    Color_RGBW rgbwStep = {0, 0, 0, 0};
    DeltaMs_RGBW rgbwDeltaMs = {0, 0, 0, 0};
    NextMillis_RGBW rgbwNextMillis = {0, 0, 0, 0};
    bool rgbwTriggerListener = false;
    uint32_t rgbwNextTriggerMillis = 0;
    Color_RGBW rgbw = {0, 0, 0, 0};
#ifdef OUTPUT_HANDLE
    std::function<void(Color_RGBW)> rgbw_listener = [](Color_RGBW rgbw)
    {
        // TODO handle brightness somehow
        ledcWrite(OUTPUT_PIN_R, rgbw.r);
        ledcWrite(OUTPUT_PIN_G, rgbw.g);
        ledcWrite(OUTPUT_PIN_B, rgbw.b);
        ledcWrite(OUTPUT_PIN_W, rgbw.w);
    };
#else
    std::function<void(Color_RGBW)> rgbw_listener = [](Color_RGBW) {};
#endif
    // end-if rgbw_supported

    // start-if rgbww_supported
    Color_RGBWW rgbwwTarget = {0, 0, 0, 0, 0};
    Color_RGBWW rgbwwStep = {0, 0, 0, 0, 0};
    DeltaMs_RGBWW rgbwwDeltaMs = {0, 0, 0, 0, 0};
    NextMillis_RGBWW rgbwwNextMillis = {0, 0, 0, 0, 0};
    bool rgbwwTriggerListener = false;
    uint32_t rgbwwNextTriggerMillis = 0;
    Color_RGBWW rgbww = {0, 0, 0, 0, 0};
#ifdef OUTPUT_HANDLE
    std::function<void(Color_RGBWW)> rgbww_listener = [](Color_RGBWW rgbww)
    {
        // TODO handle brightness somehow
        ledcWrite(OUTPUT_PIN_R, rgbww.r);
        ledcWrite(OUTPUT_PIN_G, rgbww.g);
        ledcWrite(OUTPUT_PIN_B, rgbww.b);
        ledcWrite(OUTPUT_PIN_W, rgbww.w);
        ledcWrite(OUTPUT_PIN_C, rgbww.c);
    };
#else
    std::function<void(Color_RGBWW)> rgbww_listener = [](Color_RGBWW) {};
#endif
    // end-if rgbww_supported

    // start-if hs_supported
    Color_HS hsTarget = {0, 0};
    Color_HS hsStep = {0, 0};
    DeltaMs_HS hsDeltaMs = {0, 0};
    NextMillis_HS hsNextMillis = {0, 0};
    bool hsTriggerListener = false;
    uint32_t hsNextTriggerMillis = 0;
    Color_HS hs = {0, 0};
    std::function<void(Color_HS)> hs_listener = [](Color_HS) {};
    // end-if hs_supported

    // start-if xy_supported
    Color_XY xyTarget = {0, 0};
    Color_XY xyStep = {0, 0};
    DeltaMs_XY xyDeltaMs = {0, 0};
    NextMillis_XY xyNextMillis = {0, 0};
    bool xyTriggerListener = false;
    uint32_t xyNextTriggerMillis = 0;
    Color_XY xy = {0, 0};
    std::function<void(Color_XY)> xy_listener = [](Color_XY) {};
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
    const unsigned int min_kelvin = MIN_KELVIN;
    const unsigned int max_kelvin = MAX_KELVIN;
    // end-if color_temp_supported

    // start-if brightness_supported
    const uint8_t resolution = RESOLUTION;
    // end-if brightness_supported

    void _init()
    {
#ifdef OUTPUT_MODE_ONOFF
        pinMode(OUTPUT_PIN, OUTPUT);
        digitalWrite(OUTPUT_PIN, INITIAL_BRIGHTNESS);
#else
#ifdef OUTPUT_PIN
        ledcAttach(OUTPUT_PIN, 16384, RESOLUTION);
        ledcWrite(OUTPUT_PIN, INITIAL_BRIGHTNESS);
#endif
#ifdef OUTPUT_PIN_R
        ledcAttach(OUTPUT_PIN_R, 16384, RESOLUTION);
        ledcWrite(OUTPUT_PIN_R, 0);
#endif
#ifdef OUTPUT_PIN_G
        ledcAttach(OUTPUT_PIN_G, 16384, RESOLUTION);
        ledcWrite(OUTPUT_PIN_G, 0);
#endif
#ifdef OUTPUT_PIN_B
        ledcAttach(OUTPUT_PIN_B, 16384, RESOLUTION);
        ledcWrite(OUTPUT_PIN_B, 0);
#endif
#ifdef OUTPUT_PIN_W
        ledcAttach(OUTPUT_PIN_W, 16384, RESOLUTION);
        ledcWrite(OUTPUT_PIN_W, INITIAL_BRIGHTNESS_W);
#endif
#ifdef OUTPUT_PIN_C
        ledcAttach(OUTPUT_PIN_C, 16384, RESOLUTION);
        ledcWrite(OUTPUT_PIN_C, INITIAL_BRIGHTNESS_C);
#endif
#endif

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
                                if(jsonMsg.containsKey("transition")){
                                    color_temp_target = jsonMsg["color_temp"];

                                    // if way too low for kelvin, treat as mireds and convert to kelvin
                                    if(color_temp_target < 1000)
                                        color_temp_target = 1000000 / color_temp_target;

                                    if(color_temp != color_temp_target){
                                        color_temp_step = max(1, (int)min(abs((float)color_temp_target - (float)color_temp), round(abs((float)color_temp_target - (float)color_temp) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        color_temp_delta_ms = round((1000.0f * (float)jsonMsg["transition"] * (float)color_temp_step) / abs((float)color_temp_target - (float)color_temp));
                                        color_temp_next_millis = millis() + color_temp_delta_ms;
                                    }
                                }
                                else{
                                    color_temp = jsonMsg["color_temp"];
                                    // if way too low for kelvin, treat as mireds and convert to kelvin
                                    if(color_temp < 1000)
                                        color_temp = 1000000 / color_temp;
                                        
                                    color_temp_step = 0;
                                }
                                jsonState["color_temp"] = jsonMsg["color_temp"];
                                jsonState["color_mode"] = "color_temp";
                            }
                            // end-if color_temp_supported

                            // start-if rgb_supported
                            if(jsonMsg.containsKey("color")){
                                if(jsonMsg.containsKey("transition")){
                                    rgbTarget.r = jsonMsg["color"]["r"];
                                    if(rgb.r != rgbTarget.r){
                                        rgbStep.r = max(1, (int)min(abs((float)rgbTarget.r - (float)rgb.r), round(abs((float)rgbTarget.r - (float)rgb.r) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbDeltaMs.r = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbStep.r) / abs((float)rgbTarget.r - (float)rgb.r));
                                        rgbNextMillis.r = millis() + rgbDeltaMs.r;
                                    }
                                    rgbTarget.g = jsonMsg["color"]["g"];
                                    if(rgb.g != rgbTarget.g){
                                        rgbStep.g = max(1, (int)min(abs((float)rgbTarget.g - (float)rgb.g), round(abs((float)rgbTarget.g - (float)rgb.g) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbDeltaMs.g = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbStep.g) / abs((float)rgbTarget.g - (float)rgb.g));
                                        rgbNextMillis.g = millis() + rgbDeltaMs.g;
                                    }
                                    rgbTarget.b = jsonMsg["color"]["b"];
                                    if(rgb.b != rgbTarget.b){
                                        rgbStep.b = max(1, (int)min(abs((float)rgbTarget.b - (float)rgb.b), round(abs((float)rgbTarget.b - (float)rgb.b) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbDeltaMs.b = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbStep.b) / abs((float)rgbTarget.b - (float)rgb.b));
                                        rgbNextMillis.b = millis() + rgbDeltaMs.b;
                                    }
                                }
                                else{
                                    rgb = {jsonMsg["color"]["r"], jsonMsg["color"]["g"], jsonMsg["color"]["b"]};
                                    rgbStep = {0, 0, 0};
                                }
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "rgb";
                            }
                            // end-if rgb_supported

                            // start-if rgbw_supported
                            if(jsonMsg.containsKey("color")){
                                if(jsonMsg.containsKey("transition")){
                                    rgbwTarget.r = jsonMsg["color"]["r"];
                                    if(rgbw.r != rgbwTarget.r){
                                        rgbwStep.r = max(1, (int)min(abs((float)rgbwTarget.r - (float)rgbw.r), round(abs((float)rgbwTarget.r - (float)rgbw.r) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbwDeltaMs.r = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbwStep.r) / abs((float)rgbwTarget.r - (float)rgbw.r));
                                        rgbwNextMillis.r = millis() + rgbwDeltaMs.r;
                                    }
                                    rgbwTarget.g = jsonMsg["color"]["g"];
                                    if(rgbw.g != rgbwTarget.g){
                                        rgbwStep.g = max(1, (int)min(abs((float)rgbwTarget.g - (float)rgbw.g), round(abs((float)rgbwTarget.g - (float)rgbw.g) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbwDeltaMs.g = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbwStep.g) / abs((float)rgbwTarget.g - (float)rgbw.g));
                                        rgbwNextMillis.g = millis() + rgbwDeltaMs.g;
                                    }
                                    rgbwTarget.b = jsonMsg["color"]["b"];
                                    if(rgbw.b != rgbwTarget.b){
                                        rgbwStep.b = max(1, (int)min(abs((float)rgbwTarget.b - (float)rgbw.b), round(abs((float)rgbwTarget.b - (float)rgbw.b) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbwDeltaMs.b = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbwStep.b) / abs((float)rgbwTarget.b - (float)rgbw.b));
                                        rgbwNextMillis.b = millis() + rgbwDeltaMs.b;
                                    }
                                    rgbwTarget.w = jsonMsg["color"]["w"];
                                    if(rgbw.w != rgbwTarget.w){
                                        rgbwStep.w = max(1, (int)min(abs((float)rgbwTarget.w - (float)rgbw.w), round(abs((float)rgbwTarget.w - (float)rgbw.w) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbwDeltaMs.w = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbwStep.w) / abs((float)rgbwTarget.w - (float)rgbw.w));
                                        rgbwNextMillis.w = millis() + rgbwDeltaMs.w;
                                    }
                                }
                                else{
                                    rgbw = {jsonMsg["color"]["r"], jsonMsg["color"]["g"], jsonMsg["color"]["b"], jsonMsg["color"]["w"]};
                                    rgbwStep = {0, 0, 0, 0};
                                }
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "rgbw";
                            }
                            // end-if rgbw_supported

                            // start-if rgbww_supported
                            if(jsonMsg.containsKey("color")){
                                if(jsonMsg.containsKey("transition")){
                                    rgbwwTarget.r = jsonMsg["color"]["r"];
                                    if(rgbww.r != rgbwwTarget.r){
                                        rgbwwStep.r = max(1, (int)min(abs((float)rgbwwTarget.r - (float)rgbww.r), round(abs((float)rgbwwTarget.r - (float)rgbww.r) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbwwDeltaMs.r = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbwwStep.r) / abs((float)rgbwwTarget.r - (float)rgbww.r));
                                        rgbwwNextMillis.r = millis() + rgbwwDeltaMs.r;
                                    }
                                    rgbwwTarget.g = jsonMsg["color"]["g"];
                                    if(rgbww.g != rgbwwTarget.g){
                                        rgbwwStep.g = max(1, (int)min(abs((float)rgbwwTarget.g - (float)rgbww.g), round(abs((float)rgbwwTarget.g - (float)rgbww.g) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbwwDeltaMs.g = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbwwStep.g) / abs((float)rgbwwTarget.g - (float)rgbww.g));
                                        rgbwwNextMillis.g = millis() + rgbwwDeltaMs.g;
                                    }
                                    rgbwwTarget.b = jsonMsg["color"]["b"];
                                    if(rgbww.b != rgbwwTarget.b){
                                        rgbwwStep.b = max(1, (int)min(abs((float)rgbwwTarget.b - (float)rgbww.b), round(abs((float)rgbwwTarget.b - (float)rgbww.b) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbwwDeltaMs.b = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbwwStep.b) / abs((float)rgbwwTarget.b - (float)rgbww.b));
                                        rgbwwNextMillis.b = millis() + rgbwwDeltaMs.b;
                                    }
                                    rgbwwTarget.c = jsonMsg["color"]["c"];
                                    if(rgbww.c != rgbwwTarget.c){
                                        rgbwwStep.c = max(1, (int)min(abs((float)rgbwwTarget.c - (float)rgbww.c), round(abs((float)rgbwwTarget.c - (float)rgbww.c) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbwwDeltaMs.c = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbwwStep.c) / abs((float)rgbwwTarget.c - (float)rgbww.c));
                                        rgbwwNextMillis.c = millis() + rgbwwDeltaMs.c;
                                    }
                                    rgbwwTarget.w = jsonMsg["color"]["w"];
                                    if(rgbww.w != rgbwwTarget.w){
                                        rgbwwStep.w = max(1, (int)min(abs((float)rgbwwTarget.w - (float)rgbww.w), round(abs((float)rgbwwTarget.w - (float)rgbww.w) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        rgbwwDeltaMs.w = round((1000.0f * (float)jsonMsg["transition"] * (float)rgbwwStep.w) / abs((float)rgbwwTarget.w - (float)rgbww.w));
                                        rgbwwNextMillis.w = millis() + rgbwwDeltaMs.w;
                                    }
                                }
                                else{
                                    rgbww = {jsonMsg["color"]["r"], jsonMsg["color"]["g"], jsonMsg["color"]["b"], jsonMsg["color"]["c"], jsonMsg["color"]["w"]};
                                    rgbwwStep = {0, 0, 0, 0, 0};
                                }
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "rgbww";
                            }
                            // end-if rgbww_supported

                            // start-if hs_supported
                            if(jsonMsg.containsKey("color")){
                                if(jsonMsg.containsKey("transition")){
                                    hsTarget.h = jsonMsg["color"]["h"];
                                    if(hs.h != hsTarget.h){
                                        hsStep.h = max(1, (int)min(abs((float)hsTarget.h - (float)hs.h), round(abs((float)hsTarget.h - (float)hs.h) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        hsDeltaMs.h = round((1000.0f * (float)jsonMsg["transition"] * (float)hsStep.h) / abs((float)hsTarget.h - (float)hs.h));
                                        hsNextMillis.h = millis() + hsDeltaMs.h;
                                    }
                                    hsTarget.s = jsonMsg["color"]["s"];
                                    if(hs.s != hsTarget.s){
                                        hsStep.s = max(1, (int)min(abs((float)hsTarget.s - (float)hs.s), round(abs((float)hsTarget.s - (float)hs.s) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        hsDeltaMs.s = round((1000.0f * (float)jsonMsg["transition"] * (float)hsStep.s) / abs((float)hsTarget.s - (float)hs.s));
                                        hsNextMillis.s = millis() + hsDeltaMs.s;
                                    }
                                }
                                else{
                                    hs = {jsonMsg["color"]["h"], jsonMsg["color"]["s"]};
                                    hsStep = {0, 0};
                                }
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "hs";
                            }
                            // end-if hs_supported

                            // start-if xy_supported
                            if(jsonMsg.containsKey("color")){
                                if(jsonMsg.containsKey("transition")){
                                    xyTarget.x = jsonMsg["color"]["x"];
                                    if(xy.x != xyTarget.x){
                                        xyStep.x = max(1, (int)min(abs((float)xyTarget.x - (float)xy.x), round(abs((float)xyTarget.x - (float)xy.x) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        xyDeltaMs.x = round((1000.0f * (float)jsonMsg["transition"] * (float)xyStep.x) / abs((float)xyTarget.x - (float)xy.x));
                                        xyNextMillis.x = millis() + xyDeltaMs.x;
                                    }
                                    xyTarget.y = jsonMsg["color"]["y"];
                                    if(xy.y != xyTarget.y){
                                        xyStep.y = max(1, (int)min(abs((float)xyTarget.y - (float)xy.y), round(abs((float)xyTarget.y - (float)xy.y) / ((float)intervalFreqHz * (float)jsonMsg["transition"]))));
                                        xyDeltaMs.y = round((1000.0f * (float)jsonMsg["transition"] * (float)xyStep.y) / abs((float)xyTarget.y - (float)xy.y));
                                        xyNextMillis.y = millis() + xyDeltaMs.y;
                                    }
                                }
                                else{
                                    xy = {jsonMsg["color"]["x"], jsonMsg["color"]["y"]};
                                    xyStep = {0, 0};
                                }
                                jsonState["color"] = jsonMsg["color"];
                                jsonState["color_mode"] = "xy";
                            }
                            // end-if xy_supported

                            // start-if effects_supported
                            if(jsonMsg.containsKey("effect")){
                                effect = stringToEffect(jsonMsg["effect"]);
                                jsonState["effect"] = jsonMsg["effect"];
                            }
                            else if(jsonMsg.containsKey("color") && effect != 0){
                                effect = (Effect)0; // assumed to be 'none'
                                jsonState["effect"] = effectToString(effect);
                            }
                            // end-if effects_supported
                            
                            // --------------------------------------------------

                            // start-if color_temp_supported
                            if(jsonState.containsKey("color_temp") && !jsonMsg.containsKey("transition")){
                                color_temp_listener(color_temp);
                            }
                            // end-if color_temp_supported

                            // start-if rgb_supported
                            if(jsonState.containsKey("color") && !jsonMsg.containsKey("transition")){
                                rgb_listener(rgb);
                            }
                            // end-if rgb_supported

                            // start-if rgbw_supported
                            if(jsonState.containsKey("color") && !jsonMsg.containsKey("transition")){
                                rgbw_listener(rgbw);
                            }
                            // end-if rgbw_supported

                            // start-if rgbww_supported
                            if(jsonState.containsKey("color") && !jsonMsg.containsKey("transition")){
                                rgbww_listener(rgbww);
                            }
                            // end-if rgbww_supported

                            // start-if hs_supported
                            if(jsonState.containsKey("color") && !jsonMsg.containsKey("transition")){
                                hs_listener(hs);
                            }
                            // end-if hs_supported

                            // start-if xy_supported
                            if(jsonState.containsKey("color") && !jsonMsg.containsKey("transition")){
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

#if defined(OUTPUT_MODE_ONOFF) || !defined(OUTPUT_HANDLE)
                            if(jsonState.containsKey("state")){
                                state_listener(state);
                            }
#endif

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
                                if(jsonMsg.containsKey("color_temp"))
                                    jsonRetainedCommand["color_temp"] = jsonMsg["color_temp"];
                            // end-if color_temp_supported
                            // start-if rgb_supported
                                if(jsonMsg.containsKey("color"))
                                    jsonRetainedCommand["color"]["r"] = jsonMsg["color"]["r"];
                                    jsonRetainedCommand["color"]["g"] = jsonMsg["color"]["g"];
                                    jsonRetainedCommand["color"]["b"] = jsonMsg["color"]["b"];
                            // end-if rgb_supported
                            // start-if rgbw_supported
                                if(jsonMsg.containsKey("color"))
                                    jsonRetainedCommand["color"]["r"] = jsonMsg["color"]["r"];
                                    jsonRetainedCommand["color"]["g"] = jsonMsg["color"]["g"];
                                    jsonRetainedCommand["color"]["b"] = jsonMsg["color"]["b"];
                                    jsonRetainedCommand["color"]["w"] = jsonMsg["color"]["w"];
                            // end-if rgbw_supported
                            // start-if rgbww_supported
                                if(jsonMsg.containsKey("color"))
                                    jsonRetainedCommand["color"]["r"] = jsonMsg["color"]["r"];
                                    jsonRetainedCommand["color"]["g"] = jsonMsg["color"]["g"];
                                    jsonRetainedCommand["color"]["b"] = jsonMsg["color"]["b"];
                                    jsonRetainedCommand["color"]["c"] = jsonMsg["color"]["c"];
                                    jsonRetainedCommand["color"]["w"] = jsonMsg["color"]["w"];
                            // end-if rgbww_supported
                            // start-if hs_supported
                                if(jsonMsg.containsKey("color"))
                                    jsonRetainedCommand["color"]["h"] = jsonMsg["color"]["h"];
                                    jsonRetainedCommand["color"]["s"] = jsonMsg["color"]["s"];
                            // end-if hs_supported
                            // start-if xy_supported
                                if(jsonMsg.containsKey("color"))
                                    jsonRetainedCommand["color"]["x"] = jsonMsg["color"]["x"];
                                    jsonRetainedCommand["color"]["y"] = jsonMsg["color"]["y"];
                            // end-if xy_supported
                            // start-if effects_supported
                                if(jsonMsg.containsKey("effect"))
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

        bool needLooper = false;
        // start-if brightness_supported
        needLooper = true;
        // end-if brightness_supported
        // start-if color_temp_supported
        needLooper = true;
        // end-if color_temp_supported
        // start-if rgb_supported
        needLooper = true;
        // end-if rgb_supported
        // start-if rgbw_supported
        needLooper = true;
        // end-if rgbw_supported
        // start-if rgbww_supported
        needLooper = true;
        // end-if rgbww_supported
        // start-if hs_supported
        needLooper = true;
        // end-if hs_supported
        // start-if xy_supported
        needLooper = true;
        // end-if xy_supported

        if (needLooper)
            device.setLooper([this](void)
                             {
                                 // start-if brightness_supported
                                 if (brightnessStep != 0 && millis() >= brightnessNextMillis)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)brightnessStep * max(1.0f, floor((float)(millis() - brightnessNextMillis) / (float)brightnessDeltaMs));
                                     if (abs((float)brightnessTarget - (float)brightness) <= step)
                                     {
                                         brightness = brightnessTarget;
                                         brightnessStep = 0;
                                     }
                                     else
                                     {
                                         if (brightnessTarget < brightness)
                                             brightness -= step;
                                         else
                                             brightness += step;
                                         brightnessNextMillis = millis() + brightnessDeltaMs;
                                     }
                                     brightness_listener(brightness);

                                     if (brightness == 0)
                                     {
                                         state = false;
#if defined(OUTPUT_MODE_ONOFF) || !defined(OUTPUT_HANDLE)
                                         state_listener(state);
#endif
                                     }
                                 }
                                 // end-if brightness_supported

                                 // start-if color_temp_supported
                                 if (color_temp_step != 0 && millis() >= color_temp_next_millis)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)color_temp_step * max(1.0f, floor((float)(millis() - color_temp_next_millis) / (float)color_temp_delta_ms));
                                     if (abs((float)color_temp_target - (float)color_temp) <= step)
                                     {
                                         color_temp = color_temp_target;
                                         color_temp_step = 0;
                                     }
                                     else
                                     {
                                         if (color_temp_target < color_temp)
                                             color_temp -= step;
                                         else
                                             color_temp += step;
                                         color_temp_next_millis = millis() + color_temp_delta_ms;
                                     }
                                     color_temp_listener(color_temp);
                                 }
                                 // end-if color_temp_supported

                                 // start-if rgb_supported
                                 if (rgbStep.r != 0 && millis() >= rgbNextMillis.r)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbStep.r * max(1.0f, floor((float)(millis() - rgbNextMillis.r) / (float)rgbDeltaMs.r));
                                     if (abs((float)rgbTarget.r - (float)rgb.r) <= step)
                                     {
                                         rgb.r = rgbTarget.r;
                                         rgbStep.r = 0;
                                     }
                                     else
                                     {
                                         if (rgbTarget.r < rgb.r)
                                             rgb.r -= step;
                                         else
                                             rgb.r += step;
                                         rgbNextMillis.r = millis() + rgbDeltaMs.r;
                                     }
                                     rgbTriggerListener = true;
                                 }
                                 if (rgbStep.g != 0 && millis() >= rgbNextMillis.g)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbStep.g * max(1.0f, floor((float)(millis() - rgbNextMillis.g) / (float)rgbDeltaMs.g));
                                     if (abs((float)rgbTarget.g - (float)rgb.g) <= step)
                                     {
                                         rgb.g = rgbTarget.g;
                                         rgbStep.g = 0;
                                     }
                                     else
                                     {
                                         if (rgbTarget.g < rgb.g)
                                             rgb.g -= step;
                                         else
                                             rgb.g += step;
                                         rgbNextMillis.g = millis() + rgbDeltaMs.g;
                                     }
                                     rgbTriggerListener = true;
                                 }
                                 if (rgbStep.b != 0 && millis() >= rgbNextMillis.b)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbStep.b * max(1.0f, floor((float)(millis() - rgbNextMillis.b) / (float)rgbDeltaMs.b));
                                     if (abs((float)rgbTarget.b - (float)rgb.b) <= step)
                                     {
                                         rgb.b = rgbTarget.b;
                                         rgbStep.b = 0;
                                     }
                                     else
                                     {
                                         if (rgbTarget.b < rgb.b)
                                             rgb.b -= step;
                                         else
                                             rgb.b += step;
                                         rgbNextMillis.b = millis() + rgbDeltaMs.b;
                                     }
                                     rgbTriggerListener = true;
                                 }

                                 if (rgbTriggerListener && millis() >= rgbNextTriggerMillis)
                                 {
                                     rgbTriggerListener = false;
                                     rgbNextTriggerMillis += intervalDeltaMs;
                                     rgb_listener(rgb);
                                 }
                                 // end-if rgb_supported

                                 // start-if rgbw_supported
                                 if (rgbwStep.r != 0 && millis() >= rgbwNextMillis.r)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbwStep.r * max(1.0f, floor((float)(millis() - rgbwNextMillis.r) / (float)rgbwDeltaMs.r));
                                     if (abs((float)rgbwTarget.r - (float)rgbw.r) <= step)
                                     {
                                         rgbw.r = rgbwTarget.r;
                                         rgbwStep.r = 0;
                                     }
                                     else
                                     {
                                         if (rgbwTarget.r < rgbw.r)
                                             rgbw.r -= step;
                                         else
                                             rgbw.r += step;
                                         rgbwNextMillis.r = millis() + rgbwDeltaMs.r;
                                     }
                                     rgbwTriggerListener = true;
                                 }
                                 if (rgbwStep.g != 0 && millis() >= rgbwNextMillis.g)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbwStep.g * max(1.0f, floor((float)(millis() - rgbwNextMillis.g) / (float)rgbwDeltaMs.g));
                                     if (abs((float)rgbwTarget.g - (float)rgbw.g) <= step)
                                     {
                                         rgbw.g = rgbwTarget.g;
                                         rgbwStep.g = 0;
                                     }
                                     else
                                     {
                                         if (rgbwTarget.g < rgbw.g)
                                             rgbw.g -= step;
                                         else
                                             rgbw.g += step;
                                         rgbwNextMillis.g = millis() + rgbwDeltaMs.g;
                                     }
                                     rgbwTriggerListener = true;
                                 }
                                 if (rgbwStep.b != 0 && millis() >= rgbwNextMillis.b)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbwStep.b * max(1.0f, floor((float)(millis() - rgbwNextMillis.b) / (float)rgbwDeltaMs.b));
                                     if (abs((float)rgbwTarget.b - (float)rgbw.b) <= step)
                                     {
                                         rgbw.b = rgbwTarget.b;
                                         rgbwStep.b = 0;
                                     }
                                     else
                                     {
                                         if (rgbwTarget.b < rgbw.b)
                                             rgbw.b -= step;
                                         else
                                             rgbw.b += step;
                                         rgbwNextMillis.b = millis() + rgbwDeltaMs.b;
                                     }
                                     rgbwTriggerListener = true;
                                 }
                                 if (rgbwStep.w != 0 && millis() >= rgbwNextMillis.w)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbwStep.w * max(1.0f, floor((float)(millis() - rgbwNextMillis.w) / (float)rgbwDeltaMs.w));
                                     if (abs((float)rgbwTarget.w - (float)rgbw.w) <= step)
                                     {
                                         rgbw.w = rgbwTarget.w;
                                         rgbwStep.w = 0;
                                     }
                                     else
                                     {
                                         if (rgbwTarget.w < rgbw.w)
                                             rgbw.w -= step;
                                         else
                                             rgbw.w += step;
                                         rgbwNextMillis.w = millis() + rgbwDeltaMs.w;
                                     }
                                     rgbwTriggerListener = true;
                                 }

                                 if (rgbwTriggerListener && millis() >= rgbwNextTriggerMillis)
                                 {
                                     rgbwTriggerListener = false;
                                     rgbwNextTriggerMillis += intervalDeltaMs;
                                     rgbw_listener(rgbw);
                                 }
                                 // end-if rgbw_supported

                                 // start-if rgbww_supported
                                 if (rgbwwStep.r != 0 && millis() >= rgbwwNextMillis.r)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbwwStep.r * max(1.0f, floor((float)(millis() - rgbwwNextMillis.r) / (float)rgbwwDeltaMs.r));
                                     if (abs((float)rgbwwTarget.r - (float)rgbww.r) <= step)
                                     {
                                         rgbww.r = rgbwwTarget.r;
                                         rgbwwStep.r = 0;
                                     }
                                     else
                                     {
                                         if (rgbwwTarget.r < rgbww.r)
                                             rgbww.r -= step;
                                         else
                                             rgbww.r += step;
                                         rgbwwNextMillis.r = millis() + rgbwwDeltaMs.r;
                                     }
                                     rgbwwTriggerListener = true;
                                 }
                                 if (rgbwwStep.g != 0 && millis() >= rgbwwNextMillis.g)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbwwStep.g * max(1.0f, floor((float)(millis() - rgbwwNextMillis.g) / (float)rgbwwDeltaMs.g));
                                     if (abs((float)rgbwwTarget.g - (float)rgbww.g) <= step)
                                     {
                                         rgbww.g = rgbwwTarget.g;
                                         rgbwwStep.g = 0;
                                     }
                                     else
                                     {
                                         if (rgbwwTarget.g < rgbww.g)
                                             rgbww.g -= step;
                                         else
                                             rgbww.g += step;
                                         rgbwwNextMillis.g = millis() + rgbwwDeltaMs.g;
                                     }
                                     rgbwwTriggerListener = true;
                                 }
                                 if (rgbwwStep.b != 0 && millis() >= rgbwwNextMillis.b)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbwwStep.b * max(1.0f, floor((float)(millis() - rgbwwNextMillis.b) / (float)rgbwwDeltaMs.b));
                                     if (abs((float)rgbwwTarget.b - (float)rgbww.b) <= step)
                                     {
                                         rgbww.b = rgbwwTarget.b;
                                         rgbwwStep.b = 0;
                                     }
                                     else
                                     {
                                         if (rgbwwTarget.b < rgbww.b)
                                             rgbww.b -= step;
                                         else
                                             rgbww.b += step;
                                         rgbwwNextMillis.b = millis() + rgbwwDeltaMs.b;
                                     }
                                     rgbwwTriggerListener = true;
                                 }
                                 if (rgbwwStep.c != 0 && millis() >= rgbwwNextMillis.c)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbwwStep.c * max(1.0f, floor((float)(millis() - rgbwwNextMillis.c) / (float)rgbwwDeltaMs.c));
                                     if (abs((float)rgbwwTarget.c - (float)rgbww.c) <= step)
                                     {
                                         rgbww.c = rgbwwTarget.c;
                                         rgbwwStep.c = 0;
                                     }
                                     else
                                     {
                                         if (rgbwwTarget.c < rgbww.c)
                                             rgbww.c -= step;
                                         else
                                             rgbww.c += step;
                                         rgbwwNextMillis.c = millis() + rgbwwDeltaMs.c;
                                     }
                                     rgbwwTriggerListener = true;
                                 }
                                 if (rgbwwStep.w != 0 && millis() >= rgbwwNextMillis.w)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)rgbwwStep.w * max(1.0f, floor((float)(millis() - rgbwwNextMillis.w) / (float)rgbwwDeltaMs.w));
                                     if (abs((float)rgbwwTarget.w - (float)rgbww.w) <= step)
                                     {
                                         rgbww.w = rgbwwTarget.w;
                                         rgbwwStep.w = 0;
                                     }
                                     else
                                     {
                                         if (rgbwwTarget.w < rgbww.w)
                                             rgbww.w -= step;
                                         else
                                             rgbww.w += step;
                                         rgbwwNextMillis.w = millis() + rgbwwDeltaMs.w;
                                     }
                                     rgbwwTriggerListener = true;
                                 }

                                 if (rgbwwTriggerListener && millis() >= rgbwwNextTriggerMillis)
                                 {
                                     rgbwwTriggerListener = false;
                                     rgbwwNextTriggerMillis += intervalDeltaMs;
                                     rgbww_listener(rgbww);
                                 }
                                 // end-if rgbww_supported

                                 // start-if hs_supported
                                 if (hsStep.h != 0 && millis() >= hsNextMillis.h)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)hsStep.h * max(1.0f, floor((float)(millis() - hsNextMillis.h) / (float)hsDeltaMs.h));
                                     if (abs((float)hsTarget.h - (float)hs.h) <= step)
                                     {
                                         hs.h = hsTarget.h;
                                         hsStep.h = 0;
                                     }
                                     else
                                     {
                                         if (hsTarget.h < hs.h)
                                             hs.h -= step;
                                         else
                                             hs.h += step;
                                         hsNextMillis.h = millis() + hsDeltaMs.h;
                                     }
                                     hsTriggerListener = true;
                                 }
                                 if (hsStep.s != 0 && millis() >= hsNextMillis.s)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)hsStep.s * max(1.0f, floor((float)(millis() - hsNextMillis.s) / (float)hsDeltaMs.s));
                                     if (abs((float)hsTarget.s - (float)hs.s) <= step)
                                     {
                                         hs.s = hsTarget.s;
                                         hsStep.s = 0;
                                     }
                                     else
                                     {
                                         if (hsTarget.s < hs.s)
                                             hs.s -= step;
                                         else
                                             hs.s += step;
                                         hsNextMillis.s = millis() + hsDeltaMs.s;
                                     }
                                     hsTriggerListener = true;
                                 }

                                 if (hsTriggerListener && millis() >= hsNextTriggerMillis)
                                 {
                                     hsTriggerListener = false;
                                     hsNextTriggerMillis += intervalDeltaMs;
                                     hs_listener(hs);
                                 }
                                 // end-if hs_supported

                                 // start-if xy_supported
                                 if (xyStep.x != 0 && millis() >= xyNextMillis.x)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)xyStep.x * max(1.0f, floor((float)(millis() - xyNextMillis.x) / (float)xyDeltaMs.x));
                                     if (abs((float)xyTarget.x - (float)xy.x) <= step)
                                     {
                                         xy.x = xyTarget.x;
                                         xyStep.x = 0;
                                     }
                                     else
                                     {
                                         if (xyTarget.x < xy.x)
                                             xy.x -= step;
                                         else
                                             xy.x += step;
                                         xyNextMillis.x = millis() + xyDeltaMs.x;
                                     }
                                     xyTriggerListener = true;
                                 }
                                 if (xyStep.y != 0 && millis() >= xyNextMillis.y)
                                 {
                                     // mult to compensate for potential loop-lag, causing multiple trigger skips
                                     const float step = (float)xyStep.y * max(1.0f, floor((float)(millis() - xyNextMillis.y) / (float)xyDeltaMs.y));
                                     if (abs((float)xyTarget.y - (float)xy.y) <= step)
                                     {
                                         xy.y = xyTarget.y;
                                         xyStep.y = 0;
                                     }
                                     else
                                     {
                                         if (xyTarget.y < xy.y)
                                             xy.y -= step;
                                         else
                                             xy.y += step;
                                         xyNextMillis.y = millis() + xyDeltaMs.y;
                                     }
                                     xyTriggerListener = true;
                                 }

                                 if (xyTriggerListener && millis() >= xyNextTriggerMillis)
                                 {
                                     xyTriggerListener = false;
                                     xyNextTriggerMillis += intervalDeltaMs;
                                     xy_listener(xy);
                                 }
                                 // end-if xy_supported
                             });
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

#ifndef OUTPUT_HANDLE
    // only one listener will work, newest overwrites previous
    void onState(std::function<void(bool)> _listener)
    {
        state_listener = _listener;
    }
#endif

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

#ifndef OUTPUT_HANDLE
    // only one listener will work, newest overwrites previous
    void onBrightness(std::function<void(UINT_RESOLUTION_T)> _listener)
    {
        brightness_listener = _listener;
    }
#endif
    // end-if brightness_supported

    // start-if color_temp_supported
    uint16_t getColorTemp()
    {
        return color_temp;
    }

    // does account for brightness
    UINT_RESOLUTION_T getCold()
    {
        float ratio = (float)(color_temp - MIN_KELVIN) / (float)(MAX_KELVIN - MIN_KELVIN);
        return ratio * (float)brightness;
    }

    // does account for brightness
    UINT_RESOLUTION_T getWarm()
    {
        float ratio = (float)(color_temp - MIN_KELVIN) / (float)(MAX_KELVIN - MIN_KELVIN);
        return (1.0f - ratio) * (float)brightness;
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

#ifndef OUTPUT_HANDLE
    // only one listener will work, newest overwrites previous
    void onColorTemp(std::function<void(uint16_t)> _listener)
    {
        color_temp_listener = _listener;
    }
#endif
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

#ifndef OUTPUT_HANDLE
    // only one listener will work, newest overwrites previous
    void onRGB(std::function<void(Color_RGB)> _listener)
    {
        rgb_listener = _listener;
    }
#endif
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

#ifndef OUTPUT_HANDLE
    // only one listener will work, newest overwrites previous
    void onRGBW(std::function<void(Color_RGBW)> _listener)
    {
        rgbw_listener = _listener;
    }
#endif
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

#ifndef OUTPUT_HANDLE
    // only one listener will work, newest overwrites previous
    void onRGBWW(std::function<void(Color_RGBWW)> _listener)
    {
        rgbww_listener = _listener;
    }
#endif
    // end-if rgbww_supported

    // start-if hs_supported
    Color_HS getHS()
    {
        return hs;
    }

    void setHS(Color_HS _hs)
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

#ifndef OUTPUT_HANDLE
    // only one listener will work, newest overwrites previous
    void onHS(std::function<void(Color_HS)> _listener)
    {
        hs_listener = _listener;
    }
#endif
    // end-if hs_supported

    // start-if xy_supported
    Color_XY getXY()
    {
        return xy;
    }

    void setXY(Color_XY _xy)
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

#ifndef OUTPUT_HANDLE
    // only one listener will work, newest overwrites previous
    void onXY(std::function<void(Color_XY)> _listener)
    {
        xy_listener = _listener;
    }
#endif
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

#ifndef OUTPUT_HANDLE
    // only one listener will work, newest overwrites previous
    void onEffect(std::function<void(Effect)> _listener)
    {
        effect_listener = _listener;
    }
#endif
    // end-if effects_supported
} VAR_NAME;