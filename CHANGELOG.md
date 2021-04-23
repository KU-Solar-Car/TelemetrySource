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