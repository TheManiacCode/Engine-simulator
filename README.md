# Yo, just made a little Diesel-engine simulator to poke around in.
Max torque number i got was 214Nm and 174 hp. Made this for fun, it is designed around the peugeot engine DV6C or rather the volvo D4162T engine more specifically with a tweaked turbo and with different coolant options.
Works for demonstrating simple engine internal workings.
program runs in terminal.
t=368.0s RPM=5789.4 Torque=214.3Nm hp:174.2 Throttle=0.0% Boost=2.7bar Fuel=1.4L/h Remaining=59.9L Coolant=25.4C Oil=40.5C Intake=68.8C Exhaust=258.5C

## Controls
Simple controls: At launch you can either input nothing and the engine will idle. Or input your desired throttleposition for a start at that throttleposition, while it is printing you can type whenever you want to chagne the throttleposition. You can also input a total time you want, what throttleposition you want and how long it should do that throttleposition for.
Example: 10 50 8

While the program is running you can input "p" to pause the terminal output and then press C to make it continue when it is paused. or you can input q and the program will exit.

The terminal prints a big string that indicates what is happening to the engine, see example below:
> t=371.0s RPM=2607.6 Torque=176.5Nm hp:64.6 Throttle=0.0% Boost=1.8bar Fuel=1.2L/h Remaining=59.9L Coolant=25.7C Oil=40.7C Intake=66.8C Exhaust=264.0C
1. The "t" stands for time passed in simulation.
2. rpm stands for revolutions per minute that the motor is doing at this moment.
3. Torque is the amount of work output at the current revolution measured in Newtonmeters, 
4. hp stands for horsepower and is the work output per revolution per minute, 
5. throttle is how far the throttlepositionsensor detects the throttlepedal being pushed, 
6. boost is the amount of airpressure inserted into the engine and is measured in bar or about (100 000 pascal), 
7. Fuel is the consumption of fuel per hour during current operational standards and is measured in liters per hour. 
8. Remaining is the amount of fuel left in a theoretical fuel tank that gets consumed. 
9. Coolant is the temperature of the engine coolant cooling down the engine, it is measured in celsius. 
10. Oil is the temperature of the oil, measured in celsius. 
11. Intake is the temperature of the air that gets into the engine and is measured in celsius. 
12. Exhaust is the temperature of the exhaust gases from the engine measured in celsius.  

## What does it simulate?

Mostly just shows off what happens to a engine when it is running for demonstration or to understand it.
Some components have health that will signal when it gets damaged to indicate that the car gets hurt by pushing it too much. The wear and tear is not super accurate but at least an indication to the wear the engine takes from pushing it. 
The exhaust gases sensors is not something I have a clue about so the readings are wierd but should give you an indication of what it reads.
## Can?
Yes, I have a can sender that should send out decent can messages. My idea is to use a vcan to send the can messages on and pick them up on another script using a simple python script.
If you want to you could when running the script also add where the can messages should go, example: ./engineSimulator vcan0