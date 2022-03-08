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
| Field							| Unit			| Format		|
| ---							| ---			| ---			|
| Battery Voltage 				| Volts			| Float			|
| Battery Current				| Amps			| Float			|
| Battery Temperature			| Celsius		| Float			|
| Pack Charge					| %				| Unsigned Int	|
| BMS Fault						| N/A			| Bool			|
| GPS Course					| Degrees		| Float			|
| GPS Time						| N/A			| hhmmsscc		|
| GPS Date						| N/A			| ddmmyy		|
| GPS Latitude					| Degrees		| Float			|
| GPS Longitude					| Degrees		| Float			|
| GPS Speed						| MPH			| Float			|
| Solar Voltage					| ?				| ?				|
| Solar Current					| ?				| ?				|
| Motor Speed					| ? 			| ?				|

# Orion BMS Fields Reference
| Field							| BMS Field Name				| Default PID	|
| ---							| ---							| ---			|
| Battery Voltage				| Pack Voltage					| 0xF00D		|
| Battery Current				| Signed Pack Current			| 0xF00C		|
| Battery Temperature			| Heatsink Temperature Sensor	| 0xF02D		|
| Pack Charge					| Pack State of Charge			| 0xF00F		|
