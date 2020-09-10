These files are needed if you have a very old Powermax+ or an American style Powermax Pro (UK versions seems to work fine and some American models might also work - I suggest trying without, and if you do not get alarm status or zone names then try adding the Arduino - after discussion on SmartThings forum).

Since the UART pins of the Powermax are rubbish and sometimes the Wemos cannot understand the messages, I have used an Arduino (Nano 328, but could be another device) in between, in order to get clean timing/voltage of the Serial data. Without installing Arduino Studio and setting up libraries and burning bootloaders I suggest flashing with RCT (https://www.rc-thoughts.com/rct-firmware-uploader/). This firmware will also toggle the Arduino LED every time a valid command is received, this is really useful in troubleshooting, since you should see the LED flash (either very fast or very slow, based on what data is flowing at the time).

Once you have flashed the Arduino and the Wemos (or bought a programmed one) then these are the connections that you need to follow:
Note, it is better to connect the Wemos to 12V since the 12V power supply is stronger than any 5V supply you might find on the Powermax (and do not use USB ever)
Note, the Arduino draws tiny amounts of power hence can work on the 5V pin of Wemos without risk. Do not connect 12V to the Arduino, take power from the Wemos 5V pin.
![Wiring Diagram](WemosAndArduinoWiringDiagram.jpg?raw=true)

After you have made these connections, you should have a working communication between the Wemos and Powermax!
