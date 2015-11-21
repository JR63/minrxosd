# How to compile and flash the code #


How to compile and flash arduino code is documented many times on the web.

A good entry point for how to start with arduino and to compile the code is here: http://arduino.cc/en/Guide/HomePage

A good entry point for how to flash the code is here: http://code.google.com/p/arducam-osd/wiki/Cfg_Update_Firmware

But don't use their hex file of course, but compile the right one for minRXOSD.

The code is here:

http://code.google.com/p/minrxosd/source/checkout

And use the 'OSD Config Tool' from my code section trunk/Tools, it's the same as of my other project minOPOSD.

Before you compile the minRXOSD hex file you can change the layout and the critical percent threshold for your needs by editing the following defines in OSD\_Panels.ino:


#define CRITICAL\_PERCENT

#define SHOW\_MINIMAL

#define PAN\_WARN\_X

#define PAN\_WARN\_Y

#define PAN\_CHAN\_STAT\_X

#define PAN\_CHAN\_STAT\_Y

#define PAN\_RX\_STAT\_X

#define PAN\_RX\_STAT\_Y