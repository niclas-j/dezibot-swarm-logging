#ifndef COMMAND_MESSAGE_H
#define COMMAND_MESSAGE_H

#include <stdint.h>

#define CMD_MAGIC 0xDE22

enum CommandType : uint8_t {
    CMD_LOCATE = 0x01,
};

typedef struct {
    uint16_t magic;
    uint8_t  command;
} __attribute__((packed)) CommandMessage;

#endif
