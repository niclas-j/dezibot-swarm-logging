#include "SenderMap.h"

static std::map<String, SenderInfo> emptySenderMap;
static SemaphoreHandle_t emptyMutex = xSemaphoreCreateMutex();

__attribute__((weak)) std::map<String, SenderInfo> &getSenderMap() { return emptySenderMap; }
__attribute__((weak)) SemaphoreHandle_t getSenderMapMutex() { return emptyMutex; }
