// #include "I2S.h"
// #include "starwars.h"

// #define sample_freq 11025.0

// DMA_HandleTypeDef hdma_spi3_tx;


// I2S::I2S(PinName sd, PinName ws, PinName clk, Communication *comm) {
//     _sd = sd;
//     _ws = ws;
//     _clk = clk;

//     communication_protocol = comm;

//     //i2s_instance = this;
// }

// int fs = 32000;
// int freq = 100;

// int i = 0;
// int bufflen = 1;
// int buffer[1];

// void I2S::isr()
// {
//     // buffer[0] = sound_data[i]>>1;//scale down volume a bit on amp
//     // HAL_I2S_Transmit_DMA(&hi2s3, buffer, bufflen);
//     // i++;
//     // if (i>= NUM_ELEMENTS) {
//     //     i = 0;
//     //     //sampletick.detach();

//     //     communication_protocol->sendDebugMessage("Done playing!\n");
//     // }
    

//     // buffer[0] = i * (fs/freq);
//     // HAL_I2S_Transmit_DMA(&hi2s3, buffer, bufflen);
//     // i ++;
//     // if(i == (fs/freq))
//     // {
//     //     i = 0; 
//     // } 
// }

// void I2S::initialize() {
// 	for (int i = 0; i < n_data; i++) {
// 		Wave_Data[i * 2] = (int16_t) (sin(2. * M_PI * 8. * i / 1000.) * 500); // L-ch (x 500 is amplitude)
// 		Wave_Data[i * 2 + 1] =
// 				(int16_t) (sin(2. * M_PI * 10. * i / 1000.) * 500); // R-ch
// 		//printf("%d, %d\n", Wave_Data[i * 2], Wave_Data[i * 2 + 1]);
// 		HAL_Delay(1);
// 	}

//      /* DMA controller clock enable */
//     __HAL_RCC_DMA1_CLK_ENABLE();

//     /* DMA interrupt init */
//     /* DMA1_Stream5_IRQn interrupt configuration */
//     HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
//     HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

//     hi2s3.Instance = SPI3;
//     hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
//     hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
//     hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
//     hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
//     hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_22K;
//     hi2s3.Init.CPOL = I2S_CPOL_LOW;
//     hi2s3.Init.ClockSource = I2S_CLOCK_PLL;

//     if (HAL_I2S_Init(&hi2s3) != HAL_OK)
//     {
//         communication_protocol->sendDebugMessage("Failed to initialize I2S.\n");;
//     }

//     sampletick.attach(callback(this, &I2S::isr), 1.0/sample_freq);//turn on timer interrupt

//     communication_protocol->sendDebugMessage("Sucessfully initialized I2S protocol.\n");

//     HAL_I2S_Transmit_DMA(&hi2s3, rawData, 65535);
// }



// void I2S::loop() {
//     //communication_protocol->sendDebugMessage("Starting playing chirp!\n");

//     // HAL_I2S_Transmit_DMA(&hi2s3, sound_data, NUM_ELEMENTS);

//     int state = (&hi2s3)->State;

//     // if (state == HAL_I2S_STATE_READY) {

//     //     if (HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*) Wave_Data, (uint16_t) n_data * 2) != HAL_OK)
//     //     {
//     //         communication_protocol->sendDebugMessage("Failed to I2S\n");

//     //         state = (&hi2s3)->State;

//     //         printf("State: %d\n", state);
//     //     }
//     // }
// }
// //*********************************************************************************************
// //******** Functions called from within STM32F7 library, used to configure I2S & DMA **********
// //*********************************************************************************************

// int test = 1;

// /// @brief Called when Sending of message is done :)
// /// @param hi2s 
// void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
//     // if(test % 2 == 0) {
//     //   HAL_I2S_Transmit_DMA(hi2s, &rawData[65535], 65535);
//     // }
//     // else {
//     //   HAL_I2S_Transmit_DMA(hi2s, rawData, 65535);
//     // }

//     test++;
// }

// /**
// * @brief I2S MSP Initialization
// * This function configures the hardware resources used in this example
// * @param hi2s: I2S handle pointer
// * @retval None
// */
// void HAL_I2S_MspInit(I2S_HandleTypeDef* hi2s)
// {
//   GPIO_InitTypeDef GPIO_InitStruct = {0};
//   RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

//   if(hi2s->Instance==SPI3)
//   {
//     /** Initializes the peripherals clock */
//     PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
//     PeriphClkInitStruct.PLLI2S.PLLI2SN = 96;
//     PeriphClkInitStruct.PLLI2S.PLLI2SP = RCC_PLLP_DIV2;
//     PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
//     PeriphClkInitStruct.PLLI2S.PLLI2SQ = 2;
//     PeriphClkInitStruct.PLLI2SDivQ = 1;
//     PeriphClkInitStruct.I2sClockSelection = RCC_I2SCLKSOURCE_PLLI2S;

//     if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
//     {
//         //TODO: print error message
//     }

//     /* Peripheral clock enable */
//     __HAL_RCC_SPI3_CLK_ENABLE();

//     __HAL_RCC_GPIOA_CLK_ENABLE();
//     __HAL_RCC_GPIOB_CLK_ENABLE();
//     __HAL_RCC_GPIOC_CLK_ENABLE();
//     /**I2S3 GPIO Configuration
//     PA4     ------> I2S3_WS
//     PB2     ------> I2S3_SD
//     PC10     ------> I2S3_CK
//     */
//     GPIO_InitStruct.Pin = GPIO_PIN_4;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin = GPIO_PIN_2;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     GPIO_InitStruct.Alternate = GPIO_AF7_SPI3;
//     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin = GPIO_PIN_10;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
//     HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//     /* I2S3 DMA Init */
//     /* SPI3_TX Init */
//     hdma_spi3_tx.Instance = DMA1_Stream5;
//     hdma_spi3_tx.Init.Channel = DMA_CHANNEL_0;
//     hdma_spi3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//     hdma_spi3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//     hdma_spi3_tx.Init.MemInc = DMA_MINC_ENABLE;
//     hdma_spi3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//     hdma_spi3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//     hdma_spi3_tx.Init.Mode = DMA_NORMAL;
//     hdma_spi3_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
//     hdma_spi3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

//     if (HAL_DMA_Init(&hdma_spi3_tx) != HAL_OK)
//     {
//       //Error_Handler();
//     }

//     __HAL_LINKDMA(hi2s,hdmatx,hdma_spi3_tx);
//   }
// }

// /**
// * @brief I2S MSP De-Initialization
// * This function freeze the hardware resources used in this example
// * @param hi2s: I2S handle pointer
// * @retval None
// */
// void HAL_I2S_MspDeInit(I2S_HandleTypeDef* hi2s)
// {
//   if(hi2s->Instance==SPI3)
//   {
//     /* Peripheral clock disable */
//     __HAL_RCC_SPI3_CLK_DISABLE();

//     /**I2S3 GPIO Configuration
//     PA4     ------> I2S3_WS
//     PB2     ------> I2S3_SD
//     PC10     ------> I2S3_CK
//     */
//     HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);

//     HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);

//     HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10);

//     /* I2S3 DMA DeInit */
//     HAL_DMA_DeInit(hi2s->hdmatx);
//   }
// }

// void DMA1_Stream5_IRQHandler(void)
// {
//   HAL_DMA_IRQHandler(&hdma_spi3_tx);
// }
