#include "manager.h"

#define RETAIN false
#define NUMBER_OF_OPTIONS 0

// start
struct _VAR_NAME
{
private:
    const String name = "NAME";

    const String commandTopic = "COMMAND_TOPIC";
    const String stateTopic = "STATE_TOPIC";

    const bool retain = RETAIN;

public:
    enum Option
    {
        // __insert-option-list-enum
    };

private:

    Option state;

    std::function<void(Option)> listener = [](Option) {};

public:
    const String options[NUMBER_OF_OPTIONS] = {
        // __insert-option-list
    };

   void _init()
    {
        if (!retain)
        {
            device.clearRetain(commandTopic.c_str());
            device.clearRetain(stateTopic.c_str());
        }

        device.subscribe(commandTopic.c_str(), [this](String message)
                         {
                            state = stringToOption(message);
                            device.publish(stateTopic.c_str(), message.c_str(), retain);
                            listener(state); });
    }

    String optionToString(Option option)
    {
        return options[option];
    }

    Option stringToOption(String str_option)
    {
        for (int i = 0; i < NUMBER_OF_OPTIONS; i++)
        {
            if (options[i].equals(str_option))
            {
                return (Option)i;
            }
        }

        Serial.println("Invalid stringToOption value: " + str_option);

        delay(5000);
        return (Option)0;
    }

    Option getState()
    {
        return state;
    }

    void setState(Option option)
    {
        device.publish(commandTopic.c_str(), optionToString(option).c_str(), retain);
    }

    // only one listener will work, newest overwrites previous
    void onChange(std::function<void(Option)> _listener)
    {
        listener = _listener;
    }
} VAR_NAME;