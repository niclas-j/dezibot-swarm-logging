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
public:
    explicit SwarmPage(WebServer* server);
    void handler() override;
    void getSwarmData();
};

#endif
