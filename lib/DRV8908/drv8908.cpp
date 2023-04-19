#include "drv8908.h"
#include "drv8908registers.h"

DRV8908::DRV8908(PinName mosi, PinName miso, PinName sck, PinName nss, PinName sleep, PinName fault) 
    : spi(mosi, miso, sck), chip_select(nss), sleep_pin(sleep), fault_pin(fault) //, nss, use_gpio_ssel
{
    //Empty constructor :)
}

void DRV8908::Initialize()
{
    //Enable the chip, by setting the sleep pin to high:
    sleep_pin = 1;

    //Setting SPI settings:
    spi.format(DRV8908_SPI_BITS, DRV8908_SPI_MODE);
    spi.frequency(DRV8908_SPI_FREQ);

    chip_select = 1;

}

void DRV8908::PrintErrorStatus()
{
    spi.select();
    chip_select = 0;

    if(fault_pin.read()) {
        printf("Error detected\r\n");
    }
    else {
        printf("No errors detected\r\n");
    }

    printf("Status: ");
    printf("%x\n", readByte(DRV8908_IC_STAT));
    printf("Overcurrent: ");
    printf("%x, ", readByte(DRV8908_OCP_STAT_3));
    printf("%x, ", readByte(DRV8908_OCP_STAT_2));
    printf("%x\n", readByte(DRV8908_OCP_STAT_1));
    printf("Open Load: ");
    printf("%x, ", readByte(DRV8908_OLD_STAT_3));
    printf("%x, ", readByte(DRV8908_OLD_STAT_2));
    printf("%x\n", readByte(DRV8908_OLD_STAT_1));

    chip_select = 1;
    spi.deselect();
}

void DRV8908::writeByte(uint8_t address, uint8_t data)
{
    spi.select();
    spi.frequency(DRV8908_SPI_FREQ); //?
    spi.write(address);
    spi.write(data);
    spi.deselect();
}

uint8_t DRV8908::readByte(uint8_t address)
{
    spi.select();
    spi.frequency(DRV8908_SPI_FREQ);
    spi.write(address);
    uint8_t data = spi.write(0);
    spi.deselect();

    return data;
}

void writeByte(uint8_t subAddress, uint8_t data){   /* write data to device */
    // _spi->beginTransaction(SPISettings(SPI_HS_CLOCK, MSBFIRST, SPI_MODE3)); // begin the transaction
    // digitalWrite(_csPin,LOW); // select the MPU9250 chip
    // _spi->transfer(subAddress); // write the register address
    // _spi->transfer(data); // write the data
    // digitalWrite(_csPin,HIGH); // deselect the MPU9250 chip
    // _spi->endTransaction(); // end the transaction

    // spi.frequency(SPI_FREQ); // setup clock
    // //cc=0; // select the MPU9250 chip
    // spi.write(subAddress); // write the register address
    // spi.write(data); // write the data
    // //cc=1; // deselect the MPU9250 chip
}

uint16_t readByte(uint8_t subAddress){
    // _spi->beginTransaction(SPISettings(SPI_HS_CLOCK, MSBFIRST, SPI_MODE3));
    // digitalWrite(_csPin,LOW); // select the MPU9250 chip
    // _spi->transfer(subAddress | SPI_READ); // specify the starting register address
    // uint8_t data = _spi->transfer(0x00); // read the data
    // digitalWrite(_csPin,HIGH); // deselect the MPU9250 chip
    // _spi->endTransaction(); // end the transaction

//     spi.frequency(SPI_FREQ); // setup clock
//     cc=0; // select the MPU9250 chip
//    // spi.write(DRV89xx_REGISTER_READ | (subAddress << 8)); // use READ MASK
//     //spi.write(subAddress | DRV89xx_REGISTER_READ);
//     uint16_t data =spi.write(0);   // write any to get data
//     cc=1; // deselect the MPU9250 chip 
//     return data;
    return 0;
}

void init() {
    // Configure device
    writeByte(DRV8908_OLD_CTRL_1, 0b11111111); // Disable open load detection on channels 1-8
    writeByte(DRV8908_OLD_CTRL_2, 0b11001111); // Disable errors from open load detection, open load detect on channels 9-12
    writeByte(DRV8908_OLD_CTRL_3, 0b10000000); // set Overcurrent protection to the most forgiving setting
    writeByte(DRV8908_SR_CTRL_1, 0b11111111); // Set slew rate to 2.5us vrs default 0.6us on half bridges (1-8)
    writeByte(DRV8908_SR_CTRL_2, 0b00001111); // Set slew rate to 2.5us vrs default 0.6us on half bridges (9-12)
    writeByte(DRV8908_PWM_CTRL_1, 0xFF);
}

