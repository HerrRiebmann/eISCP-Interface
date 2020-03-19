# eISCP-Interface
Network AV Receiver remote control for Onkyo &amp; Pioneer

Startup my Pioneer VSX-1131 with an ESP32 OLED (Wemos) microcontroller, via eISCP interface.

* Starting
* selecting the right channel (NET)
* select favorite (TuneIn radio)
* find right station
* set volume

[YouTube Video](https://www.youtube.com/watch?v=cbbp1mKjVOo)

### File-Agenda
* [eISCP_Interface.ino](/eISCP_Interface.ino) Start Sketch (*setup, loop, globals*)
* [Udp.ino](/Udp.ino) Sends UDP-message to local network, to retrieve device Ip adress
* [Sockets.ino](/Sockets.ino) Sends and Receives data
* [Incoming.ino](/Incoming.ino) processes the incoming message to extract command and data
* [Outgoing.ino](/Outgoing.ino) creates the outgoing message with header-data
* [TestControl.ino](/TestControl.ino) executes the commands to select a specified TuneIn station
* [Output.ino](/Output.ino) displays some data to the wemos OLED

### Improvements
- [ ] Optimize WaitForResponse()-Function
- [ ] Replace Title- & ElapsedTime-globals with a more flexible class
- [ ] Optimize display output

## Note:
You have to add the desired radio channel to TuneIn previously, yourself! Then change `while (!MessageText.startsWith("RADIO BOB!"))` with your predilection.
