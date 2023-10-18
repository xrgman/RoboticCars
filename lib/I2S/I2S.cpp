#include "I2S.h"
#include "get_lower.h"

#define sample_freq 22025.0

static uint32_t SAI1_client = 0;
DMA_HandleTypeDef hdma_sai1_a;

bool playing = false;
int part = 0;
const int maxNrOfElements = 65535;
int nrOfParts = ceil(NUM_ELEMENTS / maxNrOfElements);

//uint8_t buffer[NUM_ELEMENTS];

I2S::I2S(PinName sd, PinName ws, PinName clk, Communication *comm)
{
  _sd = sd;
  _ws = ws;
  _clk = clk;

  communication_protocol = comm;

  // i2s_instance = this;
}

void I2S::isr()
{
  // buffer[0] = data[i]; //>> 1; // scale down volume a bit on amp
  // HAL_SAI_Transmit_IT(&hsai_BlockA1, buffer, bufflen);
  // i++;

  // if (i >= NUM_ELEMENTS)
  // {
  //   i = 0;
  //   sampletick.detach();

  //   communication_protocol->sendDebugMessage("Done playing!\n");
  // }
}

void I2S::initialize()
{
  // for (int i = 0; i < n_data; i++)
  // {
  //   Wave_Data[i * 2] = (int16_t)(sin(2. * M_PI * 8. * i / 1000.) * 500);      // L-ch (x 500 is amplitude)
  //   Wave_Data[i * 2 + 1] = (int16_t)(sin(2. * M_PI * 10. * i / 1000.) * 500); // R-ch

  //   HAL_Delay(1);
  // }

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
  hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_22K;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_MONOMODE;
  //hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;

  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
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

  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
  {
    communication_protocol->sendDebugMessage("Failed to initialize SAI block B\n");
  }

  // sampletick.attach(callback(this, &I2S::isr), 1.0 / sample_freq); // turn on timer interrupt

  communication_protocol->sendDebugMessage("Sucessfully initialized I2S protocol.\n");

  // HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*) Wave_Data, n_data * 2);
  //HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)Wave_Data, (uint16_t)n_data * 2);
  //write((uint16_t *)Wave_Data, (uint16_t)n_data * 2);
  // Storing elements in buffer:
  // for (int x = 0; x < NUM_ELEMENTS; x++)
  // {
  //   buffer[x] = data[x] >> 1;
  // }
}

void I2S::loop()
{
  /*
  if (!playing)
  {
    playing = true;

    // Checking whether to stop:
    if (part > nrOfParts)
    {
      communication_protocol->sendDebugMessage("Done playing all parts.\n");

      return;
    }

    // char msg[100];
    // snprintf(msg, sizeof(msg), "Playing part %d.\n", part);
    // communication_protocol->sendDebugMessage(msg);

    int nrOfElements = (part + 1) * maxNrOfElements <= NUM_ELEMENTS ? maxNrOfElements : NUM_ELEMENTS - ((part)*maxNrOfElements);

    if (HAL_SAI_Transmit_DMA(&hsai_BlockA1, &buffer[part * nrOfElements], nrOfElements) != HAL_OK)
    {
      communication_protocol->sendDebugMessage("Error playing part.\n");
    }

    part++;
  }*/
}

bool I2S::write(uint8_t *buff, uint16_t nrOfElements) {
  return HAL_SAI_Transmit_DMA(&hsai_BlockA1, buff, nrOfElements) == HAL_OK;
}

uint8_t convertedBuff[4412 * 2];

//HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)Wave_Data, (uint16_t)n_data * 2);
bool I2S::write(uint16_t *buff, uint16_t nrOfElements) {
  printf("Writing 16 bit\n");

  for(int i = 0; i < nrOfElements; i++) {
    convertedBuff[i * 2] = 128;//(uint8_t)(buff[i] && 0xFF);
    convertedBuff[i * 2 + 1] = 155; //(uint8_t) (buff[i] >> 8);
  }

  return HAL_SAI_Transmit_DMA(&hsai_BlockA1, convertedBuff, nrOfElements * 2) == HAL_OK;
}

bool I2S::write(uint32_t *buff, uint16_t nrOfElements) {
  //return HAL_SAI_Transmit_DMA(&hsai_BlockA1, buff, nrOfElements) == HAL_OK;
}
//*********************************************************************************************
//******** Functions called from within STM32F7 library, used to configure I2S & DMA **********
//*********************************************************************************************

/// @brief Called when Sending of message is done :)
/// @param hi2s
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  playing = false;
}

void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /* SAI1 */
  if (hsai->Instance == SAI1_Block_A)
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 96;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIQ = 3;
    PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
    PeriphClkInitStruct.PLLSAIDivQ = 1;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      // TODO
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
      // TODO
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
