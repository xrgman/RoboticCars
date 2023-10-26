#ifndef I2S_H
#define I2S_H

#include "mbed.h"
#include "enums.h"
#include "stm32f7xx_hal.h"
#include "communication.h"

#define I2S_BLOCK_SIZE 2048//2056
#define I2S_SAMPLE_RATE SAMPLE_RATE_44100
#define I2S_WORDSIZE_INPUT WORD_SIZE_24_BITS
#define I2S_WORDSIZE_OUTPUT WORD_SIZE_16_BITS
#define I2S_NUMCHANNELS_INPUT NUM_CHANNELS_2
#define I2S_NUMCHANNELS_OUTPUT NUM_CHANNELS_2

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

        bool initialize(uint32_t sampleRate, uint8_t wordSizeInput, uint8_t wordSizeOutput, uint8_t numChannelsInput, uint8_t numChannelsOutput);
        void setOnTxCpltCallback(Callback<void()> onTxCpltCallback);
        void setOnRxCpltCallback(Callback<void()> onRxCpltCallback);

        bool write(uint8_t *buff, uint16_t nrOfElements);
        bool write(uint16_t *buff, uint16_t nrOfElements);
        bool write(uint32_t *buff, uint16_t nrOfElements);

        uint32_t* getPointerToReadBuffer();

        uint32_t getSampleRate();
        uint8_t getWordSizeInput();
        uint8_t getWordSizeOutput();
        uint8_t getNumChannelsInput();
        uint8_t getNumChannelsOutput();

        //Needed from inside the callback:
        void printErrorMessage(const char *message);

        Callback<void()> onTxCpltCallback;
        Callback<void()> onRxCpltCallback;
    private:
        PinName _sd, _ws, _clk;
        Communication *communication_protocol;
        
        // SAI_HandleTypeDef hsai_BlockA1;
        // SAI_HandleTypeDef hsai_BlockB1;
        bool initialized = false;

        uint32_t sampleRate;
        uint8_t wordSizeInput, wordSizeOutput, numChannelsInput, numChannelsOutput;

        void initializeClocks();
};

static I2S *i2s_instance;

#endif
