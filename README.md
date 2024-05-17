# IR sensor for First Year Project

![Schematic](Schematic.png)

Circuit output goes to PA04 - A3.
Code is in /src/ - use AC_INT_ONLY for now

The code enables the built in analog comparator and routes PA04 to one of its input, and the internal 1.1V bandap reference to the other. It generates an interrupt on every rising edge of the IR sensor. The frequency is measured in the ISR by using the micros() function.

The other script routes the comparator output to a timer to measure the period without using interrupts but is less accurate for some reason.
