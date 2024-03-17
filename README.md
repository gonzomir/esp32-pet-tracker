# ESP32 Pet Tracker

A simple pet tracker based on ESP32 board with LTE and GNSS module ([LiliGO T-SIM7600G-H](https://www.lilygo.cc/products/t-sim7600)) that should send data to Home Assistant `device_tracker` entity via REST api.

Relevant documentation and examples:

- [LiliGO T-SIM7600 example code on GitHub](https://github.com/Xinyuan-LilyGO/T-SIM7600X)
- [TinyGSM library](https://github.com/vshymanskyy/TinyGSM/) and [PR for adding HTTPS support to SIM7600](https://github.com/vshymanskyy/TinyGSM/pull/767)
- [Home Assistant REST API docs](https://developers.home-assistant.io/docs/api/rest/)
- [Home Assistant `device_tracker` entity documentation](https://developers.home-assistant.io/docs/core/entity/device-tracker/)

## Initial Plan / Requiremens

### HASS side

- Define the device tracker in HASS configuration YAML
- Update the device tracker state via REST API

### ESP32 side

- HASS URL and authentication should be read from external config / environment variables
- Maybe use some device info to create the tracker ID? Or hardcode it?
- Device should recognise home WiFi and go into deep sleep when at home

## How to run

I got the basics working - connect to LTE network, get location gron GNSS and send the data to Home Assistant.

### ESP32 board setup

To install the software on the ESP32 side complete these steps:

1. Clone the project `git clone https://github.com/gonzomir/esp32-pet-tracker.git`
2. Go into the project folder and install dependencies. I use VSCode which does all that for me, the command should be `pio pkg install`.
3. Copy `src/secrets-example.h` to `src/secrets.h` and set the appropriate values.
3. Build and upload to th board `pio run -t upload`

If using VSCode monitor the serial console to see if everything works as expected.

### Home Assistant setup

In Home Assistant I just added a device in `known_devices.yaml`:

```
pet_tracker:
  name: Dog's name
  icon: mdi:dog-side
  track: true
```
Then the ESP32 board updates it's state through the REST API.

And VOILA, you can now track you pet on the HASS' map screen.
