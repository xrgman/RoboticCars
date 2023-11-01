#include "I2S.h"

// static uint32_t __attribute__((section(".sai_dma_buffer_section"))) __attribute__((aligned(32))) sai_receive_buffer[I2S_BLOCK_SIZE] = {0};
static uint16_t __attribute__((section(".sai_dma_buffer_section"))) __attribute__((aligned(16))) sai_receive_buffer16[I2S_BLOCK_SIZE] = {0};
// static uint16_t __attribute__((section(".sai_dma_buffer_section"))) __attribute__((aligned(32))) sai_transmit_buffer[320 * 8] = {0};

static uint32_t SAI1_client = 0;
DMA_HandleTypeDef hdma_sai1_a, hdma_sai1_b;
SAI_HandleTypeDef hsai_BlockA1, hsai_BlockB1;

uint8_t writeBuffer[I2S_BLOCK_SIZE];

I2S::I2S(PinName sd, PinName ws, PinName clk, Communication *comm)
{
  // TODO, actually use these some how
  _sd = sd;
  _ws = ws;
  _clk = clk;

  communication_protocol = comm;

  // Setting default values:
  this->sampleRate = SAI_AUDIO_FREQUENCY_44K;
  this->wordSizeInput = SAI_PROTOCOL_DATASIZE_16BIT;
  this->wordSizeOutput = SAI_PROTOCOL_DATASIZE_16BIT;
  this->numChannelsInput = 2;
  this->numChannelsOutput = 2;

  i2s_instance = this;

  initializeClocks();
}

bool I2S::initialize(uint32_t sampleRate, uint8_t wordSizeInput, uint8_t wordSizeOutput, uint8_t numChannelsInput, uint8_t numChannelsOutput)
{
  this->sampleRate = sampleRate;
  this->wordSizeInput = wordSizeInput;
  this->wordSizeOutput = wordSizeOutput;
  this->numChannelsInput = numChannelsInput;
  this->numChannelsOutput = numChannelsOutput;

  /* Stop running DMA if there */
  if (initialized)
  {
    HAL_SAI_DMAStop(&hsai_BlockA1);
  }

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

  // Configuring SAI block A (RX - Master)
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_RX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_FULL;
  hsai_BlockA1.Init.AudioFrequency = sampleRate;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  // hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_OUTBLOCKA_ENABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;

  // The 2 here represents number of channels, so should be 6 eventually (or 8)
  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, wordSizeInput, numChannelsInput) != HAL_OK)
  {
    communication_protocol->sendDebugMessage("Failed to initialize SAI block A\n");

    return false;
  }

  // Configuring SAI block B (TX)
  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.AudioMode = SAI_MODESLAVE_TX;
  hsai_BlockB1.Init.Synchro = SAI_SYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_FULL;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  // hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_OUTBLOCKA_ENABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;

  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, wordSizeOutput, numChannelsOutput) != HAL_OK)
  {
    communication_protocol->sendDebugMessage("Failed to initialize SAI block B\n");

    return false;
  }

  // Starting receiver buffer via DMA (runs constantly):
  if (HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)sai_receive_buffer16, I2S_BLOCK_SIZE) != HAL_OK)
  {
    communication_protocol->sendDebugMessage("Failed to receiving buffer.\n");

    return false;
  }

  // Transmit one byte over TX line to prevent distortion:
  uint8_t txInitBuff[1] = {0};
  // HAL_SAI_Transmit_DMA(&hsai_BlockB1, txInitBuff, 1);

  initialized = true;

  communication_protocol->sendDebugMessage("Sucessfully initialized I2S protocol.\n");

  return true;
}

void I2S::initializeClocks()
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 96;               // N value
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 1;                // Second column /R
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 4;                // Second column /Q (3)
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2; // Second column /P
  PeriphClkInitStruct.PLLSAIDivQ = 2;                    // Thrith column /Q (1)
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;     // Thirth column /R
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    i2s_instance->printErrorMessage("Failed to initialize SAI clocks.\n");
  }
}

void I2S::setOnTxCpltCallback(Callback<void()> onTxCpltCallback)
{
  this->onTxCpltCallback = onTxCpltCallback;
}

void I2S::setOnRxCpltCallback(Callback<void()> onRxCpltCallback)
{
  this->onRxCpltCallback = onRxCpltCallback;
}

//*********************************************************************************************
//******** Read & Write ***********************************************************************
//*********************************************************************************************

/// @brief Write 8-bit data to the I2S bus.
/// @param buff Data to write.
/// @param nrOfElements Number of elements in the data array (Max: 2048)
/// @return Whether writing the data was successfull.
bool I2S::write(uint8_t *buff, uint16_t nrOfElements)
{
  return HAL_SAI_Transmit_DMA(&hsai_BlockB1, buff, nrOfElements) == HAL_OK;
}

/// @brief Write 16-bit data to the I2S bus.
/// @param buff Data to write.
/// @param nrOfElements Number of elements in the data array (Max: 1024)
/// @return Whether writing the data was successfull.
bool I2S::write(uint16_t *buff, uint16_t nrOfElements)
{
  for (int i = 0; i < nrOfElements; i++)
  {
    writeBuffer[i * 2] = (uint8_t)(buff[i] && 0xFF);
    writeBuffer[i * 2 + 1] = (uint8_t)(buff[i] >> 8) & 0xFF;
  }

  return write(writeBuffer, nrOfElements * 2);
  //return HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t*) buff, nrOfElements) == HAL_OK; // write(writeBuffer, nrOfElements * 2); // HAL_SAI_Transmit_DMA(&hsai_BlockB1, writeBuffer, nrOfElements * 2) == HAL_OK;;
}

/// @brief Write 32-bit data to the I2S bus.
/// @param buff Data to write.
/// @param nrOfElements Number of elements in the data array (Max: 512)
/// @return Whether writing the data was successfull.
bool I2S::write(uint32_t *buff, uint16_t nrOfElements)
{
  for (int i = 0; i < nrOfElements; i++)
  {
    writeBuffer[i * 4] = (uint8_t)(buff[i] && 0xFF);
    writeBuffer[i * 4 + 1] = (uint8_t)(buff[i] >> 8) & 0xFF;
    writeBuffer[i * 4 + 2] = (uint8_t)(buff[i] >> 16) & 0xFF;
    writeBuffer[i * 4 + 3] = (uint8_t)(buff[i] >> 24) & 0xFF;
  }

  return write(writeBuffer, nrOfElements * 4);
}

/// @brief Get a pointer to the 16-bit RX buffer of the receiver block A, to be used after RX is signeled as complete.
/// @return Pointer to the buffer.
uint16_t *I2S::getRxBuffer16()
{
  return sai_receive_buffer16;
}

/// @brief Get a pointer to the 32-bit RX buffer of the receiver block A, to be used after RX is signeled as complete.
/// @return Pointer to the buffer.
uint32_t *I2S::getRxBuffer32()
{
  return NULL;
  // return sai_receive_buffer;
}

//*********************************************************************************************
//******** Get & set **************************************************************************
//*********************************************************************************************

/// @brief Get the current sample rate.
/// @return Sampling rate.
uint32_t I2S::getSampleRate()
{
  return this->sampleRate;
}

/// @brief Get the current word size for master A.
/// @return Word size.
uint8_t I2S::getWordSizeInput()
{
  return this->wordSizeInput;
}

/// @brief Get the current word size for slave B.
/// @return Word size.
uint8_t I2S::getWordSizeOutput()
{
  return this->wordSizeOutput;
}

/// @brief Get the current number of configured channels for master A.
/// @return Number of channels.
uint8_t I2S::getNumChannelsInput()
{
  return this->numChannelsInput;
}

/// @brief Get the current number of configured channels for slave B.
/// @return Number of channels.
uint8_t I2S::getNumChannelsOutput()
{
  return this->numChannelsOutput;
}

//*********************************************************************************************
//******** Functions called from the callbacks ************************************************
//*********************************************************************************************

void I2S::printErrorMessage(const char *message)
{
  communication_protocol->sendDebugMessage(message);
}

//*********************************************************************************************
//******** SAI callbacks **********************************************************************
//*********************************************************************************************

/// @brief Called from within the HAL source code, signaling that transmittion is completed.
/// @param hsai Reference to the hsai object that executed the transmittion.
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  // Calling callback if defined:
  if (i2s_instance->onTxCpltCallback)
  {
    i2s_instance->onTxCpltCallback();
  }
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  //   i2s_instance->printErrorMessage("HAL!\n");

  //   if (i2s_instance->onTxCpltCallback)
  //   {
  //     i2s_instance->onTxCpltCallback();
  //   }
}

/// @brief Called from within the HAL source code, signaling that the receiving buffer is filled.
/// @param hsai Reference to the hsai object that executed the transmittion.
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  // for (int i = 0; i < 20; i++)
  // {
  //   //  if (sai_receive_buffer[i] > 0 && sai_receive_buffer[i] < 0xFFFF)
  //   //   {

  //   snprintf(msg, sizeof(msg), "[%d] = 0x%lx, ", i, sai_receive_buffer[i]);

  //   i2s_instance->printErrorMessage(msg);
  //   //}
  // }
  // i2s_instance->printErrorMessage("\n");

  // Calling callback if defined:
  if (i2s_instance->onRxCpltCallback)
  {
    i2s_instance->onRxCpltCallback();
  }
}

void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  char msg[100];
  snprintf(msg, sizeof(msg), "SAI error occured! (Code: %ld)\n", hsai->ErrorCode);

  i2s_instance->printErrorMessage(msg);
}

//*********************************************************************************************
//******** Functions called from within STM32F7 library, used to configure I2S & DMA **********
//*********************************************************************************************

void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* SAI1 */
  if (hsai->Instance == SAI1_Block_A)
  {
    /* Peripheral clock enable */
    if (SAI1_client == 0)
    {
      __HAL_RCC_SAI1_CLK_ENABLE();
    }
    SAI1_client++;

    /**SAI1_A_Block_A GPIO Configuration
    PE4     ------> SAI1_FS_A
    PE5     ------> SAI1_SCK_A
    PE6     ------> SAI1_SD_A
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* Peripheral DMA init*/

    hdma_sai1_a.Instance = DMA2_Stream1;
    hdma_sai1_a.Init.Channel = DMA_CHANNEL_0;
    hdma_sai1_a.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // DMA_PDATAALIGN_WORD; // DMA_PDATAALIGN_BYTE; DMA_PDATAALIGN_HALFWORD
    hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;    // DMA_MDATAALIGN_BYTE;
    hdma_sai1_a.Init.Mode = DMA_CIRCULAR;
    hdma_sai1_a.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sai1_a.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai1_a.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_sai1_a.Init.PeriphBurst = DMA_PBURST_SINGLE;

    if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK)
    {
      i2s_instance->printErrorMessage("Failed to initialize DMA A.\n");
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one stream to perform all the requested DMAs. */
    __HAL_LINKDMA(hsai, hdmarx, hdma_sai1_a);

    __HAL_LINKDMA(hsai, hdmatx, hdma_sai1_a);
  }
  if (hsai->Instance == SAI1_Block_B)
  {
    /* Peripheral clock enable */
    if (SAI1_client == 0)
    {
      __HAL_RCC_SAI1_CLK_ENABLE();
    }
    SAI1_client++;

    /**SAI1_B_Block_B GPIO Configuration
    PE3     ------> SAI1_SD_B
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* Peripheral DMA init*/
    hdma_sai1_b.Instance = DMA2_Stream0;
    hdma_sai1_b.Init.Channel = DMA_CHANNEL_10;
    hdma_sai1_b.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sai1_b.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_b.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_b.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sai1_b.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sai1_b.Init.Mode = DMA_NORMAL;
    hdma_sai1_b.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sai1_b.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    //hdma_sai1_b.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai1_b.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_sai1_b.Init.PeriphBurst = DMA_PBURST_SINGLE;

    if (HAL_DMA_Init(&hdma_sai1_b) != HAL_OK)
    {
      i2s_instance->printErrorMessage("Failed to initialize DMA B.\n");
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one stream to perform all the requested DMAs. */
    __HAL_LINKDMA(hsai, hdmarx, hdma_sai1_b);
    __HAL_LINKDMA(hsai, hdmatx, hdma_sai1_b);
  }
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
  /* SAI1 */
  if (hsai->Instance == SAI1_Block_A)
  {
    SAI1_client--;
    if (SAI1_client == 0)
    {
      /* Peripheral clock disable */
      __HAL_RCC_SAI1_CLK_DISABLE();
    }

    /**SAI1_A_Block_A GPIO Configuration
    PE4     ------> SAI1_FS_A
    PE5     ------> SAI1_SCK_A
    PE6     ------> SAI1_SD_A
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

    /* SAI1 DMA Deinit */
    HAL_DMA_DeInit(hsai->hdmarx);
    HAL_DMA_DeInit(hsai->hdmatx);
  }
  if (hsai->Instance == SAI1_Block_B)
  {
    SAI1_client--;
    if (SAI1_client == 0)
    {
      /* Peripheral clock disable */
      __HAL_RCC_SAI1_CLK_DISABLE();
    }

    /**SAI1_B_Block_B GPIO Configuration
    PE3     ------> SAI1_SD_B
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_3);

    /* SAI1 DMA Deinit */
    HAL_DMA_DeInit(hsai->hdmarx);
    HAL_DMA_DeInit(hsai->hdmatx);
  }
}

// Receive
void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_sai1_a);
}

// Transmit
void DMA2_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_sai1_b);
}

void SAI1_IRQHandler(void)
{
  HAL_SAI_IRQHandler(&hsai_BlockA1);
  HAL_SAI_IRQHandler(&hsai_BlockB1);
}
