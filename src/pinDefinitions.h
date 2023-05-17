#include "mbed.h"

//****** Ultrasonic sensor pins ******
#define ULTRASONIC_LEFT_TRIGGER_PIN PE_6 //P4, U3
#define ULTRASONIC_LEFT_ECHO_PIN PE_3 //P4, U3
#define ULTRASONIC_RIGHT_TRIGGER_PIN PB_15 //P5, U2
#define ULTRASONIC_RIGHT_ECHO_PIN PB_12 //P5, U2
#define ULTRASONIC_FRONT_TRIGGER_PIN PB_14 //P2, U1
#define ULTRASONIC_FRONT_ECHO_PIN PB_13 //P2, U1
#define ULTRASONIC_BACK_TRIGGER_PIN PE_5 //P3, U4
#define ULTRASONIC_BACK_ECHO_PIN PE_4 //P3, U4

//***** LED pins *****
#define LED_1_PIN PG_2
#define LED_2_PIN PG_4
#define LED_3_PIN PG_3
#define LED_4_PIN PG_5

//***** DRV8908 pins *****
#define DRV8908_MOSI_PIN PA_7
#define DRV8908_MISO_PIN PB_4
#define DRV8908_SCK_PIN PG_11 //Serial clock pin
#define DRV8908_NSS_PIN PG_10 //Chip select pin
#define DRV8908_SLEEP_PIN PG_13 
#define DRV8908_FAULT_PIN PG_12

//***** AK8963 pins *****
#define AK8963_SCL_PIN PF_1
#define AK8963_SDA_PIN PF_0

//***** MPU9250 pins *****
#define MPU9250_SCL_PIN PF_1
#define MPU9250_SDA_PIN PF_0
#define MPU9250_INT_PIN PF_2

//***** UART pins *****
#define SERIAL_TX_PIN USBTX
#define SERIAL_RX_PIN USBRX



//***** *****