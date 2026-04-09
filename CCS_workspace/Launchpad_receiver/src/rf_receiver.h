#pragma once

#include "main_config.h"
#include "communication_settings.h"
#include <ti/drivers/rf/RF.h>
/* Driverlib Header files */
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)

#include "RFQueue.h"
#include "smartrf_settings.h"

/** Constant header size of a Generic Data Entry */
#define DATA_ENTRY_HEADER_SIZE 8
/** Max length byte the radio will accept */
/** NOTE: Only two data entries supported at the moment */
#define NUM_DATA_ENTRIES	   2
/** The Data Entries data field will contain:
 * 1 Header byte (RF_cmdPropRx.rxConf.bIncludeHdr = 0x1)
 * Up to MAX_PACKET_LENGTH payload bytes
 * 1 status byte (RF_cmdPropRx.r
 * xConf.bAppendStatus = 0x1) */
#define NUM_APPENDED_BYTES	   2

extern RF_Object rfObject;
extern RF_Handle rfHandle;

/* Receive dataQueue for RF Core to fill in data */
extern dataQueue_t dataQueue;
extern rfc_dataEntryGeneral_t* currentDataEntry;
extern uint8_t packetLength;
extern uint8_t* packetDataPointer;
extern uint8_t packet[MAX_PACKET_LENGTH + NUM_APPENDED_BYTES - 1]; /* The length byte is stored in a separate variable */

/* Buffer which contains all Data Entries for receiving data.
 * Pragmas are needed to make sure this buffer is 4 byte aligned (requirement from the RF Core) */
#if defined(__TI_COMPILER_VERSION__)
	#pragma DATA_ALIGN(rxDataEntryBuffer, 4);
extern uint8_t rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_PACKET_LENGTH, NUM_APPENDED_BYTES)];
#elif defined(__IAR_SYSTEMS_ICC__)
	#pragma data_alignment = 4
extern uint8_t rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_PACKET_LENGTH, NUM_APPENDED_BYTES)];
#elif defined(__GNUC__)
extern uint8_t rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_PACKET_LENGTH, NUM_APPENDED_BYTES)]
	__attribute__((aligned(4)));
#else
	#error This compiler is not supported.
#endif

void rf_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
