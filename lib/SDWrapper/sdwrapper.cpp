#include "sdWrapper.h"
#include "I2S.h"

uint8_t audioData[128 * 8];

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

/// @brief Play WAV file over speaker.
/// @param filename Name of the WAV file, including the whole path (so including "/sd/x.wav")
/// @param configureCallback 
/// @param writeCallback Callback to the I2S writing.
/// @return Whether or not reading the file and starting the write sequence was successfull.
bool SDWrapper::playWavFile(const char *filename, Callback<void(SampleRate sampleRate, WordSize wordSize, uint8_t channels)> configureCallback, Callback<void(uint16_t *data, uint16_t size)> writeCallback)
{
    // Checking if another file is not playing:
    if (fileReadStarted)
    {
        communication_protocol->sendDebugMessage("Unable to play WAV file, another file is already playing.\n");

        return false;
    }

    // Mounting file system and checking if it was successfull:
    if (!mountFileSystem())
    {
        return false;
    }

    // Opening file and checking if it was successfull:
    if (!openFile(filename, &fileRead, "r"))
    {
        char msg[100];
        snprintf(msg, sizeof(msg), "File with filename %s not found.\n", filename);
        communication_protocol->sendDebugMessage(msg);

        return false;
    }

    // Setting write callback:
    this->writeCallback = writeCallback;

    // Reading WAV header:
    uint8_t header[44];
    fread(header, 1, 44, fileRead);

    WAVHeader wavHeader = createWavHeaderFromFile(header);

    printWAVHeader(wavHeader);

    // Substracting WAV file properties needed for configuration:
    uint32_t sampleRate = wavHeader.sampleRate;
    uint16_t bitsPerSample = wavHeader.bitsPerSample;
    uint16_t numChannels = wavHeader.numChannels;

    //configureCallback(getSampleRate(sampleRate), getWordSize(bitsPerSample), (uint8_t)numChannels);

    fileReadStarted = true;

    return true;
}

bool SDWrapper::recordToWavFile(const char *filename, uint8_t secondsToWrite, Callback<void(uint8_t *data, uint16_t size)> readCallback)
{
    // Mounting file system and checking if it was successfull:
    if (!mountFileSystem())
    {
        return false;
    }

    // Check if file already exists:
    // if (openFile(filename, &fileWrite, "r"))
    // {
    //     char msg[100];
    //     snprintf(msg, sizeof(msg), "File with filename %s already exists, unable to start recording.\n", filename);
    //     communication_protocol->sendDebugMessage(msg);

    //     return false;
    // }

    // Create file:
    if (!openFile(filename, &fileWrite, "w"))
    {
        char msg[100];
        snprintf(msg, sizeof(msg), "Unable to create file with filename %s.\n", filename);
        communication_protocol->sendDebugMessage(msg);

        return false;
    }

    communication_protocol->sendDebugMessage("Created file!\n");

    // Setting read callback:
    this->readCallback = readCallback;
    this->secondsToWrite = secondsToWrite;

    // Create a new WAV header and write it to the file:
    WAVHeader wavHeader = createWavHeader(44100, 16, 8);

    fwrite(&wavHeader, 1, sizeof(WAVHeader), fileWrite);

    // Now record the data and then finish it off by completing header:

    fileWriteStarted = true;

    return true;
}

void SDWrapper::signalWriteDone()
{
    writeDone = true;
}

void SDWrapper::signalReadDone()
{
    // char msg[100];
    // snprintf(msg, sizeof(msg), "data: %d, %d, %d, %d.\n", audioData[0], audioData[64], audioData[256], audioData[512]);
    // communication_protocol->sendDebugMessage(msg);
    readDone = true;
}

uint8_t test[I2S_BLOCK_SIZE /2];

void SDWrapper::run()
{
    // Playing Wav file:
    if (fileReadStarted && writeDone)
    {
        writeDone = false;

        // Block size (adjust as needed)
        uint16_t audioData[I2S_BLOCK_SIZE / 2];

        // Read and transmit audio data in blocks
        if (!feof(fileRead))
        {
            size_t bytesRead = fread((char *)audioData, 1, I2S_BLOCK_SIZE, fileRead);

            writeCallback(audioData, (uint16_t)(bytesRead / 2));
        }
        else
        {
            communication_protocol->sendDebugMessage("Closing file.\n");
            fclose(fileRead);

            fs.unmount();

            writeDone = true;
            fileReadStarted = false;
        }
    }

    // Recording to Wav file:
    if (fileWriteStarted && readDone)
    {
        readDone = false;

        // If sample rade is 44100 khz, 44100 uint16_t array data elements are required for 1 second
        if (bytesWritten < (secondsToWrite * sampleCount))
        {
            // uint16_t audioData[128 * 8];

            readCallback(audioData, 128 * 8);

            // fwrite(audioData, sizeof(uint16_t), 128 * 8, fileWrite);

            bytesWritten += 128;
        }
        else
        {
            finishWavFileRecording();
            communication_protocol->sendDebugMessage("Done recording.\n");
        }

        // char msg[100];
        // snprintf(msg, sizeof(msg), "Bytes written: %d.\n", test);
        // communication_protocol->sendDebugMessage(msg);
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
/// @param file File object in which to open file.
/// @param mode Mode in which to open file.
/// @return Whether the file was found and successfully opened.
bool SDWrapper::openFile(const char *filename, FILE **file, const char *mode)
{
    *file = fopen(filename, mode);

    return *file != NULL;
}

void SDWrapper::finishWavFileRecording()
{
    // Update the chunk and subchunk sizes in the WAV header
    uint32_t fileSize = ftell(fileWrite) - 8; // File size minus the RIFF chunkID and chunkSize
    fseek(fileWrite, 4, SEEK_SET);
    fwrite(&fileSize, 4, 1, fileWrite);

    // Offset to the subchunk2Size field
    fseek(fileWrite, 40, SEEK_SET);
    uint32_t dataSize = sampleCount * sizeof(uint16_t);
    fwrite(&dataSize, 4, 1, fileWrite);

    // Close the WAV file
    fclose(fileWrite);

    char msg[100];
    snprintf(msg, sizeof(msg), "File size: %d, data size: %d.\n", fileSize, dataSize);
    communication_protocol->sendDebugMessage(msg);

    readDone = true;
    fileWriteStarted = false;
    bytesWritten = 0;
}

//*************************************************
//******** WAV header functions *******************
//*************************************************

/// @brief Create WAV header struct object from data array.
/// @param header Data array containing the WAV header data.
/// @return Wav header struct object.
WAVHeader SDWrapper::createWavHeaderFromFile(const uint8_t header[44])
{
    WAVHeader wavHeader;

    wavHeader.chunkID[0] = header[0];
    wavHeader.chunkID[1] = header[1];
    wavHeader.chunkID[2] = header[2];
    wavHeader.chunkID[3] = header[3];
    wavHeader.chunkSize = *(uint32_t *)(&header[4]);
    wavHeader.format[0] = header[8];
    wavHeader.format[1] = header[9];
    wavHeader.format[2] = header[10];
    wavHeader.format[3] = header[11];
    wavHeader.subchunk1ID[0] = header[12];
    wavHeader.subchunk1ID[1] = header[13];
    wavHeader.subchunk1ID[2] = header[14];
    wavHeader.subchunk1ID[3] = header[15];
    wavHeader.subchunk1Size = *(uint32_t *)(&header[16]);
    wavHeader.audioFormat = *(uint16_t *)(&header[20]);
    wavHeader.numChannels = *(uint16_t *)(&header[22]);
    wavHeader.sampleRate = *(uint32_t *)(&header[24]);
    wavHeader.byteRate = *(uint32_t *)(&header[28]);
    wavHeader.blockAlign = *(uint16_t *)(&header[32]);
    wavHeader.bitsPerSample = *(uint16_t *)(&header[34]);
    wavHeader.subchunk2ID[0] = header[36];
    wavHeader.subchunk2ID[1] = header[37];
    wavHeader.subchunk2ID[2] = header[38];
    wavHeader.subchunk2ID[3] = header[39];
    wavHeader.subchunk2Size = *(uint32_t *)(&header[40]);

    return wavHeader;
}

/// @brief Create a new WAV header, to be used when creating a new WAV file.
/// @return
WAVHeader SDWrapper::createWavHeader(uint32_t sampleRate, uint16_t bitsPerSample, uint16_t numChannels)
{
    WAVHeader wavHeader;

    strcpy(wavHeader.chunkID, "RIFF");
    wavHeader.chunkSize = 0; // Will be updated later
    strcpy(wavHeader.format, "WAVE");
    strcpy(wavHeader.subchunk1ID, "fmt ");
    wavHeader.subchunk1Size = 16;            // Size of the format subchunk
    wavHeader.audioFormat = 1;               // PCM audio format
    wavHeader.numChannels = numChannels;     // 1 channel (mono)
    wavHeader.sampleRate = sampleRate;       // Sample rate (e.g., 44.1 kHz)
    wavHeader.bitsPerSample = bitsPerSample; // 16-bit audio
    strcpy(wavHeader.subchunk2ID, "data");
    wavHeader.subchunk2Size = 0; // Will be updated later

    return wavHeader;
}

/// @brief Print the contents of the header of a WAV file.
/// @param wavHeader WAV header object containing header.
void SDWrapper::printWAVHeader(WAVHeader wavHeader)
{
    char msg[100];

    snprintf(msg, sizeof(msg), "ChunkID (RIFF): %c%c%c%c\n", wavHeader.chunkID[0], wavHeader.chunkID[1], wavHeader.chunkID[2], wavHeader.chunkID[3]);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "ChunkSize (File size): %u bytes\n", wavHeader.chunkSize);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "Format (WAVE): %c%c%c%c\n", wavHeader.format[0], wavHeader.format[1], wavHeader.format[2], wavHeader.format[3]);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "Subchunk1ID (fmt): %c%c%c%c\n", wavHeader.subchunk1ID[0], wavHeader.subchunk1ID[1], wavHeader.subchunk1ID[2], wavHeader.subchunk1ID[3]);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "Subchunk1Size (Size of format data): %u bytes\n", wavHeader.subchunk1Size);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "AudioFormat (Audio format): %u (1 for PCM)\n", wavHeader.audioFormat);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "NumChannels (Number of channels): %u\n", wavHeader.numChannels);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "SampleRate (Sample rate): %u Hz\n", wavHeader.sampleRate);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "ByteRate (Byte rate): %u bytes/second\n", wavHeader.byteRate);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "BlockAlign (Block align): %u bytes\n", wavHeader.blockAlign);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "BitsPerSample (Bits per sample): %u bits\n", wavHeader.bitsPerSample);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "Subchunk2ID (data): %c%c%c%c\n", wavHeader.subchunk2ID[0], wavHeader.subchunk2ID[1], wavHeader.subchunk2ID[2], wavHeader.subchunk2ID[3]);
    communication_protocol->sendDebugMessage(msg);

    snprintf(msg, sizeof(msg), "Subchunk2Size (Size of data): %u bytes\n", wavHeader.subchunk2Size);
    communication_protocol->sendDebugMessage(msg);
}