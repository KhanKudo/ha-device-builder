#include "manager.h"

#define RETAIN false

// start
class _VAR_NAME
{
private:
    const String name = "NAME";

    const String commandTopic = "COMMAND_TOPIC";
    const String stateTopic = "STATE_TOPIC";

    const bool retain = RETAIN;

    std::function<void(bool)> listener = [](bool) {};

    bool state = false;

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
                            if(message != "ON" && message != "OFF") return;

                            state = message == "ON";
                            device.publish(stateTopic.c_str(), message.c_str(), retain);
                            listener(message == "ON"); });
    }

    bool getState()
    {
        return state;
    }

    void setState(bool newState)
    {
        device.publish(commandTopic.c_str(), newState ? "ON" : "OFF", retain);
    }

    // only one listener will work, newest overwrites previous
    void onChange(std::function<void(bool)> _listener)
    {
        listener = _listener;
    }
} VAR_NAME;