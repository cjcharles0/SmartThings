These files are needed if you have a very old Powermax+ or an American style Powermax Pro (UK style seems to work fine and some American models will work).

Since the UART pins of the ESP dont reliably work with the Powermax and I have used an Arduino in between in order to get clean timing of the Serial data. Without installing Arduino Studio and setting up libraries and burning bootloaders, I have added XLoader into the Github folder (http://www.hobbytronics.co.uk/arduino-xloader) since this should make it easier.

Once you have flashed the Arduino and the Wemos then these are the connections that you need to follow: (note that it is still better to connect the Wemos to 12V since the 12V power supply is stronger than the 5V supply, however there is only a tiny risk here, hence a 5V connection may be easier. Do not connect both 5V and 12V)
![Wiring Diagram](WemosAndArduinoWiringDiagram.jpg?raw=true)

After you have done this you should have a working communication between the Wemos and Powermax
