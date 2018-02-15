In order to trigger an alarm on the Powermax, from the Wemos, then it is currently not possible since Visonic have not given any information about how the commands might work (with much trial and error I could not find it). As a result to trigger an alarm, we need to wire the Wemos to pretend to be a wired zone of the Powermax.

The diagram below shows two methods of doing this, though I suggest the first option, since you can then trigger a tamper event, or an alarm event - An Alarm event will only set off the alarm when the system is armed, but a tamper event should trigger the alarm unless you are in Installer mode (hence can be a panic button too).

In order to do this, you just need a few wires from the Wemos to relay board, and a few wires (plus two resistors) to connect to the Powermax. I suggest trying to find a similar 5V relay module since this one supports 3V3 on the Wemos connection pins (IN1 and IN2), but also 5V which is needed for the relay - this also includes an optoisolator which will help for safety and to ensure the voltages remain separate. Any questions or problems, I suggest asking on the SmartThings forum.

![Connections to Trigger an Alarm](AlarmTriggerConnections.jpg)
