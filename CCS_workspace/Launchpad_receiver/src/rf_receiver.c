#include "rf_receiver.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

RF_Object rfObject;
RF_Handle rfHandle;

/* Receive dataQueue for RF Core to fill in data */
dataQueue_t dataQueue;
rfc_dataEntryGeneral_t* currentDataEntry;
uint8_t packetLength;
uint8_t* packetDataPointer;
uint8_t packet[MAX_PACKET_LENGTH + NUM_APPENDED_BYTES - 1]; /* The length byte is stored in a separate variable */

/* Buffer which contains all Data Entries for receiving data.
 * Pragmas are needed to make sure this buffer is 4 byte aligned (requirement from the RF Core) */
#if defined(__TI_COMPILER_VERSION__)
	#pragma DATA_ALIGN(rxDataEntryBuffer, 4);
uint8_t rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_PACKET_LENGTH, NUM_APPENDED_BYTES)];
#elif defined(__IAR_SYSTEMS_ICC__)
	#pragma data_alignment = 4
uint8_t rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_PACKET_LENGTH, NUM_APPENDED_BYTES)];
#elif defined(__GNUC__)
uint8_t rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_PACKET_LENGTH, NUM_APPENDED_BYTES)]
	__attribute__((aligned(4)));
#else
	#error This compiler is not supported.
#endif

/** Callback function for RF commands
 * Runs everytime an RF event occurs (e.g. packet receivee)
 */
void rf_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
	if (e & RF_EventRxEntryDone)
	{
		/* Toggle pin to indicate RX */
		PIN_setOutputValue(ledPinHandle, Board_PIN_LED2, !PIN_getOutputValue(Board_PIN_LED2));

		messages_count++;

		/* Get current unhandled data entry */
		currentDataEntry = RFQueue_getDataEntry();

		/* Handle the packet data, located at &currentDataEntry->data:
		 * - Length is the first byte with the current configuration
		 * - Data starts from the second byte */
		packetLength	  = *(uint8_t*)(&currentDataEntry->data);
		packetDataPointer = (uint8_t*)(&currentDataEntry->data + 1);
		size_t message_len = 0;
		if (packetLength > 2)
		{
			message_len = (size_t)packetLength - 2; /* remove 2-byte sequence number */
			if (message_len > (sizeof(received_message) - 1))
			{
				message_len = sizeof(received_message) - 1;
			}
			memcpy(received_message, packetDataPointer + 3, message_len);
		}
		received_message[message_len] = '\0';

		/* Copy the payload + the status byte to the packet variable */
		memcpy(packet, packetDataPointer, (packetLength + 1));

		RFQueue_nextEntry();
	}
}
