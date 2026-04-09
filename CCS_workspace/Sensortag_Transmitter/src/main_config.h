#pragma once

/* Main configuration file for the Sensortag Transmitter application. */

/***** Includes *****/

/* Board Header files */
#include "Board.h"
#include "communication_settings.h"
#include "smartrf_settings.h"

/***** Defines *****/

/* Packet TX Configuration */
#define PACKET_INTERVAL 500000 /* Set packet interval to 500000us or 500ms */
