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

//***** Motor HALL sensor pins *****
#define MOTOR_1_HALL_1 PF_3
#define MOTOR_1_HALL_2 PF_4
#define MOTOR_2_HALL_1 PF_8
#define MOTOR_2_HALL_2 PF_9
#define MOTOR_3_HALL_1 PF_11
#define MOTOR_3_HALL_2 PF_12
#define MOTOR_4_HALL_1 PB_0
#define MOTOR_4_HALL_2 PB_1

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

//***** ESP32 pins *****
#define ESP32_TX_PIN PE_0
#define ESP32_RX_PIN PE_1
#define ESP32_GPIO1_PIN PC_14
#define ESP32_GPIO2_PIN PC_15

//***** I2C pins *****
#define I2C_4_SDA PF_15
#define I2C_4_SCL PF_14

//***********************
//***** Peripherals *****
//***********************

//***** Respeaker 6 Mic array *****
#define RESPEAKER6MIC_BUTTON_PIN PD_7
#define RESPEAKER6MIC_AC101_AMP_EN_PIN PD_4
#define RESPEAKER6MIC_I2S_W PA_4 //Frame sync (FS, Grey)
#define RESPEAKER6MIC_I2S_SD PB_2 //Serial data ( , Purple) (SPEAKER)
#define RESPEAKER6MIC_I2S_CLK PC_10 //Serial clock (CLK, Brown) 
#define RESPEAKER6MIC_I2S_SEL PC_11 //Channel select (DOUT, Green) (MIC)
#define RESPEAKER6MIC_AC101_ADDRESS 0x1A << 1
#define RESPEAKER6MIC_AC108_ADDRESS_1 0x35 << 1
#define RESPEAKER6MIC_AC108_ADDRESS_2 0x3b << 1

//I2S on board has 4 pins:
// PCM FS - Framse Sync (WS)
// PCM CLK - PDM clock line (SCK)
// PCM DIN - Data in (probably from board to mics)
// PCM DOUT - Data out (probably from mics to board)
//I2S3 ports in cubemx


//Video has:
// WS - Word select (LRCLK)
// SCK - Serial clock (BCLK)
// SD - Serial data (DIN)

//***** *****