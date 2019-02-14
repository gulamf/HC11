HC11 CC1101
==============

This a conversion of the CC1101 Arduino Library for the HC11 module.


Usage
-----
compile.sh to compile example.c

flash.sh to burn image to HC11.

see example.c on how to send and receive data.



CREDITS
-------
Arduino Library from:
https://github.com/veonik/arduino-cc1101.git

HC11 Module info from :
https://mvdlande.wordpress.com/2016/09/03/reprogramming-a-hc-11-cc1101-433mhz-wireless-transceiver-module/

STM8 Library from
https://github.com/lujji/stm8-bare-min

SOFTWARE
--------
Compiler used SDCC v3.8

HARDWARE
--------
Writing the image file to HC11 requires a STLINK/V2 programmer.
Connect RST,SWIM,GND,3.3V to programmer and use stm8flash to burn an ihx image file to the module.

NOTE
----
On fresh HC11 boards, you may need to unlock the module for writing. Use..

stm8flash -c stlinkv2 -p stm8s003f3 -u  

