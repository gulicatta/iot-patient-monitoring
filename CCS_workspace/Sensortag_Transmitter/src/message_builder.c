#include "message_builder.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char message_buffer[MESSAGE_BUFFER_SIZE];
static unsigned int buffer_index = 0;

void clear_message_buffer()
{
	buffer_index = 0;
	memset(message_buffer, 0, MESSAGE_BUFFER_SIZE);
}

unsigned int get_message_length()
{
	return buffer_index;
}

static int append_to_buffer(const char* str)
{
	size_t len = strlen(str);
	if (buffer_index + len < MESSAGE_BUFFER_SIZE)
	{
		strcpy(message_buffer + buffer_index, str);
		buffer_index += len;
		return 0; // Success
	}
	return -1; // Buffer overflow
}

int JSON_open()
{
	return append_to_buffer("{");
}

int JSON_close()
{
	if (buffer_index > 0 && message_buffer[buffer_index - 1] == ',')
	{
		// overwrite trailing comma
		buffer_index--;
	}

	return append_to_buffer("}");
}

int JSON_add_string(const char* key, const char* value)
{
	char temp[64];
	snprintf(temp, sizeof(temp), "\"%s\":\"%s\",", key, value);
	return append_to_buffer(temp);
}

int JSON_add_float(const char* key, float value)
{
	char temp[64];
	/* Avoid %f as sprintf does not support floats on this platform */
	int negative   = (value < 0.0f) ? 1 : 0;
	float abs_val  = negative ? -value : value;
	long scaled	   = (long)(abs_val * 100.0f + 0.5f);
	long int_part  = scaled / 100;
	long frac_part = scaled % 100;

	if (negative)
	{
		snprintf(temp, sizeof(temp), "\"%s\":-%ld.%02ld,", key, int_part, frac_part);
	}
	else
	{
		snprintf(temp, sizeof(temp), "\"%s\":%ld.%02ld,", key, int_part, frac_part);
	}
	return append_to_buffer(temp);
}

int JSON_add_long(const char* key, long value)
{
	char temp[64];
	snprintf(temp, sizeof(temp), "\"%s\":%ld,", key, value);
	return append_to_buffer(temp);
}

int JSON_add_bool(const char* key, bool value)
{
	char temp[64];
	snprintf(temp, sizeof(temp), "\"%s\":%s,", key, value ? "true" : "false");
	return append_to_buffer(temp);
}

int JSON_object_open(const char* key)
{
	char temp[64];
	snprintf(temp, sizeof(temp), "\"%s\":{", key);
	return append_to_buffer(temp);
}

int JSON_object_close()
{
	if (buffer_index > 0 && message_buffer[buffer_index - 1] == ',')
	{
		// overwrite trailing comma
		buffer_index--;
	}

	return append_to_buffer("},");
}
