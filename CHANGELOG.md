# v0.2.0

Milestone reached: Successfully grabbing data off of the CAN bus and transmitting it

### Changes:

* Created a CAN ISR that updates testStats every time CAN data becomes 
available

* Fixed a bug where sending no data would result in sending invalid json 
(just the "}" character in the HTTP body)

* Made testStats volatile-qualified

* Refactored TelemetryData to wrap the entire array of data

### Issues:

* It would be nice to ensure that the getDouble(), getBool(), and getUInt() 
methods of TelemetryData don't return data if it is not present. But I
don't know the best way to implement this without the ability to use
exceptions.

* I don't completely understand why the member funcitons of TelemetryData 
have to be volatile-qualified, and why, when giving a function a 
volatile-qualified parameter, it becomes a reference.

# v0.1.1

First time reliably(ish) sending more data.

### Changes:

* Refactored the structs used to represent the data we're sending

* Check that the modem is associated with the network before sending data

* Improve the read() function and refactor the rxBuffer
 
### Issues:

* There's nothing protecting requestBuffer (for our http request)
from overflowing. As you extend the program, make sure
REQUEST\_BUFFER\_SIZE is big enough in Telemetry_Transmitter.ino.

* If you're not getting a response try getting a better signal (e.g.
going outside)
 
* Sometimes the XBee fills up the Arduino's serial buffer
faster than we read it, and bytes are lost. In the future, we will need 
to speed up the reading, possibly by removing some debug statements. For
now, this isn't mission critical.

# v0.1.0

First time successfully posting from Arduino.

### Changes:

* Create the initial XBee library:
   * Shutting down gracefully (important)
   * Sending, receiving, verifying packets in general using API mode
   * Sending TCP packets
   
* Sent some test data to the Google Cloud server

* Create MonitoredSerial class to help debug communication between
Arduino and XBee
