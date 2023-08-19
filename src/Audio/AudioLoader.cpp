#include "../imports.h"
#include "../types.h"
#include "../utils.h"

bool EqualMagicBytes(char* magic, string expected)
{
    return magic[0] == expected.c_str()[0] && magic[1] == expected.c_str()[1] &&
           magic[2] == expected.c_str()[2] && magic[3] == expected.c_str()[3];
}

WaveBuffer LoadWaveFile(string path)
{
    FILE* file;
    char magic[4];
    int fileSize;
    int formatLength;
    short formatType;
    short numChannels;
    int sampleRate;
    int bytesPerSecond;
    short blockAlign;
    short bitsPerSample;
    int dataSize;

    errno_t error = fopen_s(&file, path.c_str(), "rb");
    if (error != 0) { Debug("Unable to open file"); }

    fread_s(&magic, sizeof(magic), sizeof(char), 4, file);
    if (!EqualMagicBytes(magic, "RIFF")) { Debug("Magic should equal riff"); }

    fread_s(&fileSize, sizeof(fileSize), sizeof(int), 1, file);

    fread_s(&magic, sizeof(magic), sizeof(char), 4, file);
    if (!EqualMagicBytes(magic, "WAVE")) { Debug("Magic should equal wave"); }

    fread_s(&magic, sizeof(magic), sizeof(char), 4, file);
    if (!EqualMagicBytes(magic, "fmt ")) { Debug("Magic should equal 'fmt '"); }

    fread_s(&formatLength, sizeof(formatLength), sizeof(int), 1, file);

    fread_s(&formatType, sizeof(formatType), sizeof(short), 1, file);
    if (formatType != 1) { Debug("Format type must be 1"); }

    fread_s(&numChannels, sizeof(numChannels), sizeof(short), 1, file);
    if (numChannels != 2) { Debug("Expecting 2 channel audio"); }

    fread_s(&sampleRate, sizeof(sampleRate), sizeof(int), 1, file);
    if (sampleRate != 44100) { Debug("Expecting 44.1 khz audio"); }

    fread_s(&bytesPerSecond, sizeof(bytesPerSecond), sizeof(int), 1, file);

    fread_s(&blockAlign, sizeof(blockAlign), sizeof(short), 1, file);

    fread_s(&bitsPerSample, sizeof(bitsPerSample), sizeof(short), 1, file);
    if (bitsPerSample != 16) { Debug("Expecting 16bit audio"); }

    fread_s(&magic, sizeof(magic), sizeof(char), 4, file);
    cout << "Magic1 " << magic[0] << magic[1] << magic[2] << magic[3] << endl;
    if (EqualMagicBytes(magic, "bext"))
    {
        Debug("Wave is BWF format");

        DWORD id;
        int size;

        // TODO: learn how to road out or skip the bext format
        fread_s(&id, sizeof(id), sizeof(DWORD), 1, file);
        cout << "bextid " << id << endl;

        fread_s(&size, sizeof(size), sizeof(int), 1, file);
        cout << "bektSize " << size << endl;

        // void* bextData = malloc(size);
        // size_t bytesRead = fread_s(bextData, size, 1, size, file);
        //  size_t bytesRead = 0;
        // cout << "read " << bytesRead << " bext bytes" << endl;
    }

    // find data tag
    int bextByteCount = 0;
    char nextByte = ' ';
    while (nextByte != 'd')
    {
        bextByteCount++;
        fread_s(&nextByte, sizeof(nextByte), 1, 1, file);
    }

    cout << "Actual bext data size: " << bextByteCount << endl;

    // move 1 step back
    fseek(file, -1, SEEK_CUR);

    // lookahead
    // char lookahead[32];
    // fread_s(&lookahead, sizeof(lookahead), sizeof(char), 32, file);
    // cout << "Attempting read" << endl;
    // cout << "Lookahead " << lookahead << endl;

    fread_s(&magic, sizeof(magic), sizeof(char), 4, file);
    cout << "Magic1 " << magic[0] << magic[1] << magic[2] << magic[3] << endl;
    if (!EqualMagicBytes(magic, "data"))
    {
        Debug("Expected data chunk, but was not found");
    }

    fread_s(&dataSize, sizeof(dataSize), sizeof(int), 1, file);

    WaveBuffer buffer = {};
    void* data = malloc(dataSize);
    if (data != NULL) { buffer.loaded = true; }
    else { Debug("Failed to load data"); }

    size_t bytesRead = fread_s(data, dataSize, 1, dataSize, file);
    buffer.data = (u16*)data;
    // because 16 bit audio = 1 sample every 2 byte of data
    buffer.sample_count = dataSize / 2;

    fclose(file);

    return buffer;
}
