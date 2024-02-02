# Home Assistant Device Builder - PlatformIO ESP32/8266 MQTT Discovery

This is personal project that I use for my _Smart Devices_ to easily connect them to homeassistant over MQTT whilst maintaining full control of the device's firmware, which differs from the ESPHome approach. It by no means is "better" or "faster" but you have more control to tinker with. ESPHome is a kind-of-magic "it just works" solution, whereas I like very odd and custom hardware setups, requiring custom firmware which I enjoy writing firmware, so for that usecase ESPHome is out of the picture and that's basically it, there was no firmware library I could find at the time, so I just wrote this myself for my own use.

# Setting up json schema

Open the user's vscode settings.json file

By default this is accomplished using Ctrl+Shift+P and typing `Preferences: Open User Settings (JSON)` (then press `Enter`)

Now insert the following code, replacing the example path with your own:

```
"yaml.schemas": {
    "C:\\{ABSOLUTE PATH}\\ha-device-builder\\ha-device.schema.json": "*.ha-device.yaml",
},
```

This will allow vscode to display using intellisense what properties exist within such a ha-device.yaml config file.

Syntax checking is also provided by the json schema.

# Setting up automatic compilation

Whilst it's possible to always manually type a command into the terminal to compile the config into the new device.h file, not only is that cumbersome, but also prone to errors.

The solution is to automate the process and recompile the config every time it gets saved.

Within vscode, install the extension "Run On Save"

In the global user settings.json file, add the following, replacing the example path with your own:
```
"emeraldwalk.runonsave": {
    "commands": [
        {
            "match": "\\.ha-device.yaml$",
            "cmd": "node \"C:\\{ABSOLUTE PATH}\\ha-device-builder\\ha-device-builder.js\" \"${file}\""
        }
    ]
},
```

# Debugging

Sooner or later you will run into some compilation errors because something is wrong.

The process does export any kind of a log file, however a terminal log is available.

This can be accessed by opening the terminal panel in vscode (default `Ctrl+J`),

opening the "OUTPUT" tab and then selecting from the drop-down menu "Run On Save".

Here you will see any errors that may occur during the processing of the ha-device config file.

# Getting started

Create a new PlatformIO Project or Open an existing one.

In the root directory (the one where .pio, src, ... are in), create a file with the ending `.ha-device.yaml`.

For example `my-smart-lamp.ha-device.yaml` and open it in vscode.

If you have installed the json schema correctly, vscode should now give you suggestions on all the possible properties you can have.

If not at first, try pressing `Ctrl+Space`, making sure to select the editor panel.

You can validate that by checking if there is a cursor blinking in the file editor.

To help guide you through this process, you can look at the default example `test.ha-device.yaml`.

In it, you will find the following demo config:

```
name: Lights
manufacturer: espressif
model: esp32
suggested_area: Bedroom
sw_version: 0.0.1
identifiers: wesiudfkhj2q93w8we
features:
  - class: button
    name: My Button
    unique_id: my-button-1
```

Once you save the config file (`Ctrl+S`) an `ha-device.h` file will be created/updated in the `include/` directory of your PlatformIO Project. Include it in your main.cpp and include the `device.init()` function in `setup()` as well as the `device.loop()` function in `loop()`.