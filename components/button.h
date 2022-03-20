#include "manager.h"

// start
class _Button
{
private:
    String name = "NAME";

    String commandTopic = "COMMAND_TOPIC";

    std::function<void(void)> listener = []() {};

public:
    _Button()
    {
        manager.subscribe((char *)commandTopic.c_str(), [this](String message)
                          { listener(); });
    }

    // only one listener will work, newest overwrites previous
    void onPress(std::function<void(void)> _listener)
    {
        listener = _listener;
    }
} VAR_NAME;