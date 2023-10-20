#include "sdWrapper.h"
#include "I2S.h"

/// @brief Constructor.
/// @param mosi MOSI pin SD card.
/// @param miso MISO pin SD card.
/// @param clk CLK pin SD card.
/// @param nss CS pin SD card.
/// @param comm Reference to communication object.
SDWrapper::SDWrapper(PinName mosi, PinName miso, PinName clk, PinName nss, Communication *comm) : sd(mosi, miso, clk, nss), fs("sd")
{
    communication_protocol = comm;
}

/// @brief Initialize the SD card.
void SDWrapper::initialize()
{
    // Setting max frequency for optimal playback performance:
    sd.frequency(25000000);
}


bool SDWrapper::playWavFile(const char *filename, Callback<void(SampleRate sampleRate, WordSize wordSize, uint8_t channels)> configureCallback, Callback<void(uint16_t *data, uint16_t size)> writeCallback)
{
    // Mounting file system and checking if it was successfull:
    if (!mountFileSystem())
    {
        return false;
    }

    // Opening file and checking if it was successfull:
    if (!openFile(filename))
    {
        return false;
    }

    //Setting write callback:
    this->writeCallback = writeCallback;

    // Reading WAV header:
    uint8_t header[44];
    fread(header, 1, 44, file);

    printWAVHeader(header);

    //Substracting WAV file properties needed for configuration:
    uint32_t sampleRate = *(uint32_t *)(&header[24]);
    uint16_t bitsPerSample = *(uint16_t *)(&header[34]);
    uint16_t numChannels = *(uint16_t *)(&header[22]);

    //TODO configure it :)
    configureCallback(getSampleRate(sampleRate), getWordSize(bitsPerSample), (uint8_t)numChannels);

    fileStarted = true;

    return true;
}

void SDWrapper::signalBatchDone() {
    playing = false;
}

void SDWrapper::run()
{
    if (fileStarted && !playing)
    {
        playing = true;

        // Block size (adjust as needed)
        uint16_t audioData[I2S_BLOCK_SIZE / 2];

        // Read and transmit audio data in blocks
        if (!feof(file))
        {
            size_t bytesRead = fread((char *)audioData, 1, I2S_BLOCK_SIZE, file);

            // respeaker.writeSpeaker(audioData, (uint16_t)(bytesRead/2));
            writeCallback(audioData, (uint16_t)(bytesRead / 2));
        }
        else
        {
            communication_protocol->sendDebugMessage("Closing file.\n");
            fclose(file);

            fs.unmount();

            playing = false;
            fileStarted = false;
        }
    }
}

/// @brief Mounts the file system.
/// @return Whether or not the file system was successfully mounted.
bool SDWrapper::mountFileSystem()
{
    uint8_t success = fs.mount(&sd);

    if (success != 0)
    {
        communication_protocol->sendDebugMessage("File system not found!\n");

        return false;
    }

    return true;
}

/// @brief Open the file from the sd card.
/// @param filename Name of the file to open.
/// @return Whether the file was found and successfully opened.
bool SDWrapper::openFile(const char *filename)
{
    file = fopen(filename, "r");

    if (!file)
    {
        char msg[100];
        snprintf(msg, sizeof(msg), "File with filename %s not found.\n", filename);
        communication_protocol->sendDebugMessage(msg);

        return false;
    }

    return true;
}

/// @brief Print the contents of the header of a WAV file.
/// @param header Array containing header.
void SDWrapper::printWAVHeader(const uint8_t header[44])
{
    // "RIFF" chunk descriptor
    printf("ChunkID (RIFF): %c%c%c%c\n", header[0], header[1], header[2], header[3]);

    // File size excluding ChunkID and ChunkSize
    uint32_t fileSize = *(uint32_t *)(&header[4]);
    printf("ChunkSize (File size): %u bytes\n", fileSize);

    // "WAVE" format descriptor
    printf("Format (WAVE): %c%c%c%c\n", header[8], header[9], header[10], header[11]);

    // "fmt " subchunk
    printf("Subchunk1ID (fmt): %c%c%c%c\n", header[12], header[13], header[14], header[15]);

    // Size of the format data (16 bytes for PCM)
    uint32_t fmtSize = *(uint32_t *)(&header[16]);
    printf("Subchunk1Size (Size of format data): %u bytes\n", fmtSize);

    // Audio format (1 for PCM)
    uint16_t audioFormat = *(uint16_t *)(&header[20]);
    printf("AudioFormat (Audio format): %u (1 for PCM)\n", audioFormat);

    // Number of channels (1 for mono, 2 for stereo, etc.)
    uint16_t numChannels = *(uint16_t *)(&header[22]);
    printf("NumChannels (Number of channels): %u\n", numChannels);

    // Sample rate (e.g., 44100 Hz)
    uint32_t sampleRate = *(uint32_t *)(&header[24]);
    printf("SampleRate (Sample rate): %u Hz\n", sampleRate);

    // Byte rate (SampleRate * NumChannels * BitsPerSample / 8)
    uint32_t byteRate = *(uint32_t *)(&header[28]);
    printf("ByteRate (Byte rate): %u bytes/second\n", byteRate);

    // Block align (NumChannels * BitsPerSample / 8)
    uint16_t blockAlign = *(uint16_t *)(&header[32]);
    printf("BlockAlign (Block align): %u bytes\n", blockAlign);

    // Bits per sample (e.g., 16 bits)
    uint16_t bitsPerSample = *(uint16_t *)(&header[34]);
    printf("BitsPerSample (Bits per sample): %u bits\n", bitsPerSample);

    // "data" subchunk
    printf("Subchunk2ID (data): %c%c%c%c\n", header[36], header[37], header[38], header[39]);

    // Subchunk2Size (Size of the data)
    uint32_t dataSubchunkSize = *(uint32_t *)(&header[40]);
    printf("Subchunk2Size (Size of data): %u bytes\n", dataSubchunkSize);
}