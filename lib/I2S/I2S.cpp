#include "I2S.h"

static uint32_t SAI1_client = 0;
DMA_HandleTypeDef hdma_sai1_a;

uint8_t convertBuffer[I2S_BLOCK_SIZE];
bool donePlaying = false;

I2S::I2S(PinName sd, PinName ws, PinName clk, Communication *comm)
{
  _sd = sd;
  _ws = ws;
  _clk = clk;

  communication_protocol = comm;

  i2s_instance = this;
}

void I2S::initialize(uint32_t sampleRateOutput, uint8_t wordSizeOutput, uint8_t wordSizeInput)
{
  this->sampleRateOutput = sampleRateOutput;
  this->wordSizeInput = wordSizeInput;
  this->wordSizeOutput = wordSizeOutput;

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA1.Init.AudioFrequency = sampleRateOutput;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  // hsai_BlockA1.Init.MonoStereoMode = SAI_MONOMODE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;

  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, wordSizeOutput, 2) != HAL_OK)
  {
    communication_protocol->sendDebugMessage("Failed to initialize SAI block A\n");
  }

  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockB1.Init.Synchro = SAI_SYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;

  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, wordSizeInput, 2) != HAL_OK)
  {
    communication_protocol->sendDebugMessage("Failed to initialize SAI block B\n");
  }

  communication_protocol->sendDebugMessage("Sucessfully initialized I2S protocol.\n");
}

void I2S::setOnTxCpltCallback(Callback<void()> onTxCpltCallback)
{
  this->onTxCpltCallback = onTxCpltCallback;
}

bool I2S::write(uint8_t *buff, uint16_t nrOfElements)
{
  return HAL_SAI_Transmit_DMA(&hsai_BlockA1, buff, nrOfElements) == HAL_OK;
}

bool I2S::write(uint16_t *buff, uint16_t nrOfElements)
{
  if(nrOfElements > I2S_BLOCK_SIZE) {
    printf("Oeps!\n");
  }

  for (int i = 0; i < nrOfElements; i++)
  {
    convertBuffer[i * 2] = (uint8_t)(buff[i] && 0xFF);
    convertBuffer[i * 2 + 1] = (uint8_t)(buff[i] >> 8) & 0xFF;
  }

  return HAL_SAI_Transmit_DMA(&hsai_BlockA1, convertBuffer, nrOfElements * 2) == HAL_OK;
}

bool I2S::write(uint32_t *buff, uint16_t nrOfElements)
{
  // TODO
  //  return HAL_SAI_Transmit_DMA(&hsai_BlockA1, buff, nrOfElements) == HAL_OK;
}

void I2S::run()
{

}

//*********************************************************************************************
//******** Get & set **************************************************************************
//*********************************************************************************************

/// @brief Get the current sample rate for master A.
/// @return Sampling rate.
uint32_t I2S::getSampleRateOutput()
{
  return this->sampleRateOutput;
}

/// @brief Get the current word size for slave B.
/// @return Word size.
uint8_t I2S::getWordSizeInput()
{
  return this->wordSizeInput;
}

/// @brief Get the current word size for master A.
/// @return Word size.
uint8_t I2S::getWordSizeOutput()
{
  return this->wordSizeOutput;
}

//*********************************************************************************************
//******** Functions called from the callbacks ************************************************
//*********************************************************************************************

void I2S::printErrorMessage(const char *message)
{
  communication_protocol->sendDebugMessage(message);
}

//*********************************************************************************************
//******** Functions called from within STM32F7 library, used to configure I2S & DMA **********
//*********************************************************************************************

/// @brief Called when Sending of message is done :)
/// @param hi2s
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  // Calling callback if defined:
  if (i2s_instance->onTxCpltCallback)
  {
    i2s_instance->onTxCpltCallback();
  }
}

void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /* SAI1 */
  if (hsai->Instance == SAI1_Block_A)
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 96; //N value 
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2; //Second column /R
    PeriphClkInitStruct.PLLSAI.PLLSAIQ = 4; //Second column /Q (3)
    PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2; //Second column /P
    PeriphClkInitStruct.PLLSAIDivQ = 1; //Thrith column /Q (1)
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2; //Thirth column /R
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      i2s_instance->printErrorMessage("Failed to initialize SAI clocks.\n");
    }

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
    hdma_sai1_a.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_sai1_a.Init.Mode = DMA_NORMAL;
    hdma_sai1_a.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sai1_a.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai1_a.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_sai1_a.Init.PeriphBurst = DMA_PBURST_SINGLE;

    if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK)
    {
      i2s_instance->printErrorMessage("Failed to initialize DMA.\n");
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
  }
}

void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_sai1_a);
}
