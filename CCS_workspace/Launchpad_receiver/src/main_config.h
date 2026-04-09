#pragma once

/* Main configuration file for the Launchpad receiver application. */

#include "Board.h"
#include "communication_settings.h"

extern PIN_Handle ledPinHandle;
extern PIN_State ledPinState;

extern int messages_count;
extern char received_message[MAX_PACKET_LENGTH + 1];

#ifndef MIN
	#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
