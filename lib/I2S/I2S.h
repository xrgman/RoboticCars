#ifndef I2S_H
#define I2S_H

#include "mbed.h"
#include "stm32f7xx_hal.h"
#include "communication.h"

#define I2S_BLOCK_SIZE 2048//2056

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported functions prototypes ---------------------------------------------*/
void DMA2_Stream0_IRQHandler(void);
void DMA2_Stream1_IRQHandler(void);
void SAI1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

class I2S {
    public:
        I2S(PinName sd, PinName ws, PinName clk, Communication *comm);

        void initialize(uint32_t sampleRateOutput, uint8_t wordSizeInput, uint8_t wordSizeOutput);
        void setOnTxCpltCallback(Callback<void()> onTxCpltCallback);
        void setOnRxCpltCallback(Callback<void()> onRxCpltCallback);

        bool write(uint8_t *buff, uint16_t nrOfElements);
        bool write(uint16_t *buff, uint16_t nrOfElements);
        bool write(uint32_t *buff, uint16_t nrOfElements);

        uint32_t* getPointerToReadBuffer();

        uint32_t getSampleRateOutput();
        uint8_t getWordSizeInput();
        uint8_t getWordSizeOutput();

        //Needed from inside the callback:
        void printErrorMessage(const char *message);
        void run();

        Callback<void()> onTxCpltCallback;
        Callback<void()> onRxCpltCallback;
    private:
        PinName _sd, _ws, _clk;
        Communication *communication_protocol;
        
        // SAI_HandleTypeDef hsai_BlockA1;
        // SAI_HandleTypeDef hsai_BlockB1;

        uint32_t sampleRateOutput;
        uint8_t wordSizeInput, wordSizeOutput;

        void initializeClocks();
};

static I2S *i2s_instance;

#endif
