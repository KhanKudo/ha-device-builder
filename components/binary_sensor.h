#include "manager.h"

#define RETAIN false

// start
class _VAR_NAME
{
private:
    const String name = "NAME";

    const String stateTopic = "STATE_TOPIC";

    const bool retain = RETAIN;

public:
    _VAR_NAME()
    {
        if (!retain)
        {
            device.clearRetain(stateTopic.c_str());
        }
    }

    void update(bool newState)
    {
        device.publish(stateTopic.c_str(), newState ? "ON" : "OFF", retain);
    }
} VAR_NAME;