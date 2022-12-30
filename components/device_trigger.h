#include "manager.h"

// start
class _VAR_NAME
{
private:
    const String name = "NAME";

    const String topic = "TOPIC";

public:
    void trigger()
    {
        device.publish(topic.c_str(), ".");
    }
} VAR_NAME;