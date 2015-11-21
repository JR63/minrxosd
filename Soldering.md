# How to solder OSD and RX #


---


Only do this if you have some soldering skills.

Also only do this if you have the knowledge of what you are doing.


---


Since I own the old version V0.1 of the MinimOSD with the heating problem, I first of all desoldered the diode to isolate the 12V from the OSD and powered it from the 5V side.

In the following two pictures you can see how to do so.

Shown above is a unsoldered MinimOSD and below a soldered MinimOSD.

Desolder the diode (red rectangle) to isolate the 12V from the OSD.

Solder the two solder jumper (green rectangles) to powered the MAX7456 chip from the 5V side.

http://minrxosd.googlecode.com/svn/wiki/images/001_MinimOSD_front_soldered.JPG

http://minrxosd.googlecode.com/svn/wiki/images/002_MinimOSD_back_soldered.JPG


---


For powering the MinimOSD and to make screen switching possible I used a normal servo cable and soldered it as shown in this picture:

http://minrxosd.googlecode.com/svn/wiki/images/003_MinimOSD_wire_soldered.JPG

Be careful that you don't short-circuit anything with this soldering!

After you soldered and checked the OSD don't forget to heat-shrink it properly, so that no pin headers can be short-circuit!

This servo cable is connected to a free channel of the LRS RX later to power the OSD and to make screen switching possible.


---


For sending the data from the LRS RX to the OSD we need to connect the receivers serial TX and GND to the OSD RX and GND.

Because we already have GND using the servo cable as shown above we only need the serial TX wire, which is the red one in the following picture.

But as we like to update the LRS RX sometimes, it's a good idea also to solder the serial RX wire, which is the brown one in the following picture.

The TX and RX wires are enough because you can use GND from the servo rail when updating the LRS RX.

The yellow wire is the analog RSSI which some OSD use, it's not necessary for this OSD.

http://minrxosd.googlecode.com/svn/wiki/images/004_Scherrer_LRS_RX_wire_soldered.JPG

The TSLRS RX properly heat-shrinked:

http://minrxosd.googlecode.com/svn/wiki/images/005_Scherrer_LRS_RX_shrinked.JPG