// #include "ac101_new.h"
// #include "ac101registers.h"
// #include "util.h"

// struct kv_map
// {
//     int val;
//     int bit;
// };

// struct aif1_fs
// {
//     unsigned samp_rate;
//     int bclk_div;
//     int srbit;
// #define _SERIES_24_576K 0
// #define _SERIES_22_579K 1
//     int series;
// };

// struct pll_div
// {
//     unsigned int pll_in;
//     unsigned int pll_out;
//     int m;
//     int n_i;
//     int n_f;
// };

// static const struct kv_map codec_aif1_lrck[] = {
//     {16, 0},
//     {32, 1},
//     {64, 2},
//     {128, 3},
//     {256, 4},
// };

// static const struct kv_map codec_aif1_wsize[] = {
//     {8, 0},
//     {16, 1},
//     {20, 2},
//     {24, 3},
//     {32, 3},
// };

// static const struct aif1_fs codec_aif1_fs[] = {
//     {8000, 12, 0},
//     {11025, 8, 1, _SERIES_22_579K},
//     {12000, 8, 2},
//     {16000, 6, 3},
//     {22050, 4, 4, _SERIES_22_579K},
//     {24000, 4, 5},
//     /* {32000, 3, 6}, dividing by 3 is not support */
//     {44100, 2, 7, _SERIES_22_579K},
//     {48000, 2, 8},
//     {96000, 1, 9},
// };

// static const unsigned ac101_bclkdivs[] = {
//     1,
//     2,
//     4,
//     6,
//     8,
//     12,
//     16,
//     24,
//     32,
//     48,
//     64,
//     96,
//     128,
//     192,
//     0,
//     0,
// };

// /*
//  * Note : pll code from original tdm/i2s driver.
//  * freq_out = freq_in * N/(M*(2k+1)) , k=1,N=N_i+N_f,N_f=factor*0.2;
//  * 		N_i[0,1023], N_f_factor[0,7], m[1,64]=REG_VAL[1-63,0]
//  */
// static const struct pll_div codec_pll_div[] = {
//     {128000, _FREQ_22_579K, 1, 529, 1},
//     {192000, _FREQ_22_579K, 1, 352, 4},
//     {256000, _FREQ_22_579K, 1, 264, 3},
//     {384000, _FREQ_22_579K, 1, 176, 2}, /*((176+2*0.2)*6000000)/(38*(2*1+1))*/
//     {1411200, _FREQ_22_579K, 1, 48, 0},
//     {2822400, _FREQ_22_579K, 1, 24, 0},   /* accurate, 11025 * 256 */
//     {5644800, _FREQ_22_579K, 1, 12, 0},   /* accurate, 22050 * 256 */
//     {6000000, _FREQ_22_579K, 38, 429, 0}, /*((429+0*0.2)*6000000)/(38*(2*1+1))*/
//     {11289600, _FREQ_22_579K, 1, 6, 0},   /* accurate, 44100 * 256 */
//     {13000000, _FREQ_22_579K, 19, 99, 0},
//     {19200000, _FREQ_22_579K, 25, 88, 1},
//     {24000000, _FREQ_22_579K, 63, 177, 4}, /* 22577778 Hz */

//     {128000, _FREQ_24_576K, 1, 576, 0},
//     {192000, _FREQ_24_576K, 1, 384, 0},
//     {256000, _FREQ_24_576K, 1, 288, 0},
//     {384000, _FREQ_24_576K, 1, 192, 0},
//     {2048000, _FREQ_24_576K, 1, 36, 0}, /* accurate,  8000 * 256 */
//     {3072000, _FREQ_24_576K, 1, 24, 0}, /* accurate, 12000 * 256 */
//     {4096000, _FREQ_24_576K, 1, 18, 0}, /* accurate, 16000 * 256 */
//     {6000000, _FREQ_24_576K, 25, 307, 1},
//     {6144000, _FREQ_24_576K, 4, 48, 0},  /* accurate, 24000 * 256 */
//     {12288000, _FREQ_24_576K, 8, 48, 0}, /* accurate, 48000 * 256 */
//     {13000000, _FREQ_24_576K, 42, 238, 1},
//     {19200000, _FREQ_24_576K, 25, 96, 0},
//     {24000000, _FREQ_24_576K, 25, 76, 4}, /* accurate */

//     {_FREQ_22_579K, _FREQ_22_579K, 8, 24, 0}, /* accurate, 88200 * 256 */
//     {_FREQ_24_576K, _FREQ_24_576K, 8, 24, 0}, /* accurate, 96000 * 256 */
// };

// AC101::AC101(I2C *i2c, uint8_t device_address, PinName ampEnablePin) : enableAmp(ampEnablePin)
// {
//     this->i2c = i2c;
//     this->device_address = device_address;
// }

// bool AC101::checkDeviceOperation(Communication *communication_protocol)
// {
//     uint16_t response = readByte(0x00);

//     if (response == 0x101 && initialized)
//     { // 257
//         communication_protocol->sendDebugMessage("SUCCESS: AC101 found and functioning properly.\r\n");

//         return true;
//     }
//     else if (response == 0x101 && !initialized)
//     { // 257
//         communication_protocol->sendDebugMessage("ERROR: AC101 found but not functioning properly.\r\n");

//         return false;
//     }

//     communication_protocol->sendDebugMessage("ERROR: AC101 not found or not functioning properly.\r\n");

//     return false;
// }

// bool AC101::initialize(SampleRate sampleRate, WordSize WordSize, NumChannels numChannels)
// {
//     this->sampleRate = sampleRate;
//     this->wordSize = wordSize;
//     this->numChannels = numChannels;

//     bool ok = true;

//     // Reset all registers, readback default as sanity check
//     ok &= writeByte(CHIP_AUDIO_RS, 0x123);
//     HAL_Delay(100);
//     ok &= 0x0101 == readByte(CHIP_AUDIO_RS);

//     // Step 1: set_dai_fmt
//     set_dai_fmt();

//     // Step 2: hw_params
//     hw_params();

//     // Enalbe speakers:
//     ok &= writeByte(SPKOUT_CTRL, 0xeabd);

//     trigger(true);

//     aif1clk(true, 1);

//     // ok &= writeByte(SPKOUT_CTRL, 0xe880);

//     // // Clocking system
//     // ok &= writeByte(SYSCLK_CTRL, 0x8b08);
//     // ok &= writeByte(MOD_CLK_ENA, 0x800c);
//     // ok &= writeByte(MOD_RST_CTRL, 0x800c);

//     // // Set default at I2S, 44.1KHz, 16bit
//     // ok &= setI2sSampleRate(SAMPLE_RATE_44100);
//     // ok &= setI2sClock(BCLK_DIV_8, false, LRCK_DIV_32, false);
//     // ok &= setI2sMode(MODE_SLAVE);
//     // ok &= setI2sWordSize(WORD_SIZE_16_BITS);
//     // ok &= setI2sFormat(DATA_FORMAT_I2S);

//     // // AIF config
//     // ok &= writeByte(I2S1_SDOUT_CTRL, 0xc000);
//     // ok &= writeByte(I2S1_SDIN_CTRL, 0xc000);
//     // ok &= writeByte(I2S1_MXR_SRC, 0x2200);

//     // ok &= writeByte(ADC_SRCBST_CTRL, 0xccc4);
//     // ok &= writeByte(ADC_SRC, 0x2020);
//     // ok &= writeByte(ADC_DIG_CTRL, 0x8000);
//     // ok &= writeByte(ADC_APC_CTRL, 0xbbc3);

//     // // Path Configuration
//     // ok &= writeByte(DAC_MXR_SRC, 0xcc00);
//     // ok &= writeByte(DAC_DIG_CTRL, 0x8000);
//     // ok &= writeByte(OMIXER_SR, 0x0081);
//     // ok &= writeByte(OMIXER_DACA_CTRL, 0xf080);

//     // ok &= setMode( MODE_DAC );

//     // Enabling amplifier:
//     enableAmp.write(1);

//     initialized = true;

//     return ok;
// }

// bool AC101::switchHwConfig()
// {
//     bool ok = true;

//     uint8_t omixer_bst1_ctrl = readByte(OMIXER_BST1_CTRL);
//     uint8_t hmic_ctrl1 = readByte(HMIC_CTRL1);
//     uint8_t hmic_ctrl2 = readByte(HMIC_CTRL2);
//     uint8_t adc_apc_ctrl = readByte(ADC_APC_CTRL);
//     uint8_t spkout_ctrl = readByte(SPKOUT_CTRL);

//     Util::setBits(&omixer_bst1_ctrl, BIASVOLTAGE, 4, 0b1111); // HMIC/MMIC BIAS voltage level select:2.5v
//     Util::setBits(&hmic_ctrl1, HMIC_M, 4, 0b0000);            // debounce when Key down or keyup
//     Util::setBits(&hmic_ctrl1, HMIC_N, 4, 0b0000);            // debounce when earphone plugin or pullout
//     Util::setBits(&hmic_ctrl2, HMIC_SAMPLE_SELECT, 2, 0b10);  // Down Sample Setting Select: Downby 4,32Hz
//     Util::setBits(&hmic_ctrl2, HMIC_TH2, 5, 0b01000);         // Hmic_th2 for detecting Keydown or Keyup.
//     Util::setBits(&hmic_ctrl2, HMIC_TH1, 5, 0b00001);         // Hmic_th1[4:0],detecting eraphone plugin or pullout
//     Util::setBits(&adc_apc_ctrl, HBIASMOD, 1, 0b1);           // Headset microphone BIAS working mode: when HBIASEN = 1
//     Util::setBits(&adc_apc_ctrl, HBIASEN, 1, 0b1);            // Headset microphone BIAS Enable
//     Util::setBits(&adc_apc_ctrl, HBIASADCEN, 1, 0b1);         // Headset microphone BIAS Current sensor & ADC Enable
//     Util::setBits(&hmic_ctrl1, HMIC_PULLOUT_IRQ, 1, 0b1);     // Earphone Plugin/out Irq Enable
//     Util::setBits(&hmic_ctrl1, HMIC_PLUGIN_IRQ, 1, 0b1);      // Earphone Plugin/out Irq Enable
//     Util::setBits(&hmic_ctrl1, HMIC_KEYDOWN_IRQ, 1, 0b1);     // Hmic KeyUp/key down Irq Enable
//     Util::setBits(&hmic_ctrl1, HMIC_KEYUP_IRQ, 1, 0b1);       // Hmic KeyUp/key down Irq Enable
//     Util::setBits(&spkout_ctrl, HPCALICKS, 3, 0b111);         // headphone calibration clock frequency select

//     ok &= writeByte(omixer_bst1_ctrl, OMIXER_BST1_CTRL);
//     ok &= writeByte(hmic_ctrl1, HMIC_CTRL1);
//     ok &= writeByte(hmic_ctrl2, HMIC_CTRL2);
//     ok &= writeByte(adc_apc_ctrl, ADC_APC_CTRL);
//     ok &= writeByte(spkout_ctrl, SPKOUT_CTRL);
//     ok &= writeByte(HMIC_STS, HMIC_PEND_ALL); // Clear hmic interrupt

//     return ok;
// }

// bool AC101::set_dai_fmt()
// {
//     bool ok = true;

//     uint8_t aif1_clk_ctrl = readByte(I2S1_CLK_CTRL);

//     // Setting AC101 as master:
//     aif1_clk_ctrl &= ~(0x1 << AIF1_MSTR_MOD);
//     aif1_clk_ctrl |= (0x0 << AIF1_MSTR_MOD);

//     // Enable TDM:
//     aif1_clk_ctrl |= (0x1 << AIF1_TDMM_ENA);

//     // Writing and reading register (not sure why but still)
//     ok &= writeByte(I2S1_CLK_CTRL, aif1_clk_ctrl);
//     aif1_clk_ctrl = readByte(I2S1_CLK_CTRL);

//     // Set I2S mode:
//     aif1_clk_ctrl &= ~(3 << AIF1_DATA_FMT);
//     aif1_clk_ctrl |= (0x0 << AIF1_DATA_FMT);

//     // Writing and reading register (not sure why but still)
//     ok &= writeByte(I2S1_CLK_CTRL, aif1_clk_ctrl);
//     aif1_clk_ctrl = readByte(I2S1_CLK_CTRL);

//     // Set DAI signal inversions
//     aif1_clk_ctrl &= ~(0x1 << AIF1_LRCK_INV);
//     aif1_clk_ctrl &= ~(0x1 << AIF1_BCLK_INV);

//     ok &= writeByte(I2S1_CLK_CTRL, aif1_clk_ctrl);

//     return ok;
// }

// bool AC101::hw_params()
// {
//     bool ok = true;
//     uint8_t i = 0;
//     int reg_val;
//     int aif1_word_size = 24; //??

//     uint8_t aif1_clk_ctrl = readByte(I2S1_CLK_CTRL);
//     uint8_t aif_sr_ctrl = readByte(I2S_SR_CTRL);
//     uint8_t aif1_adcdat_ctrl = readByte(I2S1_SDOUT_CTRL);

//     // Set LRCK/BCLK ratio:
//     if (wordSize == WordSize::WORD_SIZE_24_BITS)
//     {
//         wordSize = WORD_SIZE_32_BITS;
//     }

//     uint8_t aif1_lrck_div = wordSize * numChannels;

//     for (i = 0; i < 5; i++)
//     {
//         if (codec_aif1_lrck[i].val == aif1_lrck_div)
//         {
//             break;
//         }
//     }

//     Util::setBits(&aif1_clk_ctrl, AIF1_LRCK_DIV, 3, codec_aif1_lrck[i].bit);

//     // Set PLL output freq:
//     int freq_out = _FREQ_24_576K;

//     for (i = 0; i < 9; i++)
//     {
//         if (codec_aif1_fs[i].samp_rate == sampleRate)
//         {
//             Util::setBits(&aif_sr_ctrl, AIF1_FS, 4, codec_aif1_fs[i].srbit);

//             if (codec_aif1_fs[i].series == _SERIES_22_579K)
//                 freq_out = _FREQ_22_579K;

//             break;
//         }
//     }

//     // Set I2S word size:
//     for (i = 0; i < 5; i++)
//     {
//         if (codec_aif1_wsize[i].val == aif1_word_size)
//         {
//             break;
//         }
//     }

//     Util::setBits(&aif1_clk_ctrl, AIF1_WORK_SIZ, 2, codec_aif1_wsize[i].bit);

//     // Set TDM slot size:
//     if ((reg_val = codec_aif1_wsize[i].bit) > 2)
//     {
//         reg_val = 2;
//     }

//     Util::setBits(&aif1_adcdat_ctrl, AIF1_SLOT_SIZ, 2, reg_val);

//     // setting pll if it's master mode (bit is 0 if master):
//     if ((aif1_clk_ctrl & (0x1 << AIF1_MSTR_MOD)) == 0)
//     {
//         ok &= set_pll(true, sysclk, freq_out);

//         int bclkdiv = freq_out / (aif1_lrck_div * sampleRate);

//         for (i = 0; i < 16; i++)
//         {
//             if (ac101_bclkdivs[i] >= bclkdiv)
//             {
//                 break;
//             }
//         }

//         Util::setBits(&aif1_clk_ctrl, AIF1_BCLK_DIV, 4, i);
//     }
//     else
//     {
//         ok &= set_pll(false, aif1_clk_ctrl * sampleRate, freq_out);
//     }

//     // Master mode, to clear cpu_dai fifos, disable output bclk & lrck:
//     aif1clk(false, 0);

//     ok &= writeByte(I2S1_CLK_CTRL, aif1_clk_ctrl);
//     ok &= writeByte(I2S_SR_CTRL, aif_sr_ctrl);
//     ok &= writeByte(I2S1_SDOUT_CTRL, aif1_adcdat_ctrl);

//     return ok;
// }

// bool AC101::set_pll(bool master, unsigned int freq_in, unsigned int freq_out)
// {
//     // Master == true (1), fasle (3)
//     bool ok = true;
//     int i = 0;

//     uint8_t sysclk_ctrl = readByte(SYSCLK_CTRL);
//     uint8_t pll_ctrl1 = readByte(PLL_CTRL1);
//     uint8_t pll_ctrl2 = readByte(PLL_CTRL2);

//     // clear volatile reserved bits:
//     // Util::setBits(&sysclk_ctrl, )

//     // Select aif1 clk srouce from mclk1
//     Util::setBits(&sysclk_ctrl, AIF1CLK_SRC, 2, 0b00);

//     // disable pll:
//     Util::setBits(&pll_ctrl2, PLL_EN, 1, 0b0);

//     // Checking frequency:
//     if (!freq_out)
//         ok &= false;
//     if ((freq_in < 128000) || (freq_in > _FREQ_24_576K))
//     {
//         ok &= false;
//     }
//     else if ((freq_in == _FREQ_24_576K) || (freq_in == _FREQ_22_579K))
//     {
//         if (master)
//         {
//             /*select aif1 clk source from mclk1*/
//             Util::setBits(&sysclk_ctrl, AIF1CLK_SRC, 2, 0b00);

//             ok &= false;
//         }
//     }
//     // Config PLL clk source:
//     if (master)
//     {
//         Util::setBits(&sysclk_ctrl, PLLCLK_SRC, 2, 0b00);
//     }
//     else
//     {
//         Util::setBits(&sysclk_ctrl, PLLCLK_SRC, 2, 0b10);
//     }

//     // Configuring PLL:
//     for (i = 0; i < 27; i++)
//     {
//         if ((codec_pll_div[i].pll_in == freq_in) && (codec_pll_div[i].pll_out == freq_out))
//         {
//             break;
//         }
//     }

//     uint8_t m = codec_pll_div[i].m == 64 ? 0 : codec_pll_div[i].m;

//     // Config m:
//     Util::setBits(&pll_ctrl1, PLL_POSTDIV_M, 6, m);

//     // Config n:
//     Util::setBits(&pll_ctrl2, PLL_PREDIV_NI, 10, codec_pll_div[i].n_i);
//     Util::setBits(&pll_ctrl2, PLL_POSTDIV_NF, 3, codec_pll_div[i].n_f);

//     // Enable pll:
//     Util::setBits(&pll_ctrl2, PLL_EN, 1, 0b1);
//     Util::setBits(&sysclk_ctrl, PLLCLK_ENA, 1, 0b1);
//     Util::setBits(&sysclk_ctrl, AIF1CLK_SRC, 2, 0b11);

//     ok &= writeByte(SYSCLK_CTRL, sysclk_ctrl);
//     ok &= writeByte(PLL_CTRL1, pll_ctrl1);
//     ok &= writeByte(PLL_CTRL2, pll_ctrl2);

//     return ok;
// }

// bool AC101::aif1clk(bool enable, int quick)
// {
//     bool ok = true;

//     uint8_t sysclk_ctrl = readByte(SYSCLK_CTRL);
//     uint8_t mod_clk_ena = readByte(MOD_CLK_ENA);
//     uint8_t mod_rst_ctrl = readByte(MOD_RST_CTRL);

//     if (enable)
//     {
//         if (aif1_clken == 0)
//         {
//             Util::setBits(&sysclk_ctrl, AIF1CLK_ENA, 1, 0b1);

//             if (!quick)
//             {
//                 /* enable aif1clk & sysclk */
//                 Util::setBits(&mod_clk_ena, MOD_CLK_AIF1, 1, 0b1);
//                 Util::setBits(&mod_clk_ena, MOD_RESET_AIF1, 1, 0b1);
//             }

//             Util::setBits(&sysclk_ctrl, SYSCLK_ENA, 1, 0b1);

//             printf("hw sysclk enable\n");
//             aif1_clken++;
//         }
//     }
//     else
//     {
//         if (aif1_clken != 0)
//         {
//             /* disable aif1clk & sysclk */
//             Util::setBits(&sysclk_ctrl, AIF1CLK_ENA, 1, 0b0);
//             Util::setBits(&mod_clk_ena, MOD_CLK_AIF1, 1, 0b0);
//             Util::setBits(&mod_clk_ena, MOD_RESET_AIF1, 1, 0b0);
//             Util::setBits(&sysclk_ctrl, SYSCLK_ENA, 1, 0b0);

//             printf("hw sysclk disable\n");
//             aif1_clken = 0;
//         }
//     }

//     ok &= writeByte(SYSCLK_CTRL, sysclk_ctrl);
//     ok &= writeByte(MOD_CLK_ENA, mod_clk_ena);
//     ok &= writeByte(MOD_RST_CTRL, mod_rst_ctrl);

//     return ok;
// }

// bool AC101::trigger(bool start)
// {
//     bool ok = true;

//     if (start && aif1_clken == 0)
//     {
//         uint8_t mod_clk_ena = readByte(MOD_CLK_ENA);
//         uint8_t mod_rst_ctrl = readByte(MOD_RST_CTRL);

//         Util::setBits(&mod_clk_ena, MOD_CLK_AIF1, 1, 0b1);
//         Util::setBits(&mod_rst_ctrl, MOD_RESET_AIF1, 1, 0b1);

//         ok &= writeByte(MOD_CLK_ENA, mod_clk_ena);
//         ok &= writeByte(MOD_RST_CTRL, mod_rst_ctrl);
//     }

//     return ok;
// }

// void AC101::printRegisters()
// {
//     for (size_t i = 0; i < 93; ++i)
//     {
//         uint16_t data = readByte(i);

//         printf("0x%x = (0x%x) ->", i, data);
//         Util::printAsBinary(data);
//     }
// }

// //****************************************
// //******** Change volumne functions ******
// //****************************************

// uint8_t AC101::getVolumeSpeaker()
// {
//     // Times 2, to scale to same range as headphone volume
//     return (readByte(SPKOUT_CTRL) & 31) * 2;
// }

// bool AC101::setVolumeSpeaker(uint8_t volume)
// {
//     // Divide by 2, as it is scaled to same range as headphone volume
//     volume /= 2;
//     if (volume > 31)
//         volume = 31;

//     uint16_t val = readByte(SPKOUT_CTRL);
//     val &= ~31;
//     val |= volume;
//     return writeByte(SPKOUT_CTRL, val);
// }

// uint8_t AC101::getVolumeHeadphone()
// {
//     return (readByte(HPOUT_CTRL) >> 4) & 63;
// }

// bool AC101::setVolumeHeadphone(uint8_t volume)
// {
//     if (volume > 63)
//         volume = 63;

//     uint16_t val = readByte(HPOUT_CTRL);
//     val &= ~63 << 4;
//     val |= volume << 4;
//     return writeByte(HPOUT_CTRL, val);
// }

// //****************************************
// //******** Change I2S settings ***********
// //****************************************

// bool AC101::setI2sSampleRate(I2sSampleRate_t rate)
// {
//     return writeByte(I2S_SR_CTRL, rate);
// }

// bool AC101::setI2sMode(I2sMode_t mode)
// {
//     uint16_t val = readByte(I2S1_CLK_CTRL);
//     val &= ~0x8000;
//     val |= uint16_t(mode) << 15;
//     return writeByte(I2S1_CLK_CTRL, val);
// }

// // bool AC101::setI2sWordSize(I2sWordSize_t size)
// // {
// //     uint16_t val = readByte(I2S1_CLK_CTRL);
// //     val &= ~0x0030;
// //     val |= uint16_t(size) << 4;
// //     return writeByte(I2S1_CLK_CTRL, val);
// // }

// bool AC101::setI2sFormat(I2sFormat_t format)
// {
//     uint16_t val = readByte(I2S1_CLK_CTRL);
//     val &= ~0x000C;
//     val |= uint16_t(format) << 2;
//     return writeByte(I2S1_CLK_CTRL, val);
// }

// bool AC101::setI2sClock(I2sBitClockDiv_t bitClockDiv, bool bitClockInv, I2sLrClockDiv_t lrClockDiv, bool lrClockInv)
// {
//     uint16_t val = readByte(I2S1_CLK_CTRL);
//     val &= ~0x7FC0;
//     val |= uint16_t(bitClockInv ? 1 : 0) << 14;
//     val |= uint16_t(bitClockDiv) << 9;
//     val |= uint16_t(lrClockInv ? 1 : 0) << 13;
//     val |= uint16_t(lrClockDiv) << 6;
//     return writeByte(I2S1_CLK_CTRL, val);
// }

// bool AC101::setMode(Mode_t mode)
// {
//     bool ok = true;

//     if (MODE_LINE == mode)
//     {
//         ok &= writeByte(ADC_SRC, 0x0408);
//         ok &= writeByte(ADC_DIG_CTRL, 0x8000);
//         ok &= writeByte(ADC_APC_CTRL, 0x3bc0);
//     }

//     if ((MODE_ADC == mode) or (MODE_ADC_DAC == mode) or (MODE_LINE == mode))
//     {
//         ok &= writeByte(MOD_CLK_ENA, 0x800c);
//         ok &= writeByte(MOD_RST_CTRL, 0x800c);
//     }

//     if ((MODE_DAC == mode) or (MODE_ADC_DAC == mode) or (MODE_LINE == mode))
//     {
//         // Enable Headphone output
//         ok &= writeByte(OMIXER_DACA_CTRL, 0xff80);
//         HAL_Delay(100);
//         ok &= writeByte(HPOUT_CTRL, 0xfbc0);
//         ok &= setVolumeHeadphone(30);

//         // Enable Speaker output
//         ok &= writeByte(SPKOUT_CTRL, 0xeabd);
//         HAL_Delay(10);
//         ok &= setVolumeSpeaker(30);
//     }

//     return ok;
// }

// //************************************
// //******** I2C helper functions ******
// //************************************

// /// @brief Read one byte from the requested I2C register.
// /// @param address Address of the register.
// /// @return Data currently in the register.
// uint16_t AC101::readByte(uint8_t address)
// {
//     char data[2];
//     char data_write[1];
//     data_write[0] = address;

//     i2c->write(device_address, data_write, 1, true);

//     i2c->read(device_address, data, 2, false);

//     return Util::to_ui16(data);
// }

// /// @brief Read multiple bytes from a specific I2C register.
// /// @param address Address of the register.
// /// @param count Number of bytes to read
// /// @param dest Array to store resulting data in.
// void AC101::readBytes(uint8_t address, uint8_t count, uint16_t *dest)
// {
//     char data[count];
//     char data_write[1];
//     data_write[0] = address;

//     i2c->write(device_address, data_write, 1, 1);

//     i2c->read(device_address, data, count, 0);

//     for (int i = 0; i < count; i += 2)
//     {
//         uint16_t ret = 0;
//         ret |= (data[i] << 8);
//         ret |= data[i + 1];

//         dest[i] = ret;
//     }
// }

// /// @brief Write one byte to a specific I2C register.
// /// @param address Address of the register.
// /// @param data Data to write to the register.
// bool AC101::writeByte(uint8_t address, uint16_t data)
// {
//     char data_write[3];
//     data_write[0] = address;

//     // Higher data byte:
//     data_write[1] = (data >> 8) & 0xFF;

//     // Lower data byte:
//     data_write[2] = data & 0xFF;

//     return i2c->write(device_address, data_write, 3, false) == 0;
// }
