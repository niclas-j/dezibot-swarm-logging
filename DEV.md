| Command | Firmware | Target |
|---------|----------|--------|
| pio run -e esp32s3_receiver -t upload --upload-port /dev/cu.usbmodem2101 | Receiver (debug server, WiFi) | Dezibot 4 (ESP32-S3) |
| pio run -e esp32s3_sender -t upload --upload-port /dev/cu.usbmodem101 | Sender (Serial test) | Dezibot 4 (ESP32-S3) |
| pio run -e esp32_sender -t upload | Sender (Serial test) | Dezibot 3 (ESP32) |

Monitoring:
pio device monitor -e esp32s3_receiver --port /dev/cu.usbmodem2101
pio device monitor -e esp32s3_sender --port /dev/cu.usbmodem101
pio device monitor -e esp32_sender

SPIFFS (filesystem upload):
pio run -t uploadfs -e esp32s3_receiver

Port mapping:
- /dev/cu.usbmodem2101 = receiver
- /dev/cu.usbmodem101 = sender
