# Introduction #

minRXOSD is the firmware for an OSD which shows CRC checked good/bad packet information of the well known and very well working Scherrer UHF LRS: http://www.tslrs.com/

minRXOSD uses a cheap hardware known as MinimOSD (about 18$) and can be used standalone or in conjunction with another OSD like, in this example, the minOPOSD: http://code.google.com/p/minoposd/

It's based on the ArduCamOSD project: http://code.google.com/p/arducam-osd/


---


Here you can see a screenshot with a few bad packets.

In the blue rectangle you see the bad packets warning.

In the green rectangle you see the disturbed channels.

In the red rectangle you see the failsafe count (currently zero), the packet count with CRC errors, the over all packet count and the percentage of good/bad packets of the last 4.5 seconds calculated out of the B:x and G:y data.

http://minrxosd.googlecode.com/svn/wiki/images/V27_in_flight_new_version_002.JPG


---


Have fun