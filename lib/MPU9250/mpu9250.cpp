#include "mpu9250.h"
#include "mpu9250registers.h"

MPU9250::MPU9250(PinName scl, PinName sda, PinName interrupt)
    : i2c(sda, scl) {
    //Do something with the interrupt :)

    i2c.frequency(400000);
}

void MPU9250::checkDeviceOperation(Communication *communication_protocol) {
    uint8_t device_id = ReadByte(MPU9250_WHO_AM_I);

    printf("Device id: %x\n", device_id);

    //Checking if sensor is found by using its WHO_AM_I register:
    if(device_id == 0x71 || device_id == 0x73) {
        communication_protocol->sendDebugMessage("SUCCESS: MPU9250 found and functioning properly.\r\n");

        return;
    }

    communication_protocol->sendDebugMessage("ERROR: MPU9250 not found or not functioning properly.\r\n");
}

void MPU9250::printSensorReadings() {
    AccelerometerData accelerometerData = ReadAccelerometerData();
    GyroscopeData gyroscopeData = ReadGyroscopeData();

    float temperature = ReadTemperatureData();

    printf("MPU9250 sensor data: \r\n");
    printf("*** Accelerometer x: %f, y: %f, z: %f\r\n", accelerometerData.ax, accelerometerData.ay, accelerometerData.az);
    printf("*** Gyroscope x: %f, y: %f, z: %f\r\n", gyroscopeData.gx, gyroscopeData.gy, gyroscopeData.gz);
    printf("*** Temperature: %f\r\n", temperature);
}

//************************************
//******** I2C helper functions ******
//************************************

/// @brief Read one byte from the requested I2C register.
/// @param address Address of the register.
/// @return Data currently in the register.
char MPU9250::ReadByte(uint8_t address) {
    char data[1];
    char data_write[1];
    data_write[0] = address;

    i2c.write(MPU9250_ADDRESS, data_write, 1, true);

    i2c.read(MPU9250_ADDRESS, data, 1, false);

    return data[0];
}

/// @brief Read multiple bytes from a specific I2C register.
/// @param address Address of the register.
/// @param count Number of bytes to read
/// @param dest Array to store resulting data in.
void MPU9250::ReadBytes(uint8_t address, uint8_t count, uint8_t *dest) {
    char data[14];
    char data_write[1];
    data_write[0] = address;

    i2c.write(MPU9250_ADDRESS, data_write, 1, 1); 

    i2c.read(MPU9250_ADDRESS, data, count, 0); 

    for(int i = 0; i < count; i++) {
        dest[i] = data[i];
    }
}

/// @brief Write one byte to a specific I2C register.
/// @param address Address of the register.
/// @param data Data to write to the register.
void MPU9250::WriteByte(uint8_t address, uint8_t data) {
    char data_write[2];
    data_write[0] = address;
    data_write[1] = data;

    i2c.write(MPU9250_ADDRESS, data_write, 2, false);
}

/// @brief Initialize the MPU9250 sensor.
/// @param gyroscope_scale Gyroscope scale that needs to be configured.
/// @param accelerometer_scale Accelerometer scale that needs to be configured.
void MPU9250::initialize(Gscale gyroscope_scale, Ascale accelerometer_scale) {
    //Reset all registers to default state:
    reset();

    //Performing self-test:
    //SelfTest();

    //Waking up the device, by clearing the sleep bit (6):
    WriteByte(MPU9250_PWR_MGMT_1, 0b00000000); //0x00
    thread_sleep_for(0.1);

    //Setting clock source to be PPL:
    WriteByte(MPU9250_PWR_MGMT_1, 0b00000001); //0x01

    //Disabling FSYNC and setting accelerometer bandwith to 44Hz and gyroscope bandwith to 42Hz:
    WriteByte(MPU9250_ACCEL_CONFIG, 0b00000011); //0x03

    //Setting sample rate of the gyroscope to 200Hz:
    WriteByte(MPU9250_SMPLRT_DIV, 0b00000100); //0x04

    //Configuring gyroscope range to full scale:
    uint8_t gyro_config =  ReadByte(MPU9250_GYRO_CONFIG);
    gyro_config &= ~0b11100000; //Clear self-test bits [7:5] (0xE0)
    gyro_config &= ~0b00011000; // Clear AFS bits [4:3] (0x18)
    gyro_config |= gyroscope_scale << 3; // Set full scale range gyroscope in AFS bits [4:3]

    WriteByte(MPU9250_GYRO_CONFIG, gyro_config);

    //Configuring accelerometer range to full scale:
    uint8_t accelerometer_config = ReadByte(MPU9250_ACCEL_CONFIG);
    accelerometer_config &= ~0b11100000; //Clear self-test bits [7:5] (0xE0)
    accelerometer_config &= ~0b00011000; // Clear AFS bits [4:3] (0x18)
    accelerometer_config |= accelerometer_scale << 3; // Set full scale range accelerometer in AFS bits [4:3]

    WriteByte(MPU9250_ACCEL_CONFIG, accelerometer_config);

    //Configuring accelerometer sample rate:
    uint8_t accelerometer_config2 = ReadByte(MPU9250_ACCEL_CONFIG_2);
    accelerometer_config2 &= ~0b00001111; //Clearing accel_fchoice_b (bit 3) and A_DLPFG [2:0] (0x0F)
    accelerometer_config2 |= 0b00000011; //Set accelerometer  rate to 1kHz and bandwith to 41Hz (0x03)

    WriteByte(MPU9250_ACCEL_CONFIG_2, accelerometer_config2);

    // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates, but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

    //Configuring interrupts:
    WriteByte(MPU9250_INT_PIN_CFG, 0b00100010); //0x22
    WriteByte(MPU9250_INT_ENABLE, 0b00000001);  //0x01
}

/// @brief Reset all settings of the MPU9250 back to factory defaults.
void MPU9250::reset() {
    WriteByte(MPU9250_PWR_MGMT_1, 0b10000000); //0x80
}

void MPU9250::calibrate() {
    uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
    uint16_t packet_count, fifo_count;
    int32_t gyroscope_bias[3] = {0, 0, 0}, acceleration_bias[3] = {0, 0, 0};

    //Resetting the device:
    reset();

    //Setting clock source to be PPL:
    WriteByte(MPU9250_PWR_MGMT_1, 0b00000001); //0x01
    WriteByte(MPU9250_PWR_MGMT_2, 0b00000000); //0x00
    thread_sleep_for(0.2);

    //Configuring device:
    WriteByte(MPU9250_INT_ENABLE, 0b00000000);   // Disable all interrupts (0x00)
    WriteByte(MPU9250_FIFO_EN, 0b00000000);      // Disable FIFO (0x00)
    WriteByte(MPU9250_PWR_MGMT_1, 0b00000000);   // Turn on internal clock source (0x00)
    WriteByte(MPU9250_I2C_MST_CTRL, 0b00000000); // Disable I2C master (0x00)
    WriteByte(MPU9250_USER_CTRL, 0b00000000);    // Disable FIFO and I2C master modes (0x00)
    WriteByte(MPU9250_USER_CTRL, 0b00001100);    // Reset FIFO and DMP (0x0C)
    thread_sleep_for(0.015);

    //Configure accelerometer and gyroscope:
    WriteByte(MPU9250_CONFIG, 0b00000001);      // Set low-pass filter to 188 Hz (0x01)
    WriteByte(MPU9250_SMPLRT_DIV, 0b00000000);  // Set sample rate to 1 kHz (0x00)
    WriteByte(MPU9250_GYRO_CONFIG, 0b00000000);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity (0x00)
    WriteByte(MPU9250_ACCEL_CONFIG, 0b00000000); // Set accelerometer full-scale to 2g, maximum sensitivity (0x00)

    uint16_t  accelerometer_sensitivity = 16384;  // = 16384 LSB/g
    uint16_t  gyroscope_sensitivity  = 131;   // = 131 LSB/degrees/sec
    
    // Configure FIFO to capture accelerometer and gyro data for bias calculation
    WriteByte(MPU9250_USER_CTRL, 0b00100000);   // Enable FIFO (0x40)
    WriteByte(MPU9250_FIFO_EN, 0b01111000);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250) (0x78)
    thread_sleep_for(0.04); // accumulate 40 samples in 80 milliseconds = 480 bytes

    // At end of sample accumulation, turn off FIFO sensor read
    WriteByte(MPU9250_FIFO_EN, 0b00000000); // Disable gyro and accelerometer sensors for FIFO (0x00)

    //Read FIFO sample count:
    data[0] = ReadByte(MPU9250_FIFO_COUNTH);
    data[1] = ReadByte(MPU9250_FIFO_COUNTL);

    fifo_count = ((uint16_t)data[0] << 8) | data[1];
    packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

    for (int i = 0; i < packet_count; i++) {
        //int16_t accelerometer_temp[3] = {0, 0, 0}, gyroscope_temp[3] = {0, 0, 0};

        //Read data for averaging and sum 
        ReadBytes(MPU9250_FIFO_R_W, 12, &data[0]); 

        acceleration_bias[0] += (int32_t) ((int16_t) (((int16_t)data[0] << 8) | data[1])); 
        acceleration_bias[1] += (int32_t) ((int16_t) (((int16_t)data[2] << 8) | data[3]));
        acceleration_bias[2] += (int32_t) ((int16_t) (((int16_t)data[4] << 8) | data[5]));
        gyroscope_bias[0]  += (int32_t) ((int16_t) (((int16_t)data[6] << 8) | data[7]));
        gyroscope_bias[1]  += (int32_t) ((int16_t) (((int16_t)data[8] << 8) | data[9]));
        gyroscope_bias[2]  += (int32_t) ((int16_t) (((int16_t)data[10] << 8) | data[11]));
    }

    //Calculating average bias:
    acceleration_bias[0] /= (int32_t) packet_count; 
    acceleration_bias[1] /= (int32_t) packet_count;
    acceleration_bias[2] /= (int32_t) packet_count;
    gyroscope_bias[0]  /= (int32_t) packet_count;
    gyroscope_bias[1]  /= (int32_t) packet_count;
    gyroscope_bias[2]  /= (int32_t) packet_count;

    if(acceleration_bias[2] > 0L) {
        acceleration_bias[2] -= (int32_t) accelerometer_sensitivity;   // Remove gravity from the z-axis accelerometer bias calculation
    } else {
        acceleration_bias[2] += (int32_t) accelerometer_sensitivity;
    }

    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] = (-gyroscope_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyroscope_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyroscope_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyroscope_bias[1]/4)       & 0xFF;
    data[4] = (-gyroscope_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyroscope_bias[2]/4)       & 0xFF;

    /// Push gyro biases to hardware registers
    /*  writeByte(MPU9250_ADDRESS, XG_OFFSET_H, data[0]);
      writeByte(MPU9250_ADDRESS, XG_OFFSET_L, data[1]);
      writeByte(MPU9250_ADDRESS, YG_OFFSET_H, data[2]);
      writeByte(MPU9250_ADDRESS, YG_OFFSET_L, data[3]);
      writeByte(MPU9250_ADDRESS, ZG_OFFSET_H, data[4]);
      writeByte(MPU9250_ADDRESS, ZG_OFFSET_L, data[5]);
    */

    //Construct gyro bias in deg/s for later manual subtraction
    gyroscope_bias[0] = (float) gyroscope_bias[0]/(float) gyroscope_sensitivity; 
    gyroscope_bias[1] = (float) gyroscope_bias[1]/(float) gyroscope_sensitivity;
    gyroscope_bias[2] = (float) gyroscope_bias[2]/(float) gyroscope_sensitivity;

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accelerometer_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases

    data[0] = ReadByte(MPU9250_XA_OFFSET_H);
    data[1] = ReadByte(MPU9250_XA_OFFSET_L);

    accelerometer_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];

    data[0] = ReadByte(MPU9250_YA_OFFSET_H);
    data[1] = ReadByte(MPU9250_YA_OFFSET_L);
  
    accelerometer_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];

    data[0] = ReadByte(MPU9250_ZA_OFFSET_H);
    data[1] = ReadByte(MPU9250_ZA_OFFSET_L);

    accelerometer_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];

    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

    for(int i = 0; i < 3; i++) {
        if(accelerometer_bias_reg[i] & mask) {
            mask_bit[i] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
        }
    }

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accelerometer_bias_reg[0] -= (accelerometer_bias_reg[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accelerometer_bias_reg[1] -= (accelerometer_bias_reg[1]/8);
    accelerometer_bias_reg[2] -= (accelerometer_bias_reg[2]/8);

    data[0] = (accelerometer_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accelerometer_bias_reg[0])      & 0xFF;
    data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] = (accelerometer_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accelerometer_bias_reg[1])      & 0xFF;
    data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] = (accelerometer_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accelerometer_bias_reg[2])      & 0xFF;
    data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

// Apparently this is not working for the acceleration biases in the MPU-9250
// Are we handling the temperature correction bit properly?
// Push accelerometer biases to hardware registers
    /*  writeByte(MPU9250_ADDRESS, XA_OFFSET_H, data[0]);
      writeByte(MPU9250_ADDRESS, XA_OFFSET_L, data[1]);
      writeByte(MPU9250_ADDRESS, YA_OFFSET_H, data[2]);
      writeByte(MPU9250_ADDRESS, YA_OFFSET_L, data[3]);
      writeByte(MPU9250_ADDRESS, ZA_OFFSET_H, data[4]);
      writeByte(MPU9250_ADDRESS, ZA_OFFSET_L, data[5]);
    */
    // Output scaled accelerometer biases for manual subtraction in the main program
    acceleration_bias[0] = (float)acceleration_bias[0]/(float)accelerometer_sensitivity;
    acceleration_bias[1] = (float)acceleration_bias[1]/(float)accelerometer_sensitivity;
    acceleration_bias[2] = (float)acceleration_bias[2]/(float)accelerometer_sensitivity;
}

void MPU9250::SelfTest() {
    //float destination[6] = {0,0,0,0,0,0};
    AccelerometerData accelerometer_data;
    GyroscopeData gyroscope_data;

    int16_t accelerometer_average[3], gyroscope_average[3], accelerometer_selfTest_average[3], gyroscope_selfTest_average[3];
    uint8_t selfTest[6]; 
    float factoryTrim[6];
    float selfTestResult[6];
    uint8_t FS = 0;

    //Configuring before starting the test:
    WriteByte(MPU9250_SMPLRT_DIV, 0b00000000); // Set gyro sample rate to 1 kHz (0x00)
    WriteByte(MPU9250_CONFIG, 0b00000010); // Set gyro sample rate to 1 kHz and DLPF to 92 Hz (0x02)
    WriteByte(MPU9250_GYRO_CONFIG, 1<<FS); // Set full scale range for the gyro to 250 dps
    WriteByte(MPU9250_ACCEL_CONFIG, 1<<FS); // Set full scale range for the accelerometer to 2 g
    WriteByte(MPU9250_ACCEL_CONFIG_2, 0b00000010); // Set accelerometer rate to 1 kHz and bandwidth to 92 Hz (0x02)
    
    //Get the average current values of the gyroscope and accelerometer:
    for(int i = 0; i < 200; i++) {
        accelerometer_data = ReadAccelerometerData();

        accelerometer_average[0] += accelerometer_data.ax;
        accelerometer_average[1] += accelerometer_data.ay;
        accelerometer_average[2] += accelerometer_data.az;

        gyroscope_data = ReadGyroscopeData();

        gyroscope_average[0] += gyroscope_data.gx;
        gyroscope_average[1] += gyroscope_data.gy;
        gyroscope_average[2] += gyroscope_data.gz;
    }

    //Averaging readings:
    for (int i =0; i < 3; i++) { 
        accelerometer_average[i] /= 200;
        gyroscope_average[i] /= 200;        
    }

    //Configure the accelerometer and gyroscope for self-test:
    WriteByte(MPU9250_ACCEL_CONFIG, 0b11100000); // Enable self test on all three axes and set accelerometer range to +/- 2 g (0xE0)
    WriteByte(MPU9250_GYRO_CONFIG, 0b11100000); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s (0xE0)
    //delay(25); // Delay a while to let the device stabilize

    //Retreiving average self test values:
    for(int i = 0; i < 200; i++) {
        accelerometer_data = ReadAccelerometerData();

        accelerometer_selfTest_average[0] = accelerometer_data.ax;
        accelerometer_selfTest_average[1] = accelerometer_data.ay;
        accelerometer_selfTest_average[2] = accelerometer_data.az;

        gyroscope_data = ReadGyroscopeData();

        gyroscope_selfTest_average[0] = gyroscope_data.gx;
        gyroscope_selfTest_average[1] = gyroscope_data.gy;
        gyroscope_selfTest_average[2] = gyroscope_data.gz;
    }

    //Averaging self-test readings:
    for (int i =0; i < 3; i++) { 
        accelerometer_selfTest_average[i] /= 200;
        gyroscope_selfTest_average[i] /= 200;
    }

    //Configure the accelerometer and gyroscope for normal operation:
    WriteByte(MPU9250_ACCEL_CONFIG, 0b00000000); //0x00
    WriteByte(MPU9250_GYRO_CONFIG, 0b00000000); //0x00
    //delay(25); // Delay a while to let the device stabilize

    // Retrieve accelerometer and gyroscope factory Self-Test Code from USR_Reg
    selfTest[0] = ReadByte(MPU9250_SELF_TEST_X_ACCEL); // X-axis accel self-test results
    selfTest[1] = ReadByte(MPU9250_SELF_TEST_Y_ACCEL); // Y-axis accel self-test results
    selfTest[2] = ReadByte(MPU9250_SELF_TEST_Z_ACCEL); // Z-axis accel self-test results
    selfTest[3] = ReadByte(MPU9250_SELF_TEST_X_GYRO); // X-axis gyro self-test results
    selfTest[4] = ReadByte(MPU9250_SELF_TEST_Y_GYRO); // Y-axis gyro self-test results
    selfTest[5] = ReadByte(MPU9250_SELF_TEST_Z_GYRO); // Z-axis gyro self-test results

    // Retrieve factory self-test value from self-test code reads
    factoryTrim[0] = (float)(2620/1<<FS)*(pow( (float)1.01 , ((float)selfTest[0] - (float)1.0) )); // FT[Xa] factory trim calculation
    factoryTrim[1] = (float)(2620/1<<FS)*(pow( (float)1.01 , ((float)selfTest[1] - (float)1.0) )); // FT[Ya] factory trim calculation
    factoryTrim[2] = (float)(2620/1<<FS)*(pow( (float)1.01 , ((float)selfTest[2] - (float)1.0) )); // FT[Za] factory trim calculation
    factoryTrim[3] = (float)(2620/1<<FS)*(pow( (float)1.01 , ((float)selfTest[3] - (float)1.0) )); // FT[Xg] factory trim calculation
    factoryTrim[4] = (float)(2620/1<<FS)*(pow( (float)1.01 , ((float)selfTest[4] - (float)1.0) )); // FT[Yg] factory trim calculation
    factoryTrim[5] = (float)(2620/1<<FS)*(pow( (float)1.01 , ((float)selfTest[5] - (float)1.0) )); // FT[Zg] factory trim calculation

    // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
    // To get percent, must multiply by 100
    for (int i = 0; i < 3; i++) {
        selfTestResult[i] = (float)100.0*((float)(accelerometer_selfTest_average[i] - accelerometer_average[i]))/factoryTrim[i]; // Report percent differences
        selfTestResult[i+3] = (float)100.0*((float)(gyroscope_selfTest_average[i] - gyroscope_average[i]))/factoryTrim[i+3]; // Report percent differences
    }

    printf("x-axis self test: acceleration trim within : %f%% of factory value\n\r", selfTestResult[0]);
    printf("y-axis self test: acceleration trim within : %f%% of factory value\n\r", selfTestResult[1]);
    printf("z-axis self test: acceleration trim within : %f%% of factory value\n\r", selfTestResult[2]);
    printf("x-axis self test: gyroscope trim within : %f%% of factory value\n\r", selfTestResult[3]);
    printf("y-axis self test: gyroscope trim within : %f%% of factory value\n\r", selfTestResult[4]);
    printf("z-axis self test: gyroscope trim within : %f%% of factory value\n\r", selfTestResult[5]);
}

//************************************
//******** Read sensor data **********
//************************************

/// @brief Read accelerometer data from the chip.
/// @return x, y, and z axis accelerometer data.
AccelerometerData MPU9250::ReadAccelerometerData() {
    AccelerometerData data;
    uint8_t rawData[2];

    //Reading x-axis:
    rawData[0] = ReadByte(MPU9250_ACCEL_XOUT_H);
    rawData[1] = ReadByte(MPU9250_ACCEL_XOUT_L);

    data.ax = (int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]);

    //Reading y-axis:
    rawData[0] = ReadByte(MPU9250_ACCEL_YOUT_H);
    rawData[1] = ReadByte(MPU9250_ACCEL_YOUT_L);

    data.ay = (int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]);

    //Reading z-axis:
    rawData[0] = ReadByte(MPU9250_ACCEL_ZOUT_H);
    rawData[1] = ReadByte(MPU9250_ACCEL_ZOUT_L);

    data.az = (int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]);

    return data;
}

/// @brief Read the gyroscope data from the chip.
/// @return x, y, and z axis gyroscope data.
GyroscopeData MPU9250::ReadGyroscopeData() {
    GyroscopeData data;
    uint8_t rawData[2];

    //Reading x-axis:
    rawData[0] = ReadByte(MPU9250_GYRO_XOUT_H);
    rawData[1] = ReadByte(MPU9250_GYRO_XOUT_L);

    data.gx = (int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]);

    //Reading y-axis:
    rawData[0] = ReadByte(MPU9250_GYRO_YOUT_H);
    rawData[1] = ReadByte(MPU9250_GYRO_YOUT_L);

    data.gy = (int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]);

    //Reading z-axis:
    rawData[0] = ReadByte(MPU9250_GYRO_ZOUT_H);
    rawData[1] = ReadByte(MPU9250_GYRO_ZOUT_L);

    data.gz = (int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]);

    return data;
}

/// @brief Read the current temperature around the chip.
/// @return The current temperature in degrees celcius.
float MPU9250::ReadTemperatureData() {
    int16_t temp_out;
    uint8_t rawData[2];

    //Reading high and low bytes of temperature registers:
    rawData[0] = ReadByte(MPU9250_TEMP_OUT_H);
    rawData[1] = ReadByte(MPU9250_TEMP_OUT_L);

    //Combining MSB and LSB
    temp_out = (int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]); 

    //Calculating actual temperature:
    return (((float) temp_out) / 333.87f) + 21.0f;
}
