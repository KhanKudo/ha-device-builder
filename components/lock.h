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

    std::function<void(bool)> listenerLock = [](bool) {};
    std::function<void()> listenerOpen = []() {};

    bool stateIsLocked = false;

public:
    _VAR_NAME()
    {
        if (!retain)
        {
            device.clearRetain(commandTopic.c_str());
            device.clearRetain(stateTopic.c_str());
        }

        device.subscribe(commandTopic.c_str(), [this](String message)
                         {
                            if(message != "LOCK" && message != "UNLOCK" && message != "OPEN") return;

                            if(message == "OPEN"){
                                listenerOpen();
                                return;
                            }

                            stateIsLocked = message == "LOCK";
                            device.publish(stateTopic.c_str(), stateIsLocked ? "LOCKED" : "UNLOCKED", retain);
                            listenerLock(stateIsLocked); });
    }

    // true if locked, false if unlocked
    bool isLocked()
    {
        return stateIsLocked;
    }

    void setLock(bool isLocked)
    {
        device.publish(commandTopic.c_str(), isLocked ? "LOCKED" : "UNLOCKED", retain);
    }

    // only one listener will work, newest overwrites previous, bool argument true if locked, false if unlocked
    void onLockChange(std::function<void(bool)> _listener)
    {
        listenerLock = _listener;
    }

    // only one listener will work, newest overwrites previous
    void onOpen(std::function<void()> _listener)
    {
        listenerOpen = _listener;
    }
} VAR_NAME;