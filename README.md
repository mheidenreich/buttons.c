# buttons.c
This 2-part Raspberry Pi GPIO tutorial shows how to correctly use buttons with wiringPi.
While I use the buttons to control LEDs, you can use the same approach with any other electronic components (or code in a subroutine/function in general).

Companion YouTube video: https://www.youtube.com/watch?v=9Znf0CnsbXI

gpio command not working or buttons.c does not compile on your system?

Original wiringPi library is deprecated by the original developer and it does not work with latest versions of
Raspberry Pi. If you have Raspberry Pi 4 or 400 you will receive an “unknown board” error when
attempting to use the gpio command. Also, latest version of Raspberry Pi OS may not even have the gpio
command installed, which results in a “command not found error”.

You can download and build a wiringPi fork to have wiringPi on your latest Pi: https://github.com/WiringPi/WiringPi/
Simply follow the INSTALL file instructions.
