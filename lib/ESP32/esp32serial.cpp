#include "esp32serial.h"

ESP32Serial::ESP32Serial(PinName rx, PinName tx): serial_connection(tx, rx) {
    this->rxPin = rx;
    this->txPin = tx;
}

void ESP32Serial::initialize(Callback<void(uint8_t)> on_received) {
    
}