#include "I2S.h"

// #define TRANSMITTER_MASTER

#define SAI_DMA_BUFFER_SIZE 128 // 2000 //(320 * 8 * 2)
#define SAI_DMA_GET SAI_DMA_BUFFER_SIZE
static uint32_t __attribute__((section(".sai_dma_buffer_section"))) __attribute__((aligned(32))) sai_receive_buffer[SAI_DMA_BUFFER_SIZE] = {0};
// static uint16_t __attribute__((section(".sai_dma_buffer_section"))) __attribute__((aligned(32))) sai_transmit_buffer[320 * 8] = {0};

static uint32_t SAI1_client = 0;
static bool startedReceiving = 0;
DMA_HandleTypeDef hdma_sai1_a, hdma_sai1_b;
SAI_HandleTypeDef hsai_BlockA1, hsai_BlockB1;

uint8_t writeBuffer[I2S_BLOCK_SIZE];
bool doneReading = true;

I2S::I2S(PinName sd, PinName ws, PinName clk, Communication *comm)
{
  _sd = sd;
  _ws = ws;
  _clk = clk;

  communication_protocol = comm;

  i2s_instance = this;

  initializeClocks();
}

void I2S::initialize(uint32_t sampleRateOutput, uint8_t wordSizeOutput, uint8_t wordSizeInput)
{
  this->sampleRateOutput = sampleRateOutput;
  this->wordSizeInput = wordSizeInput;
  this->wordSizeOutput = wordSizeOutput;

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

#ifdef TRANSMITTER_MASTER
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_FULL; // SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA1.Init.AudioFrequency = sampleRateOutput;
  // hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_OUTBLOCKA_ENABLE;
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
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_FULL; // SAI_FIFOTHRESHOLD_EMPTY;
  // hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_OUTBLOCKA_ENABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;

  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, wordSizeInput, 2) != HAL_OK)
  {
    communication_protocol->sendDebugMessage("Failed to initialize SAI block B\n");
  }

#else

  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_RX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_FULL;
  hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  // hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_OUTBLOCKA_ENABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;

  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_32BIT, 2) != HAL_OK)
  {
    communication_protocol->sendDebugMessage("Failed to initialize SAI block A\n");
  }

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

  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
  {
    communication_protocol->sendDebugMessage("Failed to initialize SAI block B\n");
  }

#endif

  if (!startedReceiving)
  {
    startedReceiving = true;

    if (HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)sai_receive_buffer, SAI_DMA_GET) != HAL_OK) //8000
    {
      communication_protocol->sendDebugMessage("Failed to start.\n");
    }
  }

  communication_protocol->sendDebugMessage("Sucessfully initialized I2S protocol.\n");
}

void I2S::initializeClocks()
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 96;               // N value
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;                // Second column /R
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 4;                // Second column /Q (3)
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2; // Second column /P
  PeriphClkInitStruct.PLLSAIDivQ = 1;                    // Thrith column /Q (1)
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


void I2S::run()
{
  if (doneReading)
  {
    doneReading = false;

    // read(emptyBuff, 2048);
  }
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

#ifdef TRANSMITTER_MASTER
  return HAL_SAI_Transmit_DMA(&hsai_BlockA1, buff, nrOfElements) == HAL_OK;
#else
  return HAL_SAI_Transmit_DMA(&hsai_BlockB1, buff, nrOfElements) == HAL_OK;
#endif
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

  return write(writeBuffer, nrOfElements * 2); // HAL_SAI_Transmit_DMA(&hsai_BlockB1, writeBuffer, nrOfElements * 2) == HAL_OK;;
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

/// @brief Read 8-bit data from the I2S bus.
/// @param buff Buffer to read data into.
/// @param nrOfElements Number of elements to read (Max: 2048)
/// @return Whether reading the data was successfull.
bool I2S::read(uint8_t *buff, uint16_t nrOfElements)
{
// printf("Reading 8-bit\n");

// HAL_SAI_Abort(&hsai_BlockB1);
// HAL_SAI_Abort(&hsai_BlockA1);

// Transmit needs to be in progress for this to work????
// Match read and write speeds?
#ifdef TRANSMITTER_MASTER
  if (HAL_SAI_Receive_DMA(&hsai_BlockB1, buff, nrOfElements) != HAL_OK)
  {
    printf("Failed reading 8-bit\n");
  }
#else
  if (HAL_SAI_Receive_DMA(&hsai_BlockA1, buff, nrOfElements) != HAL_OK)
  {
    printf("Failed reading 8-bit\n");
  }

#endif

  return true;
  // return HAL_SAI_Receive_DMA(&hsai_BlockA1, buff, nrOfElements) == HAL_OK;
}

/// @brief Read 16-bit data from the I2S bus.
/// @param buff Buffer to read data into.
/// @param nrOfElements Number of elements to read (Max: 2048)
/// @return Whether reading the data was successfull.
bool I2S::read(uint16_t *buff, uint16_t nrOfElements)
{

  return HAL_SAI_Receive_DMA(&hsai_BlockB1, (uint8_t *)buff, nrOfElements * 2) == HAL_OK;

  // for (int i = 0; i < nrOfElements; i++)
  // {
  //   buff[i] = ((uint16_t)readBuffer[i * 2] << 8) | readBuffer[i * 2 + 1];
  // }

  // return true;
}

/// @brief Read 32-bit data from the I2S bus.
/// @param buff Buffer to read data into.
/// @param nrOfElements Number of elements to read (Max: 2048)
/// @return Whether reading the data was successfull.
bool I2S::read(uint32_t *buff, uint16_t nrOfElements)
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
//******** SAI callbacks **********************************************************************
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

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  // doneReading = true;
  // i2s_instance->printErrorMessage("RX done!!!!!!!.\n");
  // char msg[100];

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
#ifdef TRANSMITTER_MASTER
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
    hdma_sai1_a.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_sai1_a.Init.Mode = DMA_NORMAL;
    hdma_sai1_a.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_ENABLE; // CHANGED
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
    hdma_sai1_b.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sai1_b.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_b.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_b.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_sai1_b.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_sai1_b.Init.Mode = DMA_NORMAL; // DMA_NORMAL; //DMA_CIRCULAR;
    hdma_sai1_b.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sai1_b.Init.FIFOMode = DMA_FIFOMODE_ENABLE; // CHANGED
    hdma_sai1_b.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
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
#else
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
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; // DMA_PDATAALIGN_WORD; // DMA_PDATAALIGN_BYTE;
    hdma_sai1_a.Init.MemDataAlignment = DMA_PDATAALIGN_WORD;    // DMA_MDATAALIGN_BYTE;
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
    hdma_sai1_b.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_sai1_b.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_sai1_b.Init.Mode = DMA_NORMAL;
    hdma_sai1_b.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sai1_b.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sai1_b.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
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

#endif
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
#ifdef TRANSMITTER_MASTER
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
#else

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
#endif
}

// Receive
void DMA2_Stream1_IRQHandler(void)
{
  // i2s_instance->printErrorMessage("Interrupt stream 1 (Block A)\n");

  HAL_DMA_IRQHandler(&hdma_sai1_a);
}

// Transmit
void DMA2_Stream0_IRQHandler(void)
{
  // i2s_instance->printErrorMessage("Interrupt stream 0 (Block B)\n");

  HAL_DMA_IRQHandler(&hdma_sai1_b);
}

void SAI1_IRQHandler(void)
{
  HAL_SAI_IRQHandler(&hsai_BlockA1);
  HAL_SAI_IRQHandler(&hsai_BlockB1);
}
