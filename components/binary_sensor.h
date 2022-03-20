#include "manager.h"

#define RETAIN false

// start
class _VAR_NAME
{
private:
    String name = "NAME";

    String stateTopic = "STATE_TOPIC";

    bool retain = RETAIN;

public:
    _VAR_NAME()
    {
        if (!retain)
        {
            manager.clearRetain(stateTopic.c_str());
        }
    }

    void update(bool newState)
    {
        manager.publish(stateTopic.c_str(), newState ? "ON" : "OFF", retain);
    }
} VAR_NAME;