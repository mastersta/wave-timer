##Wave Controller

####Circuit Schematic

![Circuit Schematic](https://raw.githubusercontent.com/mastersta/wave-timer/wave-controller/diag.jpg)

This board that I designed is being used in a commercial setting where I'm controlling the broken features of a wave machine at an old waterpark. It's providing three functions for me, with all the logic being provided by an arduino nano:

* Controlling the on/off cycle; The intended behavior of the system is that the waves are running for a ten minute cycle, and then are off for fifteen minutes (in which the pool is static, like a regular pool). The old wave machine is broken in such a way that the on/off switch simply turns the waves on and they stay on until the switch is turned off. Since this switch is a simply continuity-type switch that has 120VAC @ 0.2A going through it, I'm simply inserting a relay in series with it that is closed for ten minutes and open for fifteen.

* Controlling the air diverters; Four large (80HP, 480VAC 3-phase) induction motors pump air into these diverters, which control which chamber at the end of the pool that the air is pumped into. By diverting this air back and forth between adjacent chambers, the system is able to make large waves that travel down the pool. These diverters are powered by air pistons that are controlled by solenoids. When 120VAC is applied to the solenoids (which draw 0.4A total), the pistons actuate in one direction and stay there until power is removed, at which point they actuate back to their resting position. All four diverters run off of one circuit, so I'm able to simply apply and remove 120VAC to that circuit to actuate these diverters. I control the time they spend on each side of their cycle by reading a couple of potentiometers and converting that into milliseconds. I apply these times to the duty cycle of the relay that applies and removes power from the solenoids.

* Turning on a bell that rings at the start of the cycle to let swimmers know that the waves are about to start. Simple relay setup, attaching 12VDC to the bell for three seconds at the start of the cycle.

The board is powered by a 300 watt PC power supply, supplying +5VDC for the arduino and relays, and +12VDC for the bell.

There is also a seven-segment display located on the front of the housing that displays the time left in the current cycle.
