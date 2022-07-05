# ESP32 ComfortLights IoT
### Plug&Play sensors-driven smart lights, user-programmable, IoT ready
#### WORK IN PROGRESS
#

The device uses 3 different ambient sensors and data-smoothing procedures to accurately detect human presence and avoid false-positives:

- HCSR501 passive infrared sensor
- BH1750 ambient light sensor
- HCSR04 ultrasonic distance sensor
#
On the first run the device sets itself as a Wi-Fi access point, providing an user-friendly web interface to program the device:
- connection to existing wireless network
- MQTT protocol setup

No further setups are needed, settings are saved persistently on the device
#
The device has a single touch-sensitive button to control it:
- short press `<1s no feedback`
	- toggle the lights
- long press `>5s blinking led feedback`
	- toggle sensors automation
- very long press `>10s stationary led feedback`
	- factory reset

Further iterations will implement a clickable potentiometer for manual brightness control
#
As a MQTT client the device is fully IoT capable and easily implementable in common home-automation systems and services. Given the MQTT broker settings and the device `main_topic` (E.g. room of operation), the device will automatically set its own topics as follows:

- `main_topic/device_id/lights/status`
- `main_topic/device_id/sensors/status`

The device subscribes to `lights` and `sensors` topics to accept commands (such as brightness control, etc.) and publishes its own status updates in the `status` sub-topics
#
External dependencies / many thanks to:
- [BH1750](https://github.com/claws/BH1750) by claws
- [HCSR04](https://github.com/Martinsos/arduino-lib-hc-sr04) by martinsos
- [WiFiManager](https://github.com/tzapu/WiFiManager.git) by tzapu
- [arduino-mqtt](https://github.com/256dpi/arduino-mqtt) by 256dpi
