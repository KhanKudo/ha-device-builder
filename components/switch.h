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
            manager.clearRetain(commandTopic.c_str());
            manager.clearRetain(stateTopic.c_str());
        }

        manager.subscribe(commandTopic.c_str(), [this](String message)
                          {
                            if(message != "ON" && message != "OFF") return;

                            listener(message == "ON");
                            setState(message == "ON"); });
    }

    bool getState()
    {
        return state;
    }

    void setState(bool newState)
    {
        state = newState;
        manager.publish(stateTopic.c_str(), newState ? "ON" : "OFF", retain);
    }

    // only one listener will work, newest overwrites previous
    void onChange(std::function<void(bool)> _listener)
    {
        listener = _listener;
    }
} VAR_NAME;