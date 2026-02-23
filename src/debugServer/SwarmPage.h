#ifndef SWARMPAGE_H
#define SWARMPAGE_H

#include <ArduinoJson.h>
#include "PageProvider.h"
#include <map>

struct SenderInfo;

class SwarmPage : public PageProvider {
private:
    WebServer* serverPointer;
    void locateDevice();
    void forwardDevice();
    void stopDevice();
    void sendCommand(uint8_t cmd);
public:
    explicit SwarmPage(WebServer* server);
    void handler() override;
    void getSwarmData();
};

#endif
