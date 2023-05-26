#include "drv8908.h"
#include "drv8910registers.h"
#include "util.h"

DRV8908::DRV8908(PinName mosi, PinName miso, PinName sck, PinName nss, PinName sleep, PinName fault) 
    : spi(mosi, miso, sck), chip_select(nss), sleep_pin(sleep), fault_pin(fault) //, nss, use_gpio_ssel
{
    //Empty constructor :)
}

//MAYBE USE LOGIC ANALYZER
void DRV8908::initialize()
{
    //Enable the chip, by setting the sleep pin to high:
    sleep_pin.write(1);

    //Deselect the device:
    chip_select.write(1);

    //Setting SPI settings:
    spi.format(DRV8908_SPI_BITS, DRV8908_SPI_MODE);
    spi.frequency(DRV8908_SPI_FREQ);

    //Configuring the chip:
    writeByte(OLD_CTRL_1, 0b11111111); //Disabling open load detection on channels 1-8.
    writeByte(OLD_CTRL_2, 0b11001111); //Disable open load detection on channels 9-12 and disable errors from open load detection.
    //writeByte(OLD_CTRL_3, 0b10000000); //Set overcurrent protection to most forgiving setting. (DISABLED FOR NOW)
    writeByte(SR_CTRL_1, 0b11111111); //Set slew rate to 2.5us vrs default 0.6us on half bridges (1-8)
    writeByte(SR_CTRL_1, 0b00001111); //Set slew rate to 2.5us vrs default 0.6us on half bridges (9-12)
    writeByte(PWM_FREQ_CTRL, 0b11111111); //Setting pwm channels to max speed.
}

//************************************
//******** Motor functions ***********
//************************************

void DRV8908::configureMotor(uint8_t id, HalfBridge half_bridg1, HalfBridge half_bridge2, PWMChannel pwm_channel, uint8_t reverse_delay) {
    motors[id] = DRV89xxMotor(half_bridg1, half_bridge2, pwm_channel, reverse_delay);
}

void DRV8908::setMotor(uint8_t id, uint8_t speed, Direction direction) {
    motors[id].set(config_cache, speed, direction);
}

/// @brief Disable a specific motor.
/// @param id The id of the motor that needs to be disabled.
void DRV8908::disableMotor(uint8_t id) {
    motors[id].disable(config_cache);
}

/// @brief Writing the changes made by setMotor and disable to the actual chip.
void DRV8908::writeChanges() {
    //printLocalConfigContent();

    for (int i = DRV89XX_UPDATE_START; i <= DRV89XX_UPDATE_END; i++) {
        writeByte(i, config_cache[i]);
    }
}

//************************************
//******** Debug functions ***********
//************************************

void DRV8908::printRegisterContents() {
    for (int i = DRV89XX_UPDATE_START; i <= DRV89XX_UPDATE_END; i++) {
        printf("Register 0x%x: ", i);
        Util::printAsBinary(readByte(i));
    }
}

void DRV8908::printLocalConfigContent() {
    for (int i = DRV89XX_UPDATE_START; i <= DRV89XX_UPDATE_END; i++) {
        printf("Register 0x%x: ", i);
        Util::printAsBinary(config_cache[i]);
    }
}

/// @brief Checks if the device id is 010 (DRV8908)
void DRV8908::checkDeviceOperation(Communication *communication_protocol) {
    //Reading register containing device id:
    uint8_t data = readByte(CONFIG_CTRL) & 0xFF;

    //Retreiving device id:
    DeviceId deviceId = (DeviceId) Util::getBitsFromData(data, 1, 3);

    if(deviceId == drv8908 || deviceId == drv8910) { //Apperently this chip identifies as a drv8910.....
        communication_protocol->sendDebugMessage("SUCCESS: DRV8908 found and functioning properly.\r\n");
        return;
    }

    communication_protocol->sendDebugMessage("ERROR: DRV8908 not found or not functioning properly.\r\n");
}

void DRV8908::printErrorStatus()
{
    if(fault_pin.read()) {
        printf("Error detected\r\n");
    }
    else {
        printf("No errors detected\r\n");
    }

    printf("Status: ");
    Util::printAsBinary(readByte(IC_STAT));
    printf("Overcurrent: ");
    printf("%x, ", readByte(OCP_STAT_3));
    printf("%x, ", readByte(OCP_STAT_2));
    printf("%x\n", readByte(OCP_STAT_1));
    printf("Open Load: ");
    printf("%x, ", readByte(OLD_STAT_1));
    printf("%x, ", readByte(OLD_STAT_2));
    printf("%x\n", readByte(OLD_STAT_3));
}

void DRV8908::test()
{
    //This works::
    //Maybe register addresses are that of drv8910????? -> YES
    printf("Read first time:");
    Util::printAsBinary(readByte(PWM_DUTY_CTRL_1) & 0xFF);

    writeByte(PWM_DUTY_CTRL_1, 10);

    //thread_sleep_for(10);

    printf("Read second time:");
    Util::printAsBinary(readByte(PWM_DUTY_CTRL_1) & 0xFF);

    printf("\n\n\n");
}

//************************************
//******** SPI helper functions ******
//************************************

void DRV8908::writeByte(uint8_t address, uint8_t data)
{
    uint16_t command = (address << 8) | data;

    chip_select.write(0);
   
    spi.write(command);

    chip_select.write(1);

    wait_ns(400);
}

uint16_t DRV8908::readByte(uint8_t address)
{
    //Creating read message command for specified address:
    uint16_t command = DRV8908_READ_ADDRESS | (address << 8);

    chip_select.write(0);

    uint16_t data = spi.write(command);

    chip_select.write(1);

    return data;
}
