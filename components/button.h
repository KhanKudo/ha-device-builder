#include "manager.h"

// start
class _VAR_NAME
{
private:
    const String name = "NAME";

    const String commandTopic = "COMMAND_TOPIC";

    std::function<void(void)> listener = []() {};

public:
    _VAR_NAME()
    {
        device.clearRetain(commandTopic.c_str());
        device.subscribe(commandTopic.c_str(), [this](String message)
                          {
                            if(message != "PRESS") return;
                            listener(); });
    }

    // only one listener will work, newest overwrites previous
    void onPress(std::function<void(void)> _listener)
    {
        listener = _listener;
    }
} VAR_NAME;