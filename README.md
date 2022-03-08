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
| Field							| Unit			| Type			|
| ---							| ---			| ---			|
| pack_voltage	 				| Volts			| float			|
| pack_soc		 				| %				| float			|
| pack_current	 				| Amps			| float			|
| min_pack_temp	 				| Celsius		| float			|
| avg_pack_temp	 				| Celsius		| float			|
| max_pack_temp	 				| Celsius		| float			|
| min_cell_voltage 				| Volts			| float			|
| avg_cell_voltage 				| Volts			| float			|
| max_cell_voltage 				| Volts			| float			|
| input_voltage 				| Volts			| float			|
| bms_fault		 				| Bit flags		| unsigned int	|
| motor_speed					| MPH			| float			|
| motor_current					| Amps			| float			|
| motor_controller_temp			| Celsius		| float			|
| motor_temp					| Celsius		| float			|
| motor_fault					| Bit flags		| unsigned int	|
| solar_voltage	 				| Volts			| float			|
| solar_fault	 				| Bit flags		| unsigned int	|
| gps_course	 				| Degrees		| float			|
| gps_time						| hhmmsscc		| string		|
| gps_date						| ddmmyy		| string		|
| gps_lat						| Degrees		| float			|
| gps_lon						| Degrees		| float			|
| gps_speed						| MPH			| float			|

# Orion BMS Fields Reference
| Field							| BMS Field Name				| Default PID	|
| ---							| ---							| ---			|
| Battery Voltage				| Pack Voltage					| 0xF00D		|
| Battery Current				| Signed Pack Current			| 0xF00C		|
| Battery Temperature			| Heatsink Temperature Sensor	| 0xF02D		|
| Pack Charge					| Pack State of Charge			| 0xF00F		|
