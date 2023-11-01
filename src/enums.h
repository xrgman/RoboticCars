#ifndef ENUMS_H
#define ENUMS_H

typedef enum
{
    SAMPLE_RATE_CURRENT = 0, // Used to define that it doesn't need to be changed.
    SAMPLE_RATE_8000 = 8000,
    SAMPLE_RATE_11052 = 11052,
    SAMPLE_RATE_16000 = 16000,
    SAMPLE_RATE_22050 = 22050,
    SAMPLE_RATE_32000 = 32000,
    SAMPLE_RATE_44100 = 44100,
    SAMPLE_RATE_48000 = 48000,
    SAMPLE_RATE_96000 = 96000,
    SAMPLE_RATE_192000 = 192000
} SampleRate;

typedef enum
{
    WORD_SIZE_CURRENT = 0, // Used to define that it doesn't need to be changed.
    WORD_SIZE_8_BITS = 8,
    WORD_SIZE_16_BITS = 16,
    WORD_SIZE_20_BITS = 20,
    WORD_SIZE_24_BITS = 24,
    WORD_SIZE_32_BITS = 32
} WordSize;

typedef enum
{
    NUM_CHANNELS_CURRENT = 0, // Used to define that it doesn't need to be changed.
    NUM_CHANNELS_2 = 2,
    NUM_CHANNELS_4 = 4,
    NUM_CHANNELS_6 = 6,
    NUM_CHANNELS_8 = 8
} NumChannels;

/// @brief Get the sample rate enum value for a given raw sample rate value.
/// @param sampleRateRaw The raw sample rate.
/// @return Concrete sample rate associated with the raw sample rate (rounded up).
static SampleRate getSampleRate(uint32_t sampleRateRaw)
{
    if (sampleRateRaw <= 8000)
    {
        return SAMPLE_RATE_8000;
    }

    if (sampleRateRaw <= 11052)
    {
        return SAMPLE_RATE_11052;
    }

    if (sampleRateRaw <= 16000)
    {
        return SAMPLE_RATE_16000;
    }

    if (sampleRateRaw <= 22050)
    {
        return SAMPLE_RATE_22050;
    }

    if (sampleRateRaw <= 32000)
    {
        return SAMPLE_RATE_32000;
    }

    if (sampleRateRaw <= 44100)
    {
        return SAMPLE_RATE_44100;
    }

    if (sampleRateRaw <= 48000)
    {
        return SAMPLE_RATE_48000;
    }

    if (sampleRateRaw <= 96000)
    {
        return SAMPLE_RATE_96000;
    }

    return SAMPLE_RATE_192000;
}

/// @brief Get the word size enum value for a given raw word size value.
/// @param wordSizeRaw The raw word size (int bits).
/// @return Concrete word size associated with the raw word size (rounded up).
static WordSize getWordSize(uint16_t wordSizeRaw)
{
    if (wordSizeRaw <= 8)
    {
        return WORD_SIZE_8_BITS;
    }

    if (wordSizeRaw <= 16)
    {
        return WORD_SIZE_16_BITS;
    }

    if (wordSizeRaw <= 20)
    {
        return WORD_SIZE_20_BITS;
    }

    if (wordSizeRaw <= 24)
    {
        return WORD_SIZE_24_BITS;
    }

    return WORD_SIZE_32_BITS;
}

#endif