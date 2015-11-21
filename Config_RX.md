# How to config TSLRS RX #


You have to install the version V2.7 or newer from Thomas Scherrer to your LRS RX for using the following.


---


For using the faster live channel bad info you have to enable the debug channel feature on the LRS RX:

For enabling the debug channel feature you have to jumper the signal pins 3 and 4 of the RX, power the RX, wait 5 seconds and unpower it.

When everything went well, you will see the following string when powering the LRS RX with the OSD: 'MODE FLAGS 02'


---


You can disable this feature if you don't like it:

For disabling the debug channel feature you have to jumper the signal pins 4 and 5 of the RX, power the RX, wait 5 seconds and unpower it.

When everything went well, you will see the following string when powering the LRS RX with the OSD: 'MODE FLAGS 00'