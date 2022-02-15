# Telemetry
Arduino system to process and transmit car data. Uses an Arduino Due with the following additional hardware:
* Digi Xbee3 LTE-M cellular modem connected to Serial2 for transmitting data
* Garmin 15x-W GPS connected to Serial1
	* White wire = ground (not needed)
	* Black wire = ground
	* Red wire = 3.3v
	* Yellow wire = pin 19 Rx1
	* Blue wire = pin 18 Tx1
	* Orange wire = ground (not needed)

# Setup
* Install Arduino Due board using Arduino IDE boards manager
* Install the following libraries using Arduino IDE library manager:
	* TinyGPS
	* DueTimer
	* Scheduler
* Install the [Arduino-MemoryFree](https://github.com/mpflaga/Arduino-MemoryFree) library manually to %UserProfile%\Documents\Arduino\libraries

# Impotant XBee note
The XBee modem starts atuomatically upon receiving power and must always be
shutdown safely before disconnecting power, or else there is risk of permanent
hardware damage. The green DI05 light on the XBee Arduino shield will be 
blinking when the modem is on and solid when the modem is off.

# Transmitted data
| Field					| Unit/Format	|
| ---					| ---			|
| Battery Voltage		| ?				|
| Battery Current		| ?				|
| Battery Temperature	| ?				|
| BMS Fault				| Boolean		|
| GPS Course			| Degrees		|
| GPS Time				| hhmmsscc		|
| GPS Date				| ddmmyy		|
| GPS Lat, Lon			| Degrees		|
| GPS Speed				| MPH			|
| Solar Voltage			| ?				|
| Solar Current			| ?				|
| Motor Speed			| ? 			|