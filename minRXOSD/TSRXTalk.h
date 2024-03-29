/**
 ******************************************************************************
 *
 * @file       TSRXTalk.h
 * @author     Joerg-D. Rothfuchs
 * @brief      Implements a subset of the communication with the RX from TSLRS
 *
 *****************************************************************************/


#ifndef TSRXTALK_H_
#define TSRXTALK_H_

#include "OSD_Config.h"


// TODO:
//	OSD_Vars.h bereinigen		(bis auf PAL/NTSC Auswahl)
//	OSD_Config.h bereinigen		(bis auf PAL/NTSC Auswahl)


#define DEBUG_CHAN_ACTIVE		ChannelCount	// > 0 if debug channel feature on LRS RX is active


#define PACKET_TIMEOUT_FACTOR		1.2		// + 20% of frameduration
#define PACKET_WINDOW_MAX		50
#define	PACKED_GOOD			0
#define	PACKED_BAD			1


#define	CHANNEL_MAX			24
#define	CHANNEL_ERROR_SHOW_MAX		999
#define	CHANNEL_DIV			100
#define	CHANNEL_STATUS_ROWS		3
#define	CHANNEL_DELTA_DURATION		2000	// [ms]


// for older versions
#define FIRST_CHANNEL			'"'
#define LAST_CHANNEL			'7'


// for version from 2.5 up
#define	TOKEN_FAILSAVE			'F'
#define	TOKEN_GOOD			'G'
#define	TOKEN_BAD			'B'
#define	TOKEN_VALUE			'*'

#define	SUBTOKEN_FGB			':'
#define	SUBTOKEN_VALUE_ZERO		'T'
#define	SUBTOKEN_VALUE_DATA_1		'd'
#define	SUBTOKEN_VALUE_DATA_2		'D'
#define	SUBTOKEN_VALUE_PLOT_GRAPH	'I'


// TSRX parse states
typedef enum {
  TSRX_BOOT = 0,
  TSRX_VERSION_CHECK,
  TSRX_IDLE_OLDER,			// idle for older version
  TSRX_IDLE_FROM_V25,			// idle from version 2.5 up
  TSRX_FAILSAVE_START,			// waits for :
  TSRX_FAILSAVE_SCAN,			// read data
  TSRX_GOOD_START,			// waits for :
  TSRX_GOOD_SCAN,			// read data
  TSRX_BAD_START,			// waits for :
  TSRX_BAD_SCAN,			// read data
  TSRX_VALUE_START,			// waits for d or T
  TSRX_VALUE_READ_1,			// read data hi
  TSRX_VALUE_NEXT,			// waits for D
  TSRX_VALUE_READ_2,			// read data lo
  TSRX_VALUE_PLOT			// waits for I
} tsrxtalk_parse_state_t;


uint8_t get_tsrx_version(void);
uint8_t scan_value_percent(void);
uint8_t packet_window_percent(void);
int tsrxtalk_read(void);


#endif /* TSRXTALK_H_ */
