#include "drv8908.h"
#include "drv8908registers.h"
#include "util.h"

DRV8908::DRV8908(PinName mosi, PinName miso, PinName sck, PinName nss, PinName sleep, PinName fault) 
    : spi(mosi, miso, sck), chip_select(nss), sleep_pin(sleep), fault_pin(fault) //, nss, use_gpio_ssel
{
    //Empty constructor :)
}
//MAYBE USE LOGIC ANALYZER
void DRV8908::Initialize()
{
    //Enable the chip, by setting the sleep pin to high:
    //sleep_pin = 1;
    sleep_pin.write(1);

    //Deselect the device:
    chip_select.write(1);

    //Setting SPI settings:
    spi.format(DRV8908_SPI_BITS, DRV8908_SPI_MODE);
    spi.frequency(DRV8908_SPI_FREQ);

    //Disabling open load detection:
    WriteByte(DRV8908_OLD_CTRL_1, 0b11111110);
}

/// @brief Checks if the device id is 010 (DRV8908)
void DRV8908::CheckDeviceOperation() {
    //Clearing faults:
    WriteByte(DRV8908_CONFIG_CTRL, 0b00000001);

    //Reading status register:
    uint16_t data = ReadByte(DRV8908_CONFIG_CTRL);

    //printf("\nTry nr 1:\n");
    printf("\nReceived in hex: 0x%x\r\n", data);
    printf("Received in binary: ");
    Util::printAsBinary(data);

    data = ReadByte(DRV8908_OLD_CTRL_1);

    //printf("\nTry nr 1:\n");
    printf("\nReceived in hex 2: 0x%x\r\n", data);
    printf("Received in binary 2: ");
    Util::printAsBinary(data);

    //Pulling chip select low, selecting the device:
    //spi.select();
    //chip_select.write(0);

    //Setting the frequency again (not sure if required?)
    //spi.frequency(DRV8908_SPI_FREQ);

    //Waiting for chip to activate:
    //wait_us(100);

    // Writing read register 0x07 command:
    // spi.write(0b0100011100000000); //0x4700
    // uint16_t data = spi.write(0b0100011100000000);
    // uint16_t data = spi.write(0xFF);
}

void DRV8908::PrintSensorReadings() {

}

void DRV8908::Reset() {
    
}

void DRV8908::PrintErrorStatus()
{
    //spi.select();
    // chip_select = 0;

    if(fault_pin.read()) {
        printf("Error detected\r\n");
    }
    else {
        printf("No errors detected\r\n");
    }

    printf("Status: ");
    printf("%x\n", ReadByte(DRV8908_IC_STAT));
    printf("Overcurrent: ");
    printf("%x, ", ReadByte(DRV8908_OCP_STAT_3));
    printf("%x, ", ReadByte(DRV8908_OCP_STAT_2));
    printf("%x\n", ReadByte(DRV8908_OCP_STAT_1));
    printf("Open Load: ");
    printf("%x, ", ReadByte(DRV8908_OLD_STAT_3));
    printf("%x, ", ReadByte(DRV8908_OLD_STAT_2));
    printf("%x\n", ReadByte(DRV8908_OLD_STAT_1));

    // chip_select = 1;
    //spi.deselect();
}

void DRV8908::Test()
{
    

    // chip_select = 0;
    // wait_us(10);
    // spi.write(0b0100011100000000); //Read register 7 with status info :):)
    // wait_us(10);
    // chip_select = 1;

    //Select the drv8908 chip:
    chip_select.write(0);

    spi.frequency(DRV8908_SPI_FREQ);

    wait_us(10);

    //Writing read register 0x07 command:
    //spi.write(0b0100011100000000); //0x4700
    uint16_t data = spi.write(0b0100011100000000);
    //uint16_t data = spi.write(0xFF);
    
    wait_us(10);

    //Deselect the chip:
    chip_select.write(1);

    printf("Status register: %x\n", data );

    //Select the drv8908 chip:
    chip_select.write(0);

    spi.frequency(DRV8908_SPI_FREQ);

    wait_us(10);

    //Writing read register 0x07 command:
    spi.write(0b0100011100000000); //0x4700
    uint16_t data2 = spi.write(0x00);
    //uint16_t data = spi.write(0xFF);
    
    wait_us(10);

    //Deselect the chip:
    chip_select.write(1);

    printf("Status register try 2: %x\n", data2);
}

void DRV8908::WriteByte(uint8_t address, uint8_t data)
{
    uint16_t command = (address << 8) | data;

    chip_select.write(0);
    thread_sleep_for(10);

    spi.write(command);

    thread_sleep_for(10);
    chip_select.write(1);
}

uint16_t DRV8908::ReadByte(uint8_t address)
{
    //Creating read message command for specified address:
    uint16_t command = DRV8908_READ_ADDRESS | (address << 8);

    spi.lock();
    chip_select.write(0);
    thread_sleep_for(10);

    spi.frequency(DRV8908_SPI_FREQ);
    thread_sleep_for(10);

    spi.write(command);
    uint16_t data = spi.write(0x0000);

    thread_sleep_for(10);
    chip_select.write(1);
    spi.unlock();

    // thread_sleep_for(10);

    // chip_select.write(0);
    // thread_sleep_for(10);

    // uint16_t data = spi.write(0x0000);

    // thread_sleep_for(10);
    // chip_select.write(1);

    return data;
}

// void init() {
//     // Configure device
//     writeByte(DRV8908_OLD_CTRL_1, 0b11111111); // Disable open load detection on channels 1-8
//     writeByte(DRV8908_OLD_CTRL_2, 0b11001111); // Disable errors from open load detection, open load detect on channels 9-12
//     writeByte(DRV8908_OLD_CTRL_3, 0b10000000); // set Overcurrent protection to the most forgiving setting
//     writeByte(DRV8908_SR_CTRL_1, 0b11111111); // Set slew rate to 2.5us vrs default 0.6us on half bridges (1-8)
//     writeByte(DRV8908_SR_CTRL_2, 0b00001111); // Set slew rate to 2.5us vrs default 0.6us on half bridges (9-12)
//     writeByte(DRV8908_PWM_CTRL_1, 0xFF);
// }

