| Command | Firmware | Target |
|---------|----------|--------|
| pio run -e esp32s3_receiver -t upload | Receiver (debug server, WiFi) | Dezibot 4 (ESP32-S3) |
| pio run -e esp32s3_sender -t upload | Sender (Serial test) | Dezibot 4 (ESP32-S3) |
| pio run -e esp32_sender -t upload | Sender (Serial test) | Dezibot 3 (ESP32) |

For monitoring:
pio device monitor -e esp32s3_receiver
pio device monitor -e esp32s3_sender
pio device monitor -e esp32_sender