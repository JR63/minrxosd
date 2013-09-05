/**
 ******************************************************************************
 *
 * @file       minRXOSD.ino
 * @author     Joerg-D. Rothfuchs
 * @brief      OSD for RX data for RX from TSLRS
 *             framework based on the work of the developers of ArduCAM-OSD
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/> or write to the 
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ************************************************************ */
/* **************** MAIN PROGRAM - MODULES ******************** */
/* ************************************************************ */

#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 

#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];})) 


/* **********************************************/
/* ***************** INCLUDES *******************/

//#define membug 
//#define FORCEINIT  // You should never use this unless you know what you are doing 


// AVR Includes
#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Math.h>
#include <math.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "wiring.h"
#endif
#include <EEPROM.h>
#include <SimpleTimer.h>
#include <GCS_MAVLink.h>

#ifdef membug
#include <MemoryFree.h>
#endif

// Configurations
#include "OSD_Config.h"
#include "ArduCam_Max7456.h"
#include "OSD_Vars.h"

#include "PWMRead.h"
#include "TSRXTalk.h"


/* *************************************************/
/* ***************** DEFINITIONS *******************/

// OSD Hardware 
//#define ArduCAM328
#define MinimOSD

#define TELEMETRY_SPEED  9600  // How fast the data is coming to serial port
#define BOOTTIME         2000  // Time in milliseconds that we show boot loading bar and wait user input

// Objects and serial definitions
FastSerialPort0(Serial);
OSD osd;


/* ************************************************/
/* *************** help functions ****************/

void unplugSlaves(){
// Unplug list of SPI
#ifdef ArduCAM328
    digitalWrite(10,  HIGH);              // unplug USB HOST: ArduCam Only
#endif
    digitalWrite(MAX7456_SELECT,  HIGH);  // unplug OSD
}


/* **********************************************/
/* ***************** SETUP() ********************/

void setup() 
{
#ifdef ArduCAM328
    pinMode(10, OUTPUT);               // USB ArduCam only
#endif
    pinMode(MAX7456_SELECT,  OUTPUT);  // OSD CS

    Serial.begin(TELEMETRY_SPEED);

#ifdef membug
    Serial.println(freeMem());
#endif

    // Prepare OSD for displaying 
    unplugSlaves();
    osd.init();

// be quick to get the bootmessages from the RX
#if 0
    startPanels();
    delay(500);
#endif

    // OSD debug for development (Shown at start)
#ifdef membug
    osd.setPanel(1,1);
    osd.openPanel();
    osd.printf("%i",freeMem()); 
    osd.closePanel();
#endif

    // Just to easy up development things
#ifdef FORCEINIT
    InitializeOSD();
#endif

#if 0
    // Check EEPROM to see if we have initialized it already or not
    // also checks if we have new version that needs EEPROM reset
    if (readEEPROM(CHK1) + readEEPROM(CHK2) != VER) {
        osd.setPanel(6,9);
        osd.openPanel();
        osd.printf_P(PSTR("Missing/Old Config")); 
        osd.closePanel();
        InitializeOSD();
    }
#endif

// be quick to get the bootmessages from the RX
#if 0
    // Get correct panel settings from EEPROM
    readSettings();
    for (panel = 0; panel < npanels; panel++) readPanelSettings();
    panel = 0; //set panel to 0 to start in the first navigation screen
    // Show bootloader bar
    loadBar();
#endif

    pwm_read_init();
    
    osd.clear();
    panLogo();
    
    osd.setPanel(5, 0);
    osd.openPanel();

} // END of setup();


/* ***********************************************/
/* ***************** MAIN LOOP *******************/

void loop() 
{
    if (tsrxtalk_read()) {
	pwm_read();		// currently we have time for polling, later we will use int version
	writePanels();
    }
}
