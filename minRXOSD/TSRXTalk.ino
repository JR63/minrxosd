/**
 ******************************************************************************
 *
 * @file       TSRXTalk.ino
 * @author     Joerg-D. Rothfuchs
 * @brief      Implements a subset of the communication with the RX from TSLRS
 *
 *****************************************************************************/


#include "TSRXTalk.h"


#ifdef PROTOCOL_TSRXTALK


void tsrxtalk_init(void)
{
	int i;
	for (i = 0; i < NUM_CHANNELS; i++) {
		ChannelFails[i] = 0;
	}
	ChannelCount = 0;
	BadChannelTime = 0;
	BadChannel = 0;
	BadChannelDelta = 0;
	Failsafes = 0;
	FailsafesDelta = 0;
	BadPackets = 0;
	BadPacketsDelta = 0;
	GoodPackets = 0;
	GoodPacketsDelta = 0;
}


static int8_t	channel_cnt = -100;
static int16_t	scan_value;


void scan_value_clear(void) {
	scan_value = 0;
}


void scan_value_add(char c) {
	scan_value = scan_value * 10 + c - '0';
}


void scan_value_use(void) {
	switch (tsrxstatus) {
		case TSRX_FAILSAVE_SCAN:
			FailsafesDelta = scan_value - Failsafes;
			Failsafes = scan_value;
		break;
		case TSRX_GOOD_SCAN:
			GoodPacketsDelta = scan_value - GoodPackets;
			GoodPackets = scan_value;
		break;
		case TSRX_BAD_SCAN:
			BadPacketsDelta = scan_value - BadPackets;
			BadPackets = scan_value;
		break;
	}
}


int detect_str_eeprom(uint8_t c) {
	static int detect_cnt = 0;
	int ret = 0;
	
	// dumb string detect looking for string ' EEPROM' and one arbitrary char
	switch (detect_cnt) {
		case 0:
			if (c == ' ') detect_cnt++;
		break;
		case 1:
			if (c == 'E') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 2:
			if (c == 'E') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 3:
			if (c == 'P') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 4:
			if (c == 'R') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 5:
			if (c == 'O') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 6:
			if (c == 'M') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 7:
			ret = 1;
		break;
	}
	return ret;
}


int detect_str_contact(uint8_t c) {
	static int detect_cnt = 0;
	int ret = 0;
	
	// dumb string detect looking for string 'Contact' and one arbitrary char
	switch (detect_cnt) {
		case 0:
			if (c == 'C') detect_cnt++;
		break;
		case 1:
			if (c == 'o') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 2:
			if (c == 'n') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 3:
			if (c == 't') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 4:
			if (c == 'a') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 5:
			if (c == 'c') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 6:
			if (c == 't') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 7:
			ret = 1;
		break;
	}
	return ret;
}


int tsrxtalk_parse(uint8_t c) {
	static uint16_t	prev_chan_fails_val;

	switch (tsrxstatus) {
		case TSRX_BOOT:
			if (detect_str_eeprom(c) && !detect_str_contact(c)) {
				if (c == '\n' || c == '\r') {
					if (c == '\n') osd.write('|');
				} else {
					if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
					osd.write(c);
				}
			}
		break;
		case TSRX_VERSION_CHECK:
			if (detect_str_eeprom(c) && detect_str_contact(c)) version = TSRX_IDLE_FROM_V25;
			if (version == TSRX_IDLE_OLDER) view_cnt = 1;
			if (version == TSRX_IDLE_FROM_V25) view_cnt = 2;
			tsrxstatus = version;
			tsrxtalk_parse(c);
		break;
		case TSRX_IDLE_OLDER:
			int i;
	
			BadPackets++;
	
			if (c < FIRST_CHANNEL)			// lower than known channels
				i = 0;
			else if (c > LAST_CHANNEL)		// higher than known channels
				i = NUM_CHANNELS - 1;
			else					// known channel
				i = c - FIRST_CHANNEL + 1;
	
			ChannelFails[i]++;
		break;
		case TSRX_IDLE_FROM_V25:
			switch (c) {
				case TOKEN_FAILSAVE:
					tsrxstatus = TSRX_FAILSAVE_START;
				break;
				case TOKEN_GOOD:
					tsrxstatus = TSRX_GOOD_START;
				break;
				case TOKEN_BAD:
					tsrxstatus = TSRX_BAD_START;
				break;
				case TOKEN_VALUE:
					tsrxstatus = TSRX_VALUE_START;
				break;
			}
		break;
		case TSRX_FAILSAVE_START:
		case TSRX_GOOD_START:
		case TSRX_BAD_START:
			switch (c) {
				case SUBTOKEN_FGB:
					tsrxstatus++;
					scan_value_clear();
				break;
				default:
					tsrxstatus = version;
			}
		break;
		case TSRX_FAILSAVE_SCAN:
		case TSRX_GOOD_SCAN:
		case TSRX_BAD_SCAN:
			if (c >= '0' && c <= '9') {
				scan_value_add(c);
			} else {
				scan_value_use();
				tsrxstatus = version;
				tsrxtalk_parse(c);
			}
		break;
		case TSRX_VALUE_START:
			switch (c) {
				case SUBTOKEN_VALUE_ZERO:
					channel_cnt = -1;
					tsrxstatus = version;
				break;
				case SUBTOKEN_VALUE_DATA_1:
					channel_cnt++;
					if (channel_cnt < 0 || channel_cnt >= NUM_CHANNELS) channel_cnt = -100;
					tsrxstatus++;
				break;
				default:
					tsrxstatus = version;
			}
		break;
		case TSRX_VALUE_READ_1:
			if (channel_cnt >= 0 && channel_cnt < NUM_CHANNELS) {
				prev_chan_fails_val = ChannelFails[channel_cnt];
				ChannelFails[channel_cnt] = c<<8;		// hi byte
			}
			tsrxstatus++;
		break;
		case TSRX_VALUE_NEXT:
			switch (c) {
				case SUBTOKEN_VALUE_DATA_2:
					tsrxstatus++;
				break;
				default:
					tsrxstatus = version;
			}
		break;
		case TSRX_VALUE_READ_2:
			if (channel_cnt >= 0 && channel_cnt < NUM_CHANNELS) {
				ChannelFails[channel_cnt] += c;			// lo byte
				ChannelCount++;
				if (prev_chan_fails_val != ChannelFails[channel_cnt]) {
					BadChannel += ChannelFails[channel_cnt] - prev_chan_fails_val;
					BadChannelDelta += ChannelFails[channel_cnt] - prev_chan_fails_val;
					BadChannelTime = millis();
				}
			}
			tsrxstatus++;
		break;
		case TSRX_VALUE_PLOT:
			tsrxstatus = version;				// plot marker
		break;
		
		default:
			tsrxstatus = version;
	}
}


int tsrxtalk_read(void) {
	static uint8_t crlf_count = 0;
	
	if (tsrxstatus == TSRX_BOOT && millis() > 10000) {
		osd.closePanel();
		osd.clear();
		tsrxtalk_init();
		tsrxstatus = TSRX_VERSION_CHECK;
	}

	// grabbing data
	while (Serial.available() > 0) {
		uint8_t c = Serial.read();
		
		// needed for font upload, while no TSRXTalk is established
		if (tsrxstatus < TSRX_FAILSAVE_START && millis() < 20000 && millis() > 5000) {
			if (c == '\n' || c == '\r') {
				crlf_count++;
			} else {
				crlf_count = 0;
			}
			if (crlf_count == 3) {
				uploadFont();
			}
		}

		// parse data
		tsrxtalk_parse(c);
	}

        return 0;
}

#endif // PROTOCOL_TSRXTALK
