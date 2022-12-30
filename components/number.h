#include "manager.h"

#define RETAIN false
#define MIN 0
#define MAX 0
#define STEP 0

// start
class _VAR_NAME
{
private:
    const String name = "NAME";

    const String commandTopic = "COMMAND_TOPIC";
    const String stateTopic = "STATE_TOPIC";

    const bool retain = RETAIN;

    std::function<void(double)> listener = [](double) {};

    double value = 0;

public:
    const double min = MIN;
    const double max = MAX;
    const double step = STEP;

    _VAR_NAME()
    {
        if (!retain)
        {
            device.clearRetain(commandTopic.c_str());
            device.clearRetain(stateTopic.c_str());
        }

        device.subscribe(commandTopic.c_str(), [this](String message)
                         {
                            value = message.toDouble();
                            device.publish(stateTopic.c_str(), message.c_str(), retain);
                            listener(value); });
    }

    double getValue()
    {
        return value;
    }

    void setValue(double newValue)
    {
        device.publish(commandTopic.c_str(), String(newValue).c_str(), retain);
    }

    void setNone()
    {
        value = 0;
        device.publish(commandTopic.c_str(), "None", retain);
    }

    // only one listener will work, newest overwrites previous
    void onChange(std::function<void(double)> _listener)
    {
        listener = _listener;
    }
} VAR_NAME;