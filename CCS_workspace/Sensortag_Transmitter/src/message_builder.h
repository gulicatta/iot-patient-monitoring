#pragma once
#include "main_config.h"
#include "smartrf_settings.h"
#include <stdbool.h>

#define MESSAGE_BUFFER_SIZE 128

extern char message_buffer[MESSAGE_BUFFER_SIZE];

/**
 * Sets the whole message buffer to zero. This function should be called before adding new data.
 */
void clear_message_buffer();

/**
 * Returns the current length of the message in the buffer
 */
unsigned int get_message_length();

/**
 * Opens a JSON object by appending "{" to the message buffer. Begin a new JSON message with this function and end it
 * with JSON_close().
 */
int JSON_open();

/**
 * Closes a JSON object by appending "}" to the message buffer
 */
int JSON_close();

/**
 * Appends a string key-value pair to the JSON message buffer
 */
int JSON_add_string(const char* key, const char* value);

/**
 * Appends a float key-value pair to the JSON message buffer
 */
int JSON_add_float(const char* key, float value);

/**
 * Appends a long key-value pair to the JSON message buffer
 */
int JSON_add_long(const char* key, long value);

/**
 * Appends a bool key-value pair to the JSON message buffer
 */
int JSON_add_bool(const char* key, bool value);

/**
 * Opens a nested JSON object with the given key. Close the object with JSON_object_close()
 */
int JSON_object_open(const char* key);

/**
 * Closes a nested JSON object
 */
int JSON_object_close();
