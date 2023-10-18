#ifndef I2S_H
#define I2S_H

#include "mbed.h"
#include "stm32f7xx_hal.h"
#include "communication.h"

#define n_data 1000

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported functions prototypes ---------------------------------------------*/
void DMA2_Stream1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

class I2S {
    public:
        I2S(PinName sd, PinName ws, PinName clk, Communication *comm);

        void initialize();
        bool write(uint8_t *buff, uint16_t nrOfElements);
        bool write(uint16_t *buff, uint16_t nrOfElements);
        bool write(uint32_t *buff, uint16_t nrOfElements);
        void loop();

    private:
        PinName _sd, _ws, _clk;
        Communication *communication_protocol;

        //I2S_HandleTypeDef hi2s3;
        SAI_HandleTypeDef hsai_BlockA1;
        SAI_HandleTypeDef hsai_BlockB1;

        void isr();
        Ticker sampletick;
        int i = 0;
        int bufflen = 1;
        //uint8_t buffer[1];

        //int16_t Wave_Data[n_data * 2];    // n_data is defined as 1000
        
};

//I2S *i2s_instance;

#endif
