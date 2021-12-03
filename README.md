# Telemetry
Arduino system to process and transmit car data. Uses an Arduino Due with the following additional hardware:
* Digi Xbee3 LTE-M cellular modem connected to Serial2 for transmitting data
* Garmin 15x-W GPS connected to Serial1

# Setup
* Install Arduino Due board using Arduino IDE boards manager
* Install the following libraries using Arduino IDE library manager:
	* TinyGPS
	* DueTimer
* Install the [Arduino-MemoryFree](https://github.com/mpflaga/Arduino-MemoryFree) library manually to %UserProfile%\Documents\Arduino\libraries