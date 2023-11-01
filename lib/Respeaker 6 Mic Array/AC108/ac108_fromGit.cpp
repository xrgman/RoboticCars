#define TEST

#ifdef TEST1



/* AC108 definition */
#define AC108_CHANNELS_MAX		8		/* range[1, 16] */
#define AC108_RATES			(SNDRV_PCM_RATE_8000_96000 &		\
					~(SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_64000 | \
					SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000))
#define AC108_FORMATS			(/*SNDRV_PCM_FMTBIT_S16_LE | \
					SNDRV_PCM_FMTBIT_S20_3LE |   \
					SNDRV_PCM_FMTBIT_S24_LE |*/  \
					SNDRV_PCM_FMTBIT_S32_LE)

static const DECLARE_TLV_DB_SCALE(tlv_adc_pga_gain, 0, 100, 0);
static const DECLARE_TLV_DB_SCALE(tlv_ch_digital_vol, -11925,75,0);


/*
 * due to miss channels order in cpu_dai, we meed defer the clock starting.
 */
static int ac108_set_clock(int y_start_n_stop, struct snd_pcm_substream *substream, int cmd, struct snd_soc_dai *dai) {
	u8 reg;
	int ret = 0;

	dev_dbg(ac10x->codec->dev, "%s() L%d cmd:%d\n", __func__, __LINE__, y_start_n_stop);

	/* spin_lock move to machine trigger */



	if (y_start_n_stop && ac10x->sysclk_en == 0) {
		/* enable lrck clock */
		ac10x_read(I2S_CTRL, &reg, ac10x->i2cmap[_MASTER_INDEX]);
		if (reg & (0x01 << BCLK_IOEN)) {
			ret = ret || ac10x_update_bits(I2S_CTRL, 0x03 << LRCK_IOEN, 0x03 << LRCK_IOEN, ac10x->i2cmap[_MASTER_INDEX]);
		}

		/*0x10: PLL Common voltage enable, PLL enable */
		ret = ret || ac108_multi_update_bits(PLL_CTRL1, 0x01 << PLL_EN | 0x01 << PLL_COM_EN,
						   0x01 << PLL_EN | 0x01 << PLL_COM_EN, ac10x);
		/* enable global clock */
		ret = ret || ac108_multi_update_bits(I2S_CTRL, 0x1 << TXEN | 0x1 << GEN, 0x1 << TXEN | 0x1 << GEN, ac10x);

		ac10x->sysclk_en = 1UL;
	} else if (!y_start_n_stop && ac10x->sysclk_en != 0) {
		/* disable global clock */
		ret = ret || ac108_multi_update_bits(I2S_CTRL, 0x1 << TXEN | 0x1 << GEN, 0x0 << TXEN | 0x0 << GEN, ac10x);

		/*0x10: PLL Common voltage disable, PLL disable */
		ret = ret || ac108_multi_update_bits(PLL_CTRL1, 0x01 << PLL_EN | 0x01 << PLL_COM_EN,
						   0x00 << PLL_EN | 0x00 << PLL_COM_EN, ac10x);

		/* disable lrck clock if it's enabled */
		ac10x_read(I2S_CTRL, &reg, ac10x->i2cmap[_MASTER_INDEX]);
		if (reg & (0x01 << LRCK_IOEN)) {
			ret = ret || ac10x_update_bits(I2S_CTRL, 0x03 << LRCK_IOEN, 0x01 << BCLK_IOEN, ac10x->i2cmap[_MASTER_INDEX]);
		}
		if (!ret) {
			ac10x->sysclk_en = 0UL;
		}
	}

	return ret;
}

static int ac108_trigger(struct snd_pcm_substream *substream, int cmd,
			     struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct ac10x_priv *ac10x = snd_soc_codec_get_drvdata(codec);
	unsigned long flags;
	int ret = 0;
	u8 r;

	dev_dbg(dai->dev, "%s() stream=%s  cmd=%d\n",
		__FUNCTION__,
		snd_pcm_stream_str(substream),
		cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		spin_lock_irqsave(&ac10x->lock, flags);
		/* disable global clock if lrck disabled */
		ac10x_read(I2S_CTRL, &r, ac10x->i2cmap[_MASTER_INDEX]);
		if ((r & (0x01 << BCLK_IOEN)) && (r & (0x01 << LRCK_IOEN)) == 0) {
			/* disable global clock */
			ac108_multi_update_bits(I2S_CTRL, 0x1 << TXEN | 0x1 << GEN, 0x0 << TXEN | 0x0 << GEN, ac10x);
		}
		spin_unlock_irqrestore(&ac10x->lock, flags);

		/* delayed clock starting, move to machine trigger() */
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (ac10x->i2c101 && _MASTER_MULTI_CODEC == _MASTER_AC101) {
			ac101_trigger(substream, cmd, dai);
		}
		break;
	default:
		ret = -EINVAL;
	}

	dev_dbg(dai->dev, "%s() stream=%s  cmd=%d; finished %d\n",
		__FUNCTION__,
		snd_pcm_stream_str(substream),
		cmd, ret);
	return ret;
}



static int ac108_i2c_probe(struct i2c_client *i2c, const struct i2c_device_id *i2c_id) {
	struct device_node *np = i2c->dev.of_node;
	unsigned int val = 0;
	int ret = 0, index;

	if (ac10x == NULL) {
		ac10x = kzalloc(sizeof(struct ac10x_priv), GFP_KERNEL);
		if (ac10x == NULL) {
			dev_err(&i2c->dev, "Unable to allocate ac10x private data\n");
			return -ENOMEM;
		}
	}

	index = (int)i2c_id->driver_data;
	if (index == AC101_I2C_ID) {
		ac10x->i2c101 = i2c;
		i2c_set_clientdata(i2c, ac10x);
		ret = ac101_probe(i2c, i2c_id);
		if (ret) {
			ac10x->i2c101 = NULL;
			return ret;
		}
		goto __ret;
	}

	ret = of_property_read_u32(np, "data-protocol", &val);
	if (ret) {
		pr_err("Please set data-protocol.\n");
		return -EINVAL;
	}
	ac10x->data_protocol = val;

	if (of_property_read_u32(np, "tdm-chips-count", &val)) val = 1;
	ac10x->tdm_chips_cnt = val;

	pr_info(" ac10x i2c_id number: %d\n", index);
	pr_info(" ac10x data protocol: %d\n", ac10x->data_protocol);

	ac10x->i2c[index] = i2c;
	ac10x->i2cmap[index] = devm_regmap_init_i2c(i2c, &ac108_regmap);
	if (IS_ERR(ac10x->i2cmap[index])) {
		ret = PTR_ERR(ac10x->i2cmap[index]);
		dev_err(&i2c->dev, "Fail to initialize i2cmap%d I/O: %d\n", index, ret);
		return ret;
	}

	/*
	 * Writing this register with 0x12 
	 * will resets all register to their default state.
	 */
	regcache_cache_only(ac10x->i2cmap[index], false);
	ret = regmap_write(ac10x->i2cmap[index], CHIP_RST, CHIP_RST_VAL);
	msleep(1);

	/* sync regcache for FLAT type */
	ac10x_fill_regcache(&i2c->dev, ac10x->i2cmap[index]);

	ac10x->codec_cnt++;
	pr_info(" ac10x codec count  : %d\n", ac10x->codec_cnt);

	ret = sysfs_create_group(&i2c->dev.kobj, &ac108_debug_attr_group);
	if (ret) {
		pr_err("failed to create attr group\n");
	}

__ret:
	/* It's time to bind codec to i2c[_MASTER_INDEX] when all i2c are ready */
	if ((ac10x->codec_cnt != 0 && ac10x->tdm_chips_cnt < 2)
	|| (ac10x->i2c[0] && ac10x->i2c[1] && ac10x->i2c101)) {
		seeed_voice_card_register_set_clock(SNDRV_PCM_STREAM_CAPTURE, ac108_set_clock);
		/* no playback stream */
		if (! ac10x->i2c101) {
			memset(&ac108_dai[_MASTER_INDEX]->playback, '\0', sizeof ac108_dai[_MASTER_INDEX]->playback);
		}
		ret = snd_soc_register_codec(&ac10x->i2c[_MASTER_INDEX]->dev, &ac10x_soc_codec_driver,
						ac108_dai[_MASTER_INDEX], 1);
		if (ret < 0) {
			dev_err(&i2c->dev, "Failed to register ac10x codec: %d\n", ret);
		}
	}
	return ret;
}

#endif