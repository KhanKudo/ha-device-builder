#include "manager.h"

#define RETAIN false

// start
struct _VAR_NAME {
private:
  const String name = "NAME";

  const String commandTopic = "COMMAND_TOPIC";
  const String stateTopic = "STATE_TOPIC";

  const bool retain = RETAIN;

  std::function<void(bool)> listener = [](bool) {};

  bool state = false;

  void updateState(bool newState) {
    state = newState;
    device.publish(stateTopic.c_str(), newState ? "ON" : "OFF", retain);
    listener(newState);

    // device.checkAutomation(name, (uint32_t)state);
  }

public:
  void _init() {
    if (!retain) {
      device.clearRetain(commandTopic.c_str());
      device.clearRetain(stateTopic.c_str());
    }

    device.subscribe(commandTopic.c_str(), [this](String message) {
      if (message != "ON" && message != "OFF")
        return;

      updateState(message == "ON");
    });
  }

  bool getState() { return state; }

  void setState(bool newState) { updateState(newState); }

  // only one listener will work, newest overwrites previous
  void onChange(std::function<void(bool)> _listener) { listener = _listener; }
} VAR_NAME;