#include "CommandSender.h"

__attribute__((weak)) bool sendCommandToDevice(const uint8_t *mac, uint8_t command) {
    return false;
}
