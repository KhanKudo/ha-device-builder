#include "manager.h"

// start
class _VAR_NAME
{
private:
    String name = "NAME";

    String commandTopic = "COMMAND_TOPIC";

    std::function<void(void)> listener = []() {};

public:
    _VAR_NAME()
    {
        manager.clearRetain(commandTopic.c_str());
        manager.subscribe(commandTopic.c_str(), [this](String message)
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