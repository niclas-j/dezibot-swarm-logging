#ifndef SENSOR_MESSAGE_H
#define SENSOR_MESSAGE_H

#include <stdint.h>

#define MSG_MAGIC 0xDE21

typedef struct {
    uint16_t magic;
    uint32_t counter;
    uint32_t uptimeMs;
    // ColorDetection
    float    ambientLight;
    uint16_t colorR;
    uint16_t colorG;
    uint16_t colorB;
    uint16_t colorW;
    // LightDetection
    uint16_t irFront;
    uint16_t irLeft;
    uint16_t irRight;
    uint16_t irBack;
    uint16_t dlBottom;
    uint16_t dlFront;
    // Motor
    uint16_t motorLeft;
    uint16_t motorRight;
    // IMU acceleration
    int16_t  accelX;
    int16_t  accelY;
    int16_t  accelZ;
    // IMU rotation
    int16_t  gyroX;
    int16_t  gyroY;
    int16_t  gyroZ;
    // IMU extras
    float    temperature;
    int8_t   whoAmI;
    int32_t  tiltX;
    int32_t  tiltY;
    uint8_t  tiltDirection;
    // System
    uint32_t freeHeap;
    uint32_t minFreeHeap;
    uint8_t  taskCount;
    float    chipTemp;
} __attribute__((packed)) SensorMessage;

#endif
