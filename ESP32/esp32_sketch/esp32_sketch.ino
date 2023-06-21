#include "communicationDefinitions.h"
#include "BluetoothSerial.h"



#define ESP32_BAUDRATE 115200
#define ESP32_ONBOARD_LED_PIN 2
#define ESP32_DEVICE_NAME "Robotic car - ESP32"
#define STM32_RX_PIN 16
#define STM32_TX_PIN 17
#define BUF_SIZE 50

//Function definitions:
void handleSerialInput(ReceivingData *receiving_data, byte r, void (*callback)(MessageType, RelayOver, uint8_t, uint8_t*));
void processReceivedMessage(MessageType type, RelayOver relayOver, uint8_t data_size, uint8_t *data);

//Serial processing:
ReceivingData receiving_data_stm32;
ReceivingData receiving_data_bluetooth;

//Bluetooth:
BluetoothSerial SerialBT;

//Main loop timer fields:
hw_timer_t *mainLoopTimer = NULL;
bool timerFlag = false;
int programCounter = 0;

void IRAM_ATTR onMainTimer(){
  timerFlag = true;

  programCounter++;
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    Serial.println("Client Connected");
  }
 
  if(event == ESP_SPP_CLOSE_EVT ){
    Serial.println("Client disconnected");
  }
}

void setup() {
  Serial.begin(ESP32_BAUDRATE);
  Serial2.begin(ESP32_BAUDRATE);
  SerialBT.begin(ESP32_DEVICE_NAME, false);
  SerialBT.setTimeout(5000);
  SerialBT.register_callback(callback);

  pinMode(ESP32_ONBOARD_LED_PIN, OUTPUT);

  //Starting main loop timer:
  mainLoopTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(mainLoopTimer, &onMainTimer, true);
  timerAlarmWrite(mainLoopTimer, 50000, true);
  timerAlarmEnable(mainLoopTimer);
}

void loop() {
  //Reading serial input from STM32:
  if(Serial2.available()) {
    processReceivedByte(&receiving_data_stm32, Serial2.read(), &processReceivedMessage);
  }

  //Reading serial from bluetooth:
  if(SerialBT.available()) {
    processReceivedByte(&receiving_data_bluetooth, SerialBT.read(), &processReceivedMessage);
  }

  //Every 50ms:
  if(timerFlag) {

    //Every second:
    if(programCounter % 20 == 0) {

      //Blink blue status led:
      digitalWrite(ESP32_ONBOARD_LED_PIN, !digitalRead(ESP32_ONBOARD_LED_PIN));
    }

    //Clearing timer flag:
    timerFlag = false;
  }
}

//Serial stuff:


void sendByte(RelayOver sendOver, uint8_t b) {
  switch(sendOver) {
    case SERIAL_WIRE:
      Serial.print((char) b);
      break;
    case SERIAL_WIRE_STM32:
      //Serial.print((char) b);
      Serial2.print((char) b);
      break;
    case BLUETOOTH:
      // Serial.print((char) b);
      SerialBT.print((char) b);
      break;
    default:
      //TODO some error
      break;
  }
}

void processReceivedMessage(MessageType type, RelayOver relayOver, uint8_t data_size, uint8_t *data) {
    //Passing message if needed:
    //Serial.println("Received something :)");

    if(relayOver != NONE) { 
      sendMessage(relayOver, NONE, type, data_size, data, &sendByte);

      return;
    }
    else {
      switch(type) {
        case DEBUG:
          for(int i = 0; i < data_size; i++) {
            Serial.print((char) data[i]);
          }
          break;
        case PING:
          sendMessage(SERIAL_WIRE_STM32, NONE, ACK, 0, NULL, &sendByte);
          break;
      }
    }

    //Process message:
   

}

