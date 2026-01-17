#include <Arduino.h>
#include <painlessMesh.h>

#define MESH_PREFIX     "DEZIBOT_MESH"
#define MESH_PASSWORD   "somethingSneaky"
#define MESH_PORT       5555
#define GROUP_NUMBER    5

Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg) {
    int separatorIndex = msg.indexOf('#');
    if (separatorIndex != -1) {
        String groupNumberStr = msg.substring(0, separatorIndex);
        uint32_t num = groupNumberStr.toInt();
        String restOfMsg = msg.substring(separatorIndex + 1);

        Serial.printf("Received from %u groupNumber=%u msg=%s\n", from, num, restOfMsg.c_str());

        if (num == GROUP_NUMBER) {
            Serial.printf(">> GROUP %d: %s\n", GROUP_NUMBER, restOfMsg.c_str());
        }
    } else {
        Serial.printf("Received from %u: %s\n", from, msg.c_str());
    }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n--- Empfaenger starting ---");

    mesh.setDebugMsgTypes(ERROR | STARTUP);
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    Serial.println("Mesh initialized, waiting for messages...");
}

void loop() {
    mesh.update();
}
