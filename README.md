<img atl="CHICK - hicken House Illumination Control boKs" src="https://raw.github.com/joshcam/CHICK/master/doc/images/Emojione_1F425.png" width="100">This repository contains 3D hardware designs, firmware and other documentation for CHICK.<img atl="CHICK - Chicken House Illumination Control boKs" src="https://raw.github.com/joshcam/CHICK/master/doc/images/Emojione_1F425.png" width="100">

## Project Description
CHICK [Chicken House Illumination Control boKs] is an open source, simple to use, minimalistic approach daylight automated 12 volt powered LED light controller. Its purpose is to extend daylight hours in a chicken house/coup thus promoting greater egg production.
You can read more info about that here: https://poultrykeeper.com/general-chickens/light-for-layers/

Some years ago I installed a solar panel, charge controller, battery bank and LED lighting with timer and manual functions in my mothers chicken coup. The timer was an off the shelf product that I modified to work with low voltage. It worked fairly reliably but the time on and off had to be adjusted with the seasons quite often. I wanted a simple controller without the need to set timers, adjusted time for DST, and change time with the seasons. And did I say I wanted it to be SIMPLE?!

<img atl="Assembled CHICK" src="https://raw.github.com/joshcam/CHICK/master/doc/images/inside_assymbled_01a.jpg">

Thingiverse files for CHICK: https://www.thingiverse.com/thing:2595492

## Operation
Operating CHICK goes like this. There is a manual button, push on/push off. You can turn the lights on or off without affecting the current on/off cycle. To set the on time (or light level at which it triggers the lights to turn on) you simply do this: At the time of day when the light level reaches the point where you want the lights to come on, say at dusk you simply hold down the set button and wait for the LEDs to flash. The code takes multiple samples over 3 seconds to verify a good reading then saves that level to EEPROM. Now you just press the hours button until the LED corresponding to the number of hours you want the lights to remain on is lit (1-5). This is also saved to EEPROM. Your done, when the sun goes down and the light reaches the level at which you specified the code begins a 5 minute verification checking the level once each minute to ensure the level is still below the trigger level. Once that condition is met the lights are turned on and remain on for the time set in hours. Once the time is up the lights shut off and are now waiting for a light cycle (dawn) above the trigger level to reset at which point the code again is waiting for dusk. If the lights are triggered by a temporary dark event such as clouds, solar eclipse, a bird sitting on the sensor, etc. an increase in light level above the saved trigger point will result in the count down timer to shut the lights off being sped up significantly.

The code is well documented and any questions about it's operation should be able to be gleaned from there, if not feel free to open an issue or send me a message directly.

## Open Source
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a>  <a rel="license" href="http://creativecommons.org/freeworks"><img alt="Approved for Free Cultural Works" style="border-width:0" height="32" width="32" src="http://creativecommons.org/images/deed/seal.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.
