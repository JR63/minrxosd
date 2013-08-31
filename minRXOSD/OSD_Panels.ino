/**
 ******************************************************************************
 *
 * @file       OSD_Panels.ino
 * @author     Joerg-D. Rothfuchs
 * @brief      The OSD panels
 *
 *****************************************************************************/


#include "OSD_Config.h"
#include "PWMRead.h"
#include "TSRXTalk.h"


#define	WARNING_DURATION			4500	// [ms]

#define WARN_FLASH_TIME				1000	// [ms]	time with which the warnings are flashing
#define WARN_MAX				2	//	number of implemented warnings


#define PAN_WARN_X				5
#define PAN_WARN_Y				1


#define PAN_CHAN_STAT_X_OLDER			3
#define PAN_CHAN_STAT_Y_OLDER			2


#define PAN_CHAN_STAT_X_FROM_V25		4
#define PAN_CHAN_STAT_Y_FROM_V25		2

#define PAN_RX_STAT_X_FROM_V25			22
#define PAN_RX_STAT_Y_FROM_V25			8


static int tsrx_data_view = 0;


/******* MAIN FUNCTIONS *******/


/******************************************************************/
// Panel  : writePanels
// Output : Write the panels
/******************************************************************/
void writePanels() {
	switchView();
	showView();
}


/******************************************************************/
// Panel  : switchView
// Needs  : nothing
// Output : check switch view RC channel
/******************************************************************/
void switchView(void) {
	static unsigned long switch_time = 0;
	static int switch_state = 0;
	int stick_position;
	
	stick_position = pwm_get();
	
	if (switch_state == 0 && PWM_CHECK(stick_position)) {
		switch_state = 1;
		switch_time = millis();
	}
	
	if (switch_state == 1 && !PWM_CHECK(stick_position)) {
		switch_state = 0;
		if (millis() - switch_time < 1000) {
			tsrx_data_view = (tsrx_data_view >= view_cnt) ? 0 : tsrx_data_view + 1;
			osd.clear();
		}
	}
}


/******************************************************************/
// Panel  : showView
// Needs  : nothing
// Output : show the views
/******************************************************************/
void showView(void) {
	if (version == TSRX_IDLE_OLDER) {
		switch (tsrx_data_view) {
			case 0:
				panChannelStatus(PAN_CHAN_STAT_X_OLDER, PAN_CHAN_STAT_Y_OLDER);
			break;
			case 1:
			break;
		}
	}
	
	if (version == TSRX_IDLE_FROM_V25) {
		switch (tsrx_data_view) {
			case 0:
				panChannelStatus(PAN_CHAN_STAT_X_FROM_V25, PAN_CHAN_STAT_Y_FROM_V25);
				panRxStatus(PAN_RX_STAT_X_FROM_V25, PAN_RX_STAT_Y_FROM_V25);
			break;
			case 1:
				panRxStatus(PAN_RX_STAT_X_FROM_V25, PAN_RX_STAT_Y_FROM_V25);
			break;
			case 2:
			break;
		}
	}
	
	panWarn(PAN_WARN_X, PAN_WARN_Y);
}


/******* PANELS *******/


/******************************************************************/
// Panel  : panBoot
// Needs  : X, Y locations
// Output : Booting up text and empty bar after that
/******************************************************************/
void panBoot(int first_col, int first_line) {
#if 0
	osd.setPanel(first_col, first_line);
	osd.openPanel();
	osd.printf_P(PSTR("booting up:\xed\xf2\xf2\xf2\xf2\xf2\xf2\xf2\xf3")); 
	osd.closePanel();
#endif
}


/******************************************************************/
// Panel  : panLogo
// Needs  : 
// Output : Startup OSD LOGO
/******************************************************************/
void panLogo() {
	osd.setPanel(5, 0);
	osd.openPanel();
	osd.printf_P(PSTR("minrxosd 0.1.0  beta"));
	osd.closePanel();
}


/******************************************************************/
// Panel  : panWarn
// Needs  : X, Y locations
// Output : Warnings if there are any
/******************************************************************/
void panWarn(int first_col, int first_line) {
    static char warning_string[20];
    static uint8_t last_warning_type = 1;
    static uint8_t warning_type = 0;
    static unsigned long warn_text_timer = 0;
    int cycle;

    if (millis() > warn_text_timer) {				// if the text or blank text has been shown for a while
        if (warning_type) {					// there was a warning, so we now blank it out for a while
            last_warning_type = warning_type;			// save the warning type for cycling
            warning_type = 0;
	    sprintf(warning_string, "                    ");	// blank the warning
	    warn_text_timer = millis() + WARN_FLASH_TIME / 2;	// set clear warning time
        } else {
            cycle = last_warning_type;				// start the warning checks cycle where we left it last time
            do {				                // cycle through the warning checks
                if (++cycle > WARN_MAX) cycle = 1;
                switch (cycle) {
			case 1:					// BAD PACKETS xxxxxx
				if (BadPacketsDelta != 0 || BadChannelDelta != 0) {
					if (BadChannelDelta != 0 && millis() > BadChannelTime + WARNING_DURATION) {
						BadChannelDelta = 0;
					}
					warning_type = cycle;
					if (BadChannelDelta > BadPacketsDelta)
						sprintf(warning_string, " bad packets %6i ", BadChannelDelta);
					else
						sprintf(warning_string, " bad packets %6i ", BadPacketsDelta);
				}
			break;
			case 2:					// FAILSAFE xxxx
				if (FailsafesDelta != 0) {
					warning_type = cycle;
					sprintf(warning_string, "    failsafe %4i   ", Failsafes);
				}
			break;
                }
            } while (!warning_type && cycle != last_warning_type);
	    if (warning_type) {					// if there is a warning
		warn_text_timer = millis() + WARN_FLASH_TIME;	// set show warning time
	    }
        }

	osd.setPanel(first_col, first_line);
	osd.openPanel();
	osd.printf("%s", warning_string);
	osd.closePanel();
    }
}


/******************************************************************/
// Panel  : panChannelStatus
// Needs  : X, Y locations
// Output : Shows the channel status
/******************************************************************/
void panChannelStatus(int first_col, int first_line) {
	static unsigned long last_bad_time = 0;
	static uint16_t last_bad_count = 0;
	uint16_t bad_count = 0;
	static uint8_t show = 0;
	int x, y;
	int div = 100;
	int digit;
	int sub;
	char c;
	
	
	if (version == TSRX_IDLE_OLDER) {
		bad_count = BadPackets;
	} else {
		bad_count = BadChannel;
	}
	if (last_bad_count != bad_count) {
		show = 1;
		last_bad_time = millis();
		last_bad_count = bad_count;
	}
	if (millis() - last_bad_time > WARNING_DURATION) {
		show = 0;
	}
	
	osd.setPanel(first_col, first_line);
	osd.openPanel();
    
	for (y = 0; y < CHANNEL_STATUS_ROWS; y++) {
		for (x = 0; x < NUM_CHANNELS; x++) {
			if (!show) {
				c = ' ';
			} else if (ChannelFails[x] > 999) {
				c = '*';
			} else {
				digit = (int)(ChannelFails[x] / div);
				if (digit > 9) {
					sub = (int)(digit / 10) * 10;
				} else {
					sub = 0;
				}
				digit -= sub;
				if (digit == 0 && ChannelFails[x] < div) {
					c = ' ';
				} else {
					c = digit + '0';
				}
			}
			osd.printf("%c", c);
		}
		osd.printf("|");
		div /= 10;
	}
    
	osd.closePanel();
}


/******************************************************************/
// Panel  : panRxStatus
// Needs  : X, Y locations
// Output : Shows the RX status
/******************************************************************/
void panRxStatus(int first_col, int first_line) {
	osd.setPanel(first_col, first_line);
	osd.openPanel();
	osd.printf("%6i%c|", Failsafes, 'f');
	if (ChannelCount) {
		osd.printf("%6i%c|", BadChannel, 'c');
		osd.printf("%6i%c|", ChannelCount, 'p');
	} else {
		osd.printf("%6i%c|", BadPackets, 'b');
		osd.printf("%6i%c|", GoodPackets, 'g');
	}
	osd.printf("%c%5.1f%c", 0xE1, (float)(GoodPacketsDelta) / (float)(GoodPacketsDelta + BadPacketsDelta) * 100.0, '%');
	osd.closePanel();
}
