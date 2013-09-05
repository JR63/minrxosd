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


static uint8_t		tsrxstatus = TSRX_BOOT;
static uint8_t		version = TSRX_IDLE_OLDER;

static unsigned long	LastPacketTime = 0;
static uint8_t		PacketTimeout = 37;
static uint8_t		PacketsPerSecond = 30;
static uint8_t		PacketWindow[PACKET_WINDOW_MAX];

static int8_t		channel_cnt = -100;
static uint32_t		scan_value;


void tsrxtalk_init(void)
{
	int i;
	for (i = 0; i < PACKET_WINDOW_MAX; i++) {
		PacketWindow[i] = PACKED_GOOD;
	}
	for (i = 0; i < CHANNEL_MAX; i++) {
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
	LastPacketTime = millis();
}


uint8_t get_tsrx_version(void) {
	return version;
}


void scan_value_clear(void) {
	scan_value = 0;
}


void scan_value_add(char c) {
	scan_value = scan_value * 10 + c - '0';
}


void scan_value_set(void) {
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


int8_t scan_value_percent(void) {
	return (int8_t) ((1.0 - (float) BadPacketsDelta / (float) (GoodPacketsDelta + BadPacketsDelta)) * 100.0 + 0.5);
}


void packet_window_set(uint8_t good_bad, uint8_t cnt) {
	static uint8_t index = 0;
	int i;
	
	for (i = 0; i < cnt; i++) {
		PacketWindow[index++] = good_bad;
		index = index >= PacketsPerSecond ? 0 : index;
	}
}


int8_t packet_window_percent(void) {
	int i;
	uint8_t bads = 0;
	
	for (i = 0; i < PacketsPerSecond; i++) {
		if (PacketWindow[i] == PACKED_BAD) bads++;
	}
	
	return (int8_t) ((1.0 - (float) bads / (float) PacketsPerSecond) * 100.0 + 0.5);
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


uint16_t detect_frameduration(uint8_t c) {
	static int detect_cnt = 0;
	static uint16_t frameduration = 0;
	uint16_t ret = 0;
	
	// dumb string detect looking for string 'Rate: ' and the following value
	switch (detect_cnt) {
		case 0:
			if (c == 'R') detect_cnt++;
		break;
		case 1:
			if (c == 'a') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 2:
			if (c == 't') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 3:
			if (c == 'e') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 4:
			if (c == ':') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 5:
			if (c == ' ') detect_cnt++;
			else detect_cnt = 0;
		break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			frameduration = frameduration * 10 + c - '0';
			detect_cnt++;
		break;
		case 11:
			PacketTimeout = (uint8_t) ((frameduration * PACKET_TIMEOUT_FACTOR) / 1000.0);
			//osd.printf("|packet timeout: %2u", PacketTimeout);
			PacketsPerSecond = (uint8_t) (1000.0 / (frameduration / 1000.0));
			PacketsPerSecond = PacketsPerSecond > PACKET_WINDOW_MAX ? PACKET_WINDOW_MAX : PacketsPerSecond;
			//osd.printf("|packets per second: %2u", PacketsPerSecond);
			detect_cnt++;
		break;
		case 12:
			ret = frameduration;
		break;
	}
	return ret;
}


int tsrxtalk_parse(uint8_t c) {
	static uint16_t	new_chan_fails_val;

	switch (tsrxstatus) {
		case TSRX_BOOT:
			if (detect_str_eeprom(c) && !detect_str_contact(c)) {
				detect_frameduration(c);
				if (c == '\n' || c == '\r') {
					if (c == '\n') osd.write('|');
				} else {
					if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
					osd.write(c);
				}
			}
		break;
		case TSRX_VERSION_CHECK:
			if (detect_str_eeprom(c) && detect_str_contact(c)) {
				version = TSRX_IDLE_FROM_V25;
			}
			tsrxstatus = version;
			tsrxtalk_parse(c);
		break;
		case TSRX_IDLE_OLDER:
			int i;
			if (c < FIRST_CHANNEL)			// lower than known channels
				i = 0;
			else if (c > LAST_CHANNEL)		// higher than known channels
				i = CHANNEL_MAX - 1;
			else					// known channel
				i = c - FIRST_CHANNEL + 1;
			BadChannel++;
			BadChannelDelta++;
			BadChannelTime = millis();
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
				scan_value_set();
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
					if (channel_cnt < 0 || channel_cnt >= CHANNEL_MAX) channel_cnt = -100;
					tsrxstatus++;
				break;
				default:
					tsrxstatus = version;
			}
		break;
		case TSRX_VALUE_READ_1:					// hi byte
			new_chan_fails_val = c<<8;
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
		case TSRX_VALUE_READ_2:					// lo byte
			new_chan_fails_val += c;
			tsrxstatus++;
		break;
		case TSRX_VALUE_PLOT:					// plot marker
			LastPacketTime = millis();
			if (channel_cnt >= 0 && channel_cnt < CHANNEL_MAX) {
				ChannelCount++;
				if (ChannelFails[channel_cnt] != new_chan_fails_val) {
					uint16_t delta_channel_fails = new_chan_fails_val - ChannelFails[channel_cnt];
					packet_window_set(PACKED_BAD, delta_channel_fails);
					BadChannel += delta_channel_fails;
					BadChannelDelta += delta_channel_fails;
					BadChannelTime = millis();
					ChannelFails[channel_cnt] = new_chan_fails_val;
				} else {
					packet_window_set(PACKED_GOOD, 1);
				}
			}
			tsrxstatus = version;
		break;
		
		default:
			tsrxstatus = version;
	}
}


int tsrxtalk_read(void) {
	static uint8_t crlf_count = 0;
	
	// check version after boot time
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

	// simulate bad channel if no packet was received for PacketTimeout milli seconds
	if (tsrxstatus >= TSRX_IDLE_FROM_V25 && millis() > LastPacketTime + PacketTimeout) {
		LastPacketTime = millis();
		packet_window_set(PACKED_BAD, 1);
		BadChannel++;
		BadChannelDelta++;
		BadChannelTime = millis();
	}
		
	// clear BadChannelDelta after some time
	if (BadChannelDelta && millis() > BadChannelTime + CHANNEL_DELTA_DURATION) {
		BadChannelDelta = 0;
	}
	
	if (tsrxstatus < TSRX_IDLE_OLDER) {
		return 0;
	} else {
		return 1;
	}
}

#endif // PROTOCOL_TSRXTALK
