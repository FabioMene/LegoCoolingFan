# LegoCoolingFan - A cooling fan built with legos
This is what happens when you have a big pc fan and some legos laying around

I didn't have pnp transistors (nor p-mosfets) to make an h-bridge to drive the sweep motor
so i've exploited the output stage of two TDA2030A op-amps. I've used an Arduino Uno R3 board,
which has an onboard 5 volt regulator, but I've used a 7805 anyway because I want to remove the
arduino and leave the bare ATmega328p MCU with a crystal

Schematic
![schematic](https://github.com/FabioMene/LegoCoolingFan/raw/master/schematic.png)

I didn't have any perfboard piece (I know, I'm not that well-equipped) so i've taped the components
directly on lego structure, I just need some time to remake this properly

Upper left
![upper left shot](https://github.com/FabioMene/LegoCoolingFan/raw/master/up-left.jpg)

Upper frontal
![upper frontal shot](https://github.com/FabioMene/LegoCoolingFan/raw/master/front.jpg)

Upper right
![upper right shot](https://github.com/FabioMene/LegoCoolingFan/raw/master/up-right.jpg)

Lower frontal
![lower frontal shot](https://github.com/FabioMene/LegoCoolingFan/raw/master/lower-front.jpg)
