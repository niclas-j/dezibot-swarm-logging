#ifndef COMMAND_SENDER_H
#define COMMAND_SENDER_H

#include <stdint.h>

bool sendCommandToDevice(const uint8_t *mac, uint8_t command);

#endif
