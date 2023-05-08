#include "ak8963.h"
#include "ak8963registers.h"

AK8963::AK8963(PinName scl, PinName sda) : i2c(sda, scl) {

}

/// @brief Check if the device is connected, by reading it's WHO_AM_I register.
void AK8963::CheckDeviceOperation() {
    uint8_t device_id = ReadByte(AK8963_WHO_AM_I);

    //Checking if sensor is found by using its WHO_AM_I register:
    if(device_id == 0x48) {
        printf("SUCCESS: AK8963 found and functioning properly.\r\n");

        return;
    }

    printf("ERROR: AK8963 not found or not functioning properly.\r\n");
}

/// @brief Print all sensor data.
void AK8963::PrintSensorReadings() {
    MagnetometerData magnetometerData;

    uint8_t status = ReadMagnetometerData(&magnetometerData);

    printf("AK8963 sensor data: \r\n");
    printf("*** Status: %s\n\r", status ? "no overflow" : "overflow");
    printf("*** Magnetometer x: %f, y: %f, z: %f\r\n", magnetometerData.mx, magnetometerData.my, magnetometerData.mz);
}
   
/// @brief Initialize the AK8963 magnetometer.
/// @param magnetometer_scale measurement scale, either 14 or 16 bit.
/// @param magnetometer_mode measurement mode.
void AK8963::Initialize(Mscale magnetometer_scale, Mmode magnetometer_mode) {
    uint8_t rawData[3]; 

    //Saving configuration
    magnetometerScale = magnetometer_scale;
    magnetometerMode = magnetometer_mode;

    //Resetting all settings:
    Reset();

    //Power off the magnetometer:
    WriteByte(AK8963_CNTL1, Mmode::MOP_POWER_DOWN); //0x00 
    thread_sleep_for(0.01);

    //Setting Fuse ROM access mode:
    WriteByte(AK8963_CNTL1, Mmode::MOP_FUSE_ROM); //0x0F
    thread_sleep_for(0.01);

    //Reading sensitivty adjustment values:
    rawData[0] = ReadByte(AK8963_ASAX);
    rawData[1] = ReadByte(AK8963_ASAY);
    rawData[2] = ReadByte(AK8963_ASAZ);

    //Calculating calibration data, using the formula from the datasheet:
    magnetometer_sensitivity_adjustment[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f;  
    magnetometer_sensitivity_adjustment[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;
    magnetometer_sensitivity_adjustment[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f;

    //Power off the magnetometer:
    WriteByte(AK8963_CNTL1, Mmode::MOP_POWER_DOWN); //0x00 
    thread_sleep_for(0.01);

    // Configure the magnetometer for continuous read and highest resolution
    // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
    // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
    WriteByte(AK8963_CNTL1, magnetometer_scale << 4 | magnetometer_mode); // Set magnetometer data resolution and sample ODR
    thread_sleep_for(0.01);
}

/// @brief Reset all registers of the magnetometer.
void AK8963::Reset() {
    WriteByte(AK8963_CNTL2, 0b00000001); // 0x01
}

void AK8963::Calibrate() {
//     uint16_t ii = 0, sample_count = 0;
//     int32_t mag_bias[3] = {0, 0, 0}, mag_scale[3] = {0, 0, 0};
//     int16_t mag_max[3] = {-32767, -32767, -32767}, mag_min[3] = {32767, 32767, 32767}, mag_temp[3] = {0, 0, 0};
  
//     // shoot for ~fifteen seconds of mag data
//     if(Mmode == 0x02) 
//         sample_count = 128;  // at 8 Hz ODR, new mag data is available every 125 ms

//     if(Mmode == 0x06) 
//         sample_count = 1500;  // at 100 Hz ODR, new mag data is available every 10 ms
    
//    for(ii = 0; ii < sample_count; ii++) {
//         readMagData(mag_temp);  // Read the mag data  
        
//         for (int jj = 0; jj < 3; jj++) {
//             if(mag_temp[jj] > mag_max[jj]) 
//                 mag_max[jj] = mag_temp[jj];

//             if(mag_temp[jj] < mag_min[jj]) 
//                 mag_min[jj] = mag_temp[jj];
//         }
    
//         if(Mmode == 0x02) 
//             wait(0.135);  // at 8 Hz ODR, new mag data is available every 125 ms

//         if(Mmode == 0x06)
//             wait(0.012);  // at 100 Hz ODR, new mag data is available every 10 ms
//     }

//     // Get hard iron correction
//     mag_bias[0]  = (mag_max[0] + mag_min[0])/2;  // get average x mag bias in counts
//     mag_bias[1]  = (mag_max[1] + mag_min[1])/2;  // get average y mag bias in counts
//     mag_bias[2]  = (mag_max[2] + mag_min[2])/2;  // get average z mag bias in counts
    
//     dest1[0] = (float) mag_bias[0]*mRes*magCalibration[0];  // save mag biases in G for main program
//     dest1[1] = (float) mag_bias[1]*mRes*magCalibration[1];   
//     dest1[2] = (float) mag_bias[2]*mRes*magCalibration[2];  
       
//     // Get soft iron correction estimate
//     mag_scale[0]  = (mag_max[0] - mag_min[0])/2;  // get average x axis max chord length in counts
//     mag_scale[1]  = (mag_max[1] - mag_min[1])/2;  // get average y axis max chord length in counts
//     mag_scale[2]  = (mag_max[2] - mag_min[2])/2;  // get average z axis max chord length in counts

//     float avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
//     avg_rad /= 3.0;

//     dest2[0] = avg_rad/((float)mag_scale[0]);
//     dest2[1] = avg_rad/((float)mag_scale[1]);
//     dest2[2] = avg_rad/((float)mag_scale[2]);
}

//************************************
//******** I2C helper functions ******
//************************************

/// @brief Read one byte from the requested I2C register.
/// @param address Address of the register.
/// @return Data currently in the register.
char AK8963::ReadByte(uint8_t address) {
    char data[1];
    char data_write[1];
    data_write[0] = address;

    i2c.write(AK8963_ADDRESS, data_write, 1, true);

    i2c.read(AK8963_ADDRESS, data, 1, false);

    return data[0];
}

/// @brief Read multiple bytes from a specific I2C register.
/// @param address Address of the register.
/// @param count Number of bytes to read
/// @param dest Array to store resulting data in.
void AK8963::ReadBytes(uint8_t address, uint8_t count, uint8_t *dest) {
    char data[14];
    char data_write[1];
    data_write[0] = address;

    i2c.write(address, data_write, 1, 1); 

    i2c.read(address, data, count, 0); 

    for(int i = 0; i < count; i++) {
        dest[i] = data[i];
    }
}

/// @brief Write one byte to a specific I2C register.
/// @param address Address of the register.
/// @param data Data to write to the register.
void AK8963::WriteByte(uint8_t address, uint8_t data) {
    char data_write[2];
    data_write[0] = address;
    data_write[1] = data;

    i2c.write(AK8963_ADDRESS, data_write, 2, false);
}

//************************************
//******** Read sensor data **********
//************************************

/// @brief Method that can be used to check if new data is available.
/// @return whether new data is available and ready to be read.
bool AK8963::IsDataReady() {
    uint8_t status1 = ReadByte(AK8963_ST1);

    return status1 & 0x01;
}

/// @brief Read the magnetometer sensor data.
/// @param data Data object to store the data in.
/// @return status id, can be either of the following value: 0 (overflow) 1 (success)
bool AK8963::ReadMagnetometerData(MagnetometerData *data) {
    uint8_t rawData[2];

    //Reading measurement data x-axis:
    rawData[0] = ReadByte(AK8963_XOUT_L);
    rawData[1] = ReadByte(AK8963_XOUT_H);

    data->mx = (int16_t)(((int16_t)rawData[1] << 8) | rawData[0]);

    //Reading measurement data y-axis:
    rawData[0] = ReadByte(AK8963_YOUT_L);
    rawData[1] = ReadByte(AK8963_YOUT_H);

    data->my = (int16_t)(((int16_t)rawData[1] << 8) | rawData[0]);

    //Reading measurement data z-axis:
    rawData[0] = ReadByte(AK8963_ZOUT_L);
    rawData[1] = ReadByte(AK8963_ZOUT_H);

    data->mz = (int16_t)(((int16_t)rawData[1] << 8) | rawData[0]);

    //Applying sensitivity correction:
    data->mx *= magnetometer_sensitivity_adjustment[0];
    data->my *= magnetometer_sensitivity_adjustment[1];
    data->mz *= magnetometer_sensitivity_adjustment[2];

    //Checking if an overflow occured:
    uint8_t status2 = ReadByte(AK8963_ST2);

    if(status2 & 0x08) {
        return false; //Overflow occured.
    }

    return true; //New data successfully gathered.
}