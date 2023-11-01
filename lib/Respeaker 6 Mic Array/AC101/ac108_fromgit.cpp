#define HUP

#ifdef HUP1


void drc_config(struct snd_soc_codec *codec)
{
	int reg_val;
	reg_val = ac101_read(codec, 0xa3);
	reg_val &= ~(0x7ff<<0);
	reg_val |= 1<<0;
	ac101_write(codec, 0xa3, reg_val);
	ac101_write(codec, 0xa4, 0x2baf);

	reg_val = ac101_read(codec, 0xa5);
	reg_val &= ~(0x7ff<<0);
	reg_val |= 1<<0;
	ac101_write(codec, 0xa5, reg_val);
	ac101_write(codec, 0xa6, 0x2baf);

	reg_val = ac101_read(codec, 0xa7);
	reg_val &= ~(0x7ff<<0);
	ac101_write(codec, 0xa7, reg_val);
	ac101_write(codec, 0xa8, 0x44a);

	reg_val = ac101_read(codec, 0xa9);
	reg_val &= ~(0x7ff<<0);
	ac101_write(codec, 0xa9, reg_val);
	ac101_write(codec, 0xaa, 0x1e06);

	reg_val = ac101_read(codec, 0xab);
	reg_val &= ~(0x7ff<<0);
	reg_val |= (0x352<<0);
	ac101_write(codec, 0xab, reg_val);
	ac101_write(codec, 0xac, 0x6910);

	reg_val = ac101_read(codec, 0xad);
	reg_val &= ~(0x7ff<<0);
	reg_val |= (0x77a<<0);
	ac101_write(codec, 0xad, reg_val);
	ac101_write(codec, 0xae, 0xaaaa);

	reg_val = ac101_read(codec, 0xaf);
	reg_val &= ~(0x7ff<<0);
	reg_val |= (0x2de<<0);
	ac101_write(codec, 0xaf, reg_val);
	ac101_write(codec, 0xb0, 0xc982);

	ac101_write(codec, 0x16, 0x9f9f);

}

void drc_enable(struct snd_soc_codec *codec,bool on)
{
	int reg_val;
	if (on) {
		ac101_write(codec, 0xb5, 0xA080);
		reg_val = ac101_read(codec, MOD_CLK_ENA);
		reg_val |= (0x1<<6);
		ac101_write(codec, MOD_CLK_ENA, reg_val);
		reg_val = ac101_read(codec, MOD_RST_CTRL);
		reg_val |= (0x1<<6);
		ac101_write(codec, MOD_RST_CTRL, reg_val);

		reg_val = ac101_read(codec, 0xa0);
		reg_val |= (0x7<<0);
		ac101_write(codec, 0xa0, reg_val);
	} else {
		ac101_write(codec, 0xb5, 0x0);
		reg_val = ac101_read(codec, MOD_CLK_ENA);
		reg_val &= ~(0x1<<6);
		ac101_write(codec, MOD_CLK_ENA, reg_val);
		reg_val = ac101_read(codec, MOD_RST_CTRL);
		reg_val &= ~(0x1<<6);
		ac101_write(codec, MOD_RST_CTRL, reg_val);

		reg_val = ac101_read(codec, 0xa0);
		reg_val &= ~(0x7<<0);
		ac101_write(codec, 0xa0, reg_val);
	}
}

void set_configuration(struct snd_soc_codec *codec)
{
	if (speaker_double_used) {
		ac101_update_bits(codec, SPKOUT_CTRL, (0x1f<<SPK_VOL), (double_speaker_val<<SPK_VOL));
	} else {
		ac101_update_bits(codec, SPKOUT_CTRL, (0x1f<<SPK_VOL), (single_speaker_val<<SPK_VOL));
	}
	ac101_update_bits(codec, HPOUT_CTRL, (0x3f<<HP_VOL), (headset_val<<HP_VOL));
	ac101_update_bits(codec, ADC_SRCBST_CTRL, (0x7<<ADC_MIC1G), (mainmic_val<<ADC_MIC1G));
	ac101_update_bits(codec, ADC_SRCBST_CTRL, (0x7<<ADC_MIC2G), (headsetmic_val<<ADC_MIC2G));
	if (dmic_used) {
		ac101_write(codec, ADC_VOL_CTRL, adc_digital_val);
	}
	if (drc_used) {
		drc_config(codec);
	}
	/*headphone calibration clock frequency select*/
	ac101_update_bits(codec, SPKOUT_CTRL, (0x7<<HPCALICKS), (0x7<<HPCALICKS));

	/* I2S1 DAC Timeslot 0 data <- I2S1 DAC channel 0 */
	// "AIF1IN0L Mux" <= "AIF1DACL"
	// "AIF1IN0R Mux" <= "AIF1DACR"
	ac101_update_bits(codec, AIF1_DACDAT_CTRL, 0x3 << AIF1_DA0L_SRC, 0x0 << AIF1_DA0L_SRC);
	ac101_update_bits(codec, AIF1_DACDAT_CTRL, 0x3 << AIF1_DA0R_SRC, 0x0 << AIF1_DA0R_SRC);
	/* Timeslot 0 Left & Right Channel enable */
	ac101_update_bits(codec, AIF1_DACDAT_CTRL, 0x3 << AIF1_DA0R_ENA, 0x3 << AIF1_DA0R_ENA);

	/* DAC Digital Mixer Source Select <- I2S1 DA0 */
	// "DACL Mixer"	+= "AIF1IN0L Mux"
	// "DACR Mixer" += "AIF1IN0R Mux"
	ac101_update_bits(codec, DAC_MXR_SRC, 0xF << DACL_MXR_ADCL, 0x8 << DACL_MXR_ADCL);
	ac101_update_bits(codec, DAC_MXR_SRC, 0xF << DACR_MXR_ADCR, 0x8 << DACR_MXR_ADCR);
	/* Internal DAC Analog Left & Right Channel enable */
	ac101_update_bits(codec, OMIXER_DACA_CTRL, 0x3 << DACALEN, 0x3 << DACALEN);

	/* Output Mixer Source Select */
	// "Left Output Mixer"  += "DACL Mixer"
	// "Right Output Mixer" += "DACR Mixer"
	ac101_update_bits(codec, OMIXER_SR, 0x1 << LMIXMUTEDACL, 0x1 << LMIXMUTEDACL);
	ac101_update_bits(codec, OMIXER_SR, 0x1 << RMIXMUTEDACR, 0x1 << RMIXMUTEDACR);
	/* Left & Right Analog Output Mixer enable */
	ac101_update_bits(codec, OMIXER_DACA_CTRL, 0x3 << LMIXEN, 0x3 << LMIXEN);

	/* Headphone Ouput Control */ 
	// "HP_R Mux" <= "DACR Mixer"
	// "HP_L Mux" <= "DACL Mixer"
	ac101_update_bits(codec, HPOUT_CTRL, 0x1 << LHPS, 0x0 << LHPS);
	ac101_update_bits(codec, HPOUT_CTRL, 0x1 << RHPS, 0x0 << RHPS);

	/* Speaker Output Control */
	// "SPK_L Mux" <= "SPK_LR Adder"
	// "SPK_R Mux" <= "SPK_LR Adder"
	ac101_update_bits(codec, SPKOUT_CTRL, (0x1 << LSPKS) | (0x1 << RSPKS), (0x1 << LSPKS) | (0x1 << RSPKS));
	/* Enable Left & Right Speaker */
	ac101_update_bits(codec, SPKOUT_CTRL, (0x1 << LSPK_EN) | (0x1 << RSPK_EN), (0x1 << LSPK_EN) | (0x1 << RSPK_EN));
	return;
}

static int late_enable_dac(struct snd_soc_codec* codec, int event) {
	struct ac10x_priv *ac10x = snd_soc_codec_get_drvdata(codec);

	mutex_lock(&ac10x->dac_mutex);
	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		AC101_DBG();
		if (ac10x->dac_enable == 0){
			/*enable dac module clk*/
			ac101_update_bits(codec, MOD_CLK_ENA, (0x1<<MOD_CLK_DAC_DIG), (0x1<<MOD_CLK_DAC_DIG));
			ac101_update_bits(codec, MOD_RST_CTRL, (0x1<<MOD_RESET_DAC_DIG), (0x1<<MOD_RESET_DAC_DIG));
			ac101_update_bits(codec, DAC_DIG_CTRL, (0x1<<ENDA), (0x1<<ENDA));
			ac101_update_bits(codec, DAC_DIG_CTRL, (0x1<<ENHPF),(0x1<<ENHPF));
		}
		ac10x->dac_enable++;
		break;
	case SND_SOC_DAPM_POST_PMD:
		if (ac10x->dac_enable != 0){
			ac10x->dac_enable = 0;

			ac101_update_bits(codec, DAC_DIG_CTRL, (0x1<<ENHPF),(0x0<<ENHPF));
			ac101_update_bits(codec, DAC_DIG_CTRL, (0x1<<ENDA), (0x0<<ENDA));
			/*disable dac module clk*/
			ac101_update_bits(codec, MOD_CLK_ENA, (0x1<<MOD_CLK_DAC_DIG), (0x0<<MOD_CLK_DAC_DIG));
			ac101_update_bits(codec, MOD_RST_CTRL, (0x1<<MOD_RESET_DAC_DIG), (0x0<<MOD_RESET_DAC_DIG));
		}
		break;
	}
	mutex_unlock(&ac10x->dac_mutex);
	return 0;
}

static int ac101_headphone_event(struct snd_soc_codec* codec, int event) {
	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		/*open*/
		AC101_DBG("post:open\n");
		ac101_update_bits(codec, OMIXER_DACA_CTRL, (0xf<<HPOUTPUTENABLE), (0xf<<HPOUTPUTENABLE));
		msleep(10);
		ac101_update_bits(codec, HPOUT_CTRL, (0x1<<HPPA_EN), (0x1<<HPPA_EN));
		ac101_update_bits(codec, HPOUT_CTRL, (0x3<<LHPPA_MUTE), (0x3<<LHPPA_MUTE));
		break;
	case SND_SOC_DAPM_PRE_PMD:
		/*close*/
		AC101_DBG("pre:close\n");
		ac101_update_bits(codec, HPOUT_CTRL, (0x3<<LHPPA_MUTE), (0x0<<LHPPA_MUTE));
		msleep(10);
		ac101_update_bits(codec, OMIXER_DACA_CTRL, (0xf<<HPOUTPUTENABLE), (0x0<<HPOUTPUTENABLE));
		ac101_update_bits(codec, HPOUT_CTRL, (0x1<<HPPA_EN), (0x0<<HPPA_EN));
		break;
	}
	return 0;
}

static int ac101_sysclk_started(void) {
	int reg_val;

	reg_val = ac101_read(static_ac10x->codec, SYSCLK_CTRL);
	return (reg_val & (0x1<<SYSCLK_ENA));
}

static int ac101_aif_play(struct ac10x_priv* ac10x) {
	struct snd_soc_codec * codec = ac10x->codec;

	late_enable_dac(codec, SND_SOC_DAPM_PRE_PMU);
	ac101_headphone_event(codec, SND_SOC_DAPM_POST_PMU);
	if (drc_used) {
		drc_enable(codec, 1);
	}

	/* Enable Left & Right Speaker */
	ac101_update_bits(codec, SPKOUT_CTRL, (0x1 << LSPK_EN) | (0x1 << RSPK_EN), (0x1 << LSPK_EN) | (0x1 << RSPK_EN));
	if (ac10x->gpiod_spk_amp_gate) {
		gpiod_set_value(ac10x->gpiod_spk_amp_gate, 1);
	}
	return 0;
}



#if _MASTER_MULTI_CODEC == _MASTER_AC101
static int ac101_set_clock(int y_start_n_stop, struct snd_pcm_substream *substream, int cmd, struct snd_soc_dai *dai) {
	int r;

	if (y_start_n_stop) {
		/* enable global clock */
		r = ac101_aif1clk(static_ac10x->codec, SND_SOC_DAPM_PRE_PMU, 1);
	} else {
		/* disable global clock */
		static_ac10x->aif1_clken = 1;
		r = ac101_aif1clk(static_ac10x->codec, SND_SOC_DAPM_POST_PMD, 0);
	}
	return r;
}
#endif

static void codec_resume_work(struct work_struct *work)
{
	struct ac10x_priv *ac10x = container_of(work, struct ac10x_priv, codec_resume);
	struct snd_soc_codec *codec = ac10x->codec;

	AC101_DBG("+++\n");

	set_configuration(codec);
	if (drc_used) {
		drc_config(codec);
	}
	/*enable this bit to prevent leakage from ldoin*/
	ac101_update_bits(codec, ADDA_TUNE3, (0x1<<OSCEN), (0x1<<OSCEN));

	AC101_DBG("---\n");
	return;
}

int ac101_set_bias_level(struct snd_soc_codec *codec, enum snd_soc_bias_level level)
{
	switch (level) {
	case SND_SOC_BIAS_ON:
		AC101_DBG("SND_SOC_BIAS_ON\n");
		break;
	case SND_SOC_BIAS_PREPARE:
		AC101_DBG("SND_SOC_BIAS_PREPARE\n");
		break;
	case SND_SOC_BIAS_STANDBY:
		AC101_DBG("SND_SOC_BIAS_STANDBY\n");
		#ifdef CONFIG_AC101_SWITCH_DETECT
		switch_hw_config(codec);
		#endif
		break;
	case SND_SOC_BIAS_OFF:
		#ifdef CONFIG_AC101_SWITCH_DETECT
		ac101_update_bits(codec, ADC_APC_CTRL, (0x1<<HBIASEN), (0<<HBIASEN));
		ac101_update_bits(codec, ADC_APC_CTRL, (0x1<<HBIASADCEN), (0<<HBIASADCEN));
		#endif
		ac101_update_bits(codec, OMIXER_DACA_CTRL, (0xf<<HPOUTPUTENABLE), (0<<HPOUTPUTENABLE));
		ac101_update_bits(codec, ADDA_TUNE3, (0x1<<OSCEN), (0<<OSCEN));
		AC101_DBG("SND_SOC_BIAS_OFF\n");
		break;
	}
	snd_soc_codec_get_dapm(codec)->bias_level = level;
	return 0;
}



#endif