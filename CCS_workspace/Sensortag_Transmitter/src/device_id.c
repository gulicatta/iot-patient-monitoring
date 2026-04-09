#include "device_id.h"

#include <stdint.h>
#include <string.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ccfg_simple_struct.h)

#define EXTADDR_OFFSET 0x2F0
#define EXT_ADDR_LEN   8

static char device_id_hex[17]					  = {0};
static const uint8_t dummy_ext_addr[EXT_ADDR_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static char nibble_to_hex(uint8_t nibble)
{
	return (nibble < 10U) ? (char)('0' + nibble) : (char)('A' + (nibble - 10U));
}

static void load_device_id_hex(char* out)
{
	uint8_t ext_addr[EXT_ADDR_LEN] = {0};
	uint32_t mac0				   = __ccfg.CCFG_IEEE_MAC_0;
	uint32_t mac1				   = __ccfg.CCFG_IEEE_MAC_1;
	int i;

	memcpy(ext_addr, &mac0, sizeof(mac0));
	memcpy(ext_addr + sizeof(mac0), &mac1, sizeof(mac1));

	if (memcmp(ext_addr, dummy_ext_addr, EXT_ADDR_LEN) == 0)
	{
		memcpy(ext_addr, (const void*)(uintptr_t)(FCFG1_BASE + EXTADDR_OFFSET), EXT_ADDR_LEN);
	}

	for (i = 0; i < EXT_ADDR_LEN; i++)
	{
		uint8_t byte	 = ext_addr[(EXT_ADDR_LEN - 1) - i];
		out[i * 2]		 = nibble_to_hex((uint8_t)(byte >> 4));
		out[(i * 2) + 1] = nibble_to_hex((uint8_t)(byte & 0x0F));
	}

	out[EXT_ADDR_LEN * 2] = '\0';
}

bool initialize_device_id(void)
{
	load_device_id_hex(device_id_hex);
	return true;
}

const char* get_device_id_hex(void)
{
	return device_id_hex;
}
