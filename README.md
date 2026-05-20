# Yo, just made a little Diesel-engine simulator to poke around in.
Max torque number i got was 202Nm and 166.3 hp. Made this for fun, it is designed around the peugeot engine DV6C or rather the volvo D4162T engine more specifically with a tweaked turbo and with different coolant options.
Works for demonstrating simple engine internal workings.
Runs in terminal.
## Controls
Simple controls: At launch you can either input nothing and the engine will idle. Or input your desired throttleposition for a start at that throttleposition, while it is printing you can type whenever you want to chagne the throttleposition. You can also input a total time you want, what throttleposition you want and how long it should do that throttleposition for.
Example: 10 50 8

While the program is running you can input "p" to pause the terminal output and then press C to make it continue when it is paused. or you can input q and the program will exit.

## What does it simulate?

Mostly just shows off what happens to a engine when it is running for demonstration or to understand it.
Some components have health that will signal when it gets damaged to indicate that the car gets hurt by pushing it too much. The wear and tear is not super accurate but at least an indication to the wear the engine takes from pushing it. 
