Butterfly Logger 20080529 v0.28

Changes:
- New definitions in main.h to remove all the LCD, menu and joystick code for a serial only interface.
- Usart 'h' command to display a message on the LCD
- Usart 'H' command to print the data headers 
- Changes to the makefile to produce smaller code with 4.3.0 compiler
- No longer need to comment out unused source files from the makefile as linker now removes unused code.

This project was built using the GNU GCC tool-chain on MacOS X 10.5.2
using AVRMacpack20080514 ( gcc4.3.0)

It will also build under windows with WinAVR. (tested winXP WinAVR 20080411)

To build simply type "make" at the command prompt in the source directory

More documentation can be found at 

Home Page:
http://butterflylogger.sf.net/

Forums:
http://sourceforge.net/forum/forum.php?forum_id=489504

Wiki:
http://brokentoaster.com/wiki/doku.php?id=butterflylogger
