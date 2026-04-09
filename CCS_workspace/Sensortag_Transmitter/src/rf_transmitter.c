#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <ti/drivers/rf/RF.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

/* Driverlib Header files */
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)

#include "rf_transmitter.h"
#include "message_builder.h"

static RF_Object rfObject;
static RF_Handle rfHandle;

static uint16_t seqNumber;

/* +1 to store the length byte when using variable-length packets */
static uint8_t packet[MAX_PACKET_LENGTH + 1];

void init_rf_transmitter()
{
	RF_Params rfParams;
	RF_Params_init(&rfParams);
	seqNumber = 0;

	RF_cmdPropTx.pktLen					  = MAX_PACKET_LENGTH;
	RF_cmdPropTx.pPkt					  = packet;
	RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;

	/* Request access to the radio */
#if defined(DeviceFamily_CC26X0R2)
	rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioSetup, &rfParams);
#else
	rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
#endif // DeviceFamily_CC26X0R2

	/* Set the frequency */
	RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);
}

static void RF_runCmd_error_handling(RF_EventMask terminationReason)
{
	switch (terminationReason)
	{
		case RF_EventLastCmdDone:
			// A stand-alone radio operation command or the last radio
			// operation command in a chain finished.
			break;
		case RF_EventCmdCancelled:
			// Command cancelled before it was started; it can be caused
			// by RF_cancelCmd() or RF_flushCmd().
			break;
		case RF_EventCmdAborted:
			// Abrupt command termination caused by RF_cancelCmd() or
			// RF_flushCmd().
			break;
		case RF_EventCmdStopped:
			// Graceful command termination caused by RF_cancelCmd() or
			// RF_flushCmd().
			break;
		default:
			// Uncaught error event
			while (1)
				;
	}
}

static void RF_command_status_error_handling(uint32_t cmdStatus)
{
	switch (cmdStatus)
	{
		case PROP_DONE_OK:
			// Packet transmitted successfully
			break;
		case PROP_DONE_STOPPED:
			// received CMD_STOP while transmitting packet and finished
			// transmitting packet
			break;
		case PROP_DONE_ABORT:
			// Received CMD_ABORT while transmitting packet
			break;
		case PROP_ERROR_PAR:
			// Observed illegal parameter
			break;
		case PROP_ERROR_NO_SETUP:
			// Command sent without setting up the radio in a supported
			// mode using CMD_PROP_RADIO_SETUP or CMD_RADIO_SETUP
			break;
		case PROP_ERROR_NO_FS:
			// Command sent without the synthesizer being programmed
			break;
		case PROP_ERROR_TXUNF:
			// TX underflow observed during operation
			break;
		default:
			// Uncaught error event - these could come from the
			// pool of states defined in rf_mailbox.h
			while (1)
				;
	}
}

void send_packet_rf(char* message)
{
	/* Build variable-length packet: [len][seq hi][seq lo][payload...] */
	unsigned int msg_len = get_message_length();
	unsigned int max_payload = MAX_PACKET_LENGTH;
	unsigned int payload_len = msg_len + 2; /* 2 bytes for seq number */
	if (payload_len > max_payload)
	{
		payload_len = max_payload;
	}

	/* Clear full packet to avoid stale bytes from previous transmissions. */
	memset(packet, 0, sizeof(packet));

	packet[0] = (uint8_t)payload_len; /* length byte excludes itself */

	packet[1] = (uint8_t)(seqNumber >> 8);
	packet[2] = (uint8_t)(seqNumber++);

	if (payload_len > 2)
	{
		unsigned int copy_len = payload_len - 2;
		memcpy(packet + 3, message, copy_len);
	}

	/* Send packet */
	RF_EventMask terminationReason = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal, NULL, 0);
	RF_runCmd_error_handling(terminationReason);

	uint32_t cmdStatus = ((volatile RF_Op*)&RF_cmdPropTx)->status;
	RF_command_status_error_handling(cmdStatus);

	/* Power down the radio */
	RF_yield(rfHandle);
}
