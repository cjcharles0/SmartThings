# Instructions for setting up Powermax into SmartThings

This code has been written to interface an ESP8266 Wemos D1 R2 into SmartThings. The Arduino code is highly based on the
Powermax C++ library created on https://www.domoticaforum.eu and then integrated onto a Wemos D1 R2 by irekz
(https://github.com/irekzielinski/PowerMaxAlarm). I have then modified it and created some extra functions in order to
interface with SmartThings.

Some important points to note:
1) The website created by the ESP8266 is currently not password protected (on my to do list eventually). I take no responsibility for your Powermax security!
2) It is currently a bit manual to get all devices set up, hence I strongly advise you make SmartThings and the Wemos have a fixed IP address in your router config.
3) I am using Arduino Studio 1.6.9 and version 2.3.0 of the ESP8266 board library. There is no reason I can think of, why it would not work with another version, but if you are having problems please ensure you use this version.

The key steps to follow when setting up the integrations are as follows:
1) First set up Arduino Studio and add the Wemos D1 R2 (go to Tools menu, then Boards, then board manager and search for ESP8266)
2) Confirm that you now have the required ESP8266 libraries installed (if not you will need to add them from Sketch/Include Libraries/Manage Libraries)
3) Connect the Wemos D1 R2 to your computer and ensure the drivers are correct in order for it to show up in Arduino Studio
4) 
