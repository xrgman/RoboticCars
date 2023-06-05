#include "communicationDefinitions.h"

#define ESP32_BAUDRATE 115200
#define ESP32_ONBOARD_LED_PIN 2
#define STM32_RX_PIN 16
#define STM32_TX_PIN 17
#define BUF_SIZE 50

//Function definitions:
void handleSerial2Input(byte r);

//Main loop timer fields:
hw_timer_t *mainLoopTimer = NULL;
bool timerFlag = false;
int programCounter = 0;

void IRAM_ATTR onMainTimer(){
  timerFlag = true;

  programCounter++;
}

void setup() {
  Serial.begin(ESP32_BAUDRATE);
  Serial2.begin(ESP32_BAUDRATE);

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
    handleSerial2Input(Serial2.read());
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
ReceivingData receiving_data;

void sendByte(RelayOver sendOver, byte b) {
  switch(sendOver) {
    case SERIAL_WIRE:
      Serial.print((char) b);
      break;
    case BLUETOOTH:
      break;
    default:
      //TODO some error
      break;
  }
}

void sendMessage(RelayOver sendOver, MessageType type, uint8_t size, uint8_t* data) {
    uint8_t checkSum = 0;

    sendByte(sendOver, '?');
    checkSum ^= '?';

    //Sending message type:
    sendByte(sendOver, type);
    checkSum ^= type;

    //Sending message size:
    sendByte(sendOver, size);
    checkSum ^= size;
    
    //Sending data:
    for (int i = 0; i < size; i++) {
        sendByte(sendOver, data[i]);
        checkSum ^= data[i];
    }

  //TODO checksummetje
}

void processReceivedMessage(MessageType type, RelayOver relayOver, uint8_t data_size, uint8_t *data) {
    if(relayOver == BLUETOOTH) { //change to not equal none and just pass the relayover
      //Serial.println("Relaying message over bluetooth");
      sendMessage(SERIAL_WIRE, type, data_size, data);
      //Just pass it on to bluettoth
    }

}

void handleSerial2Input(byte r) {
    // Detecting start character:
    if (receiving_data.status == MessageStatus::IDLE && r == '?') {
        receiving_data.status = MessageStatus::RELAY;
    }
    //Retreiving relay over type:
    else if(receiving_data.status == MessageStatus::RELAY) {
        receiving_data.relayOver = (RelayOver) (r); 
        receiving_data.status = MessageStatus::TYPE;
    }
    //Retreiving the message type:
    else if(receiving_data.status == MessageStatus::TYPE) {
        receiving_data.type = (MessageType) (r); 
        receiving_data.status = MessageStatus::SIZE;
    }
    //Retreiving the data length:
    else if(receiving_data.status == MessageStatus::SIZE) {
        receiving_data.data_size = r; 
        receiving_data.status = MessageStatus::DATA;
    }
    //Retreiving the data:
    else if(receiving_data.status == MessageStatus::DATA) {
        if(receiving_data.idx >= receiving_data.data_size) {
            // Printing received message

            //Processing message:
            processReceivedMessage(receiving_data.type, receiving_data.relayOver, receiving_data.data_size, receiving_data.data);

            //Resetting buffer:
            receiving_data.status = MessageStatus::IDLE;
            receiving_data.idx = 0;

            return;
        }
        
        receiving_data.data[receiving_data.idx] = r;
        receiving_data.idx++;
    }
}
