#include "manager.h"

#define RETAIN false

// start
class _VAR_NAME
{
private:
    const String name = "NAME";

    const String commandTopic = "COMMAND_TOPIC";

    const bool retain = RETAIN;

    std::function<void(void)> listener = []() {};

public:
    _VAR_NAME()
    {
        if (!retain)
        {
            device.clearRetain(commandTopic.c_str());
        }

        device.subscribe(commandTopic.c_str(), [this](String message)
                         {
                            if(message != "ON") return;
                            listener(); });
    }

    // only one listener will work, newest overwrites previous
    void onActive(std::function<void(void)> _listener)
    {
        listener = _listener;
    }
} VAR_NAME;