= ESP32 Pet Tracker

I'm creating a simple pet tracker based on ESP32 board with LTE and GNSS module (LiliGO T-SIM7600G-H) that should send data to Home Assistant `device_tracker` entity via REST api.

Relevant documentation and examples:

- [LiliGO T-SIM7600 example code on GitHub](https://github.com/Xinyuan-LilyGO/T-SIM7600X)
- [TinyGSM library](https://github.com/vshymanskyy/TinyGSM/) and [PR for adding HTTPS support to SIM7600](https://github.com/vshymanskyy/TinyGSM/pull/767)
- [Home Assistant REST API docs](https://developers.home-assistant.io/docs/api/rest/)
- [Home Assistant `device_tracker` entity documentation](https://developers.home-assistant.io/docs/core/entity/device-tracker/)

== Initial Plan / Requiremens

=== HASS side

- Define the device tracker in HASS configuration YAML
- Update the device tracker state via REST API

=== ESP32 side

- HASS URL and authentication should be read from external config / environment variables
- Maybe use some device info to create the tracker ID? Or hardcode it?
- Device should recognise home WiFi and go into deep sleep when at home
