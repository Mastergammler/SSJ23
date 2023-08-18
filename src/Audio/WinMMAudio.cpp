#include "../globals.h"
#include "../imports.h"
#include "../utils.h"

#define SAMPLING_RATE 44100
#define CHUNK_SIZE 1024 * 32
#define BIT_DEPTH 16
#define CHANNELS 2
#define TWO_PI (M_PI + M_PI)
#define PRINT_ERROR(a, args...)                                                \
    printf("ERROR %s() %s Line %d: " a "\n",                                   \
           __FUNCTION__,                                                       \
           __FILE__,                                                           \
           __LINE__,                                                           \
           ##args);

HWAVEOUT wave_out;
WAVEHDR header[2] = {0};
bool chunk_swap = false;
short chunks[2][CHUNK_SIZE];
float toneFrequency = 200;
double wave_position = 0;
double wave_step;

int cycleCount = 0;

thread audioThread;

WaveBuffer currentFile;
int currentSampleIndex;

void CALLBACK WaveOutProc(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
void FillNextBuffer(bool writeWave);
void StartAudioTest();

void InitPlaybackBuffer()
{
    // stop it
    // StartAudioTest();
}

void PlayAudioFile(WaveBuffer& wave)
{
    currentFile = wave;

    {
        WAVEFORMATEX format = {.wFormatTag = WAVE_FORMAT_PCM,
                               .nChannels = CHANNELS,
                               .nSamplesPerSec = SAMPLING_RATE,
                               .wBitsPerSample = BIT_DEPTH,
                               .cbSize = 0

        };

        // how many bytes each sample requires
        format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
        if (waveOutOpen(&wave_out,
                        WAVE_MAPPER,
                        &format,
                        (DWORD_PTR)WaveOutProc,
                        (DWORD_PTR)NULL,
                        CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
        {
            Debug("Waveoutopen failed");
            return;
        }
    }

    // first 4 bit are left, next 4 bit are right
    if (waveOutSetVolume(wave_out, 0xFFFFFFFF) != MMSYSERR_NOERROR)
    {
        Debug("waveOUtGetVolume failed");
        return;
    }

    int currentSample = 0;

    for (int i = 0; i < 2; i++)
    {
        FillNextBuffer(false);

        header[i].lpData = (CHAR*)chunks[i];
        header[i].dwBufferLength = CHUNK_SIZE * 2;
        if (waveOutPrepareHeader(wave_out, &header[i], sizeof(header[i])) !=
            MMSYSERR_NOERROR)
        {
            Debug("waveOutPrepareHeader[%d] failed");
            return;
        }

        if (waveOutWrite(wave_out, &header[i], sizeof(header[i])) !=
            MMSYSERR_NOERROR)
        {
            Debug("waveOutWrite[%d] failed");
            return;
        }
    }
}

void FillNextBuffer(bool writeWave)
{
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        if (currentSampleIndex < currentFile.sample_count)
            chunks[chunk_swap][i] = currentFile.data[currentSampleIndex++];
        else
            chunks[chunk_swap][i] = 0;
    }

    if (writeWave)
    {
        if (waveOutWrite(wave_out,
                         &header[chunk_swap],
                         sizeof(header[chunk_swap])) != MMSYSERR_NOERROR)
        {
            Debug("waveOutWrite failed");
        }
    }

    chunk_swap = !chunk_swap;
}

void StartAudioTest()
{

    {
        WAVEFORMATEX format = {.wFormatTag = WAVE_FORMAT_PCM,
                               .nChannels = CHANNELS,
                               .nSamplesPerSec = SAMPLING_RATE,
                               .wBitsPerSample = BIT_DEPTH,
                               .cbSize = 0

        };

        // how many bytes each sample requires
        format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
        if (waveOutOpen(&wave_out,
                        WAVE_MAPPER,
                        &format,
                        (DWORD_PTR)WaveOutProc,
                        (DWORD_PTR)NULL,
                        CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
        {
            Debug("Waveoutopen failed");
            return;
        }
    }

    // first 4 bit are left, next 4 bit are right
    if (waveOutSetVolume(wave_out, 0xFFFFFFFF) != MMSYSERR_NOERROR)
    {
        Debug("waveOUtGetVolume failed");
        return;
    }

    wave_step = TWO_PI * toneFrequency / SAMPLING_RATE;

    // GENERATE SINE WAVE
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < CHUNK_SIZE; j++)
        {
            chunks[i][j] = sin(wave_position) * 32767;
            wave_position += wave_step;
        }
        header[i].lpData = (CHAR*)chunks[i];
        header[i].dwBufferLength = CHUNK_SIZE * 2;
        if (waveOutPrepareHeader(wave_out, &header[i], sizeof(header[i])) !=
            MMSYSERR_NOERROR)
        {
            Debug("waveOutPrepareHeader[%d] failed");
            return;
        }

        if (waveOutWrite(wave_out, &header[i], sizeof(header[i])) !=
            MMSYSERR_NOERROR)
        {
            Debug("waveOutWrite[%d] failed");
            return;
        }
    }
}

global_var float sineValue = 0;
int runningSampleIndex = 0;

void generateNextChunk()
{
    if (cycleCount++ < 16)
    {
        float prevPosition = wave_position;
        float prevSin = sineValue;

        // DWORD callbackThreadId = GetCurrentThreadId();
        //  cout << "CallbackThread: " << callbackThreadId << endl;
        //   Debug("WOM_DONE");

        float wavePeriod = SAMPLING_RATE / toneFrequency;
        // float sineValue = 0;

        // bytes per sample = sizeof(int16) * 2

        for (int i = 0; i < CHUNK_SIZE; i++)
        {
            float t = TWO_PI * runningSampleIndex / wavePeriod;
            sineValue = sin(t);
            short sampleValue = (short)(sineValue * 10000);

            chunks[chunk_swap][i] = sampleValue;
            if (runningSampleIndex % 500 == 0)
            {
                cout << "Sample: " << sineValue << endl;
            }
            // sin(wave_position) * 32767;
            if (i % CHANNELS == 0) { runningSampleIndex++; }
        }
        // cout << "WavePos: " << prevPosition << "-" << wave_position << endl;
        // cout << "  Delta: " << wave_position - prevPosition << endl;
        // cout << "  Freq: " << frequency << " Step: " << wave_step << endl;
        cout << "Prev : " << prevSin << " - " << sineValue << endl;

        if (waveOutWrite(wave_out,
                         &header[chunk_swap],
                         sizeof(header[chunk_swap])) != MMSYSERR_NOERROR)
        {
            Debug("waveOutWrite failed");
        }
        chunk_swap = !chunk_swap;
    }
}

void CALLBACK WaveOutProc(HWAVEOUT wave_out_handle,
                          UINT message,
                          DWORD_PTR instance,
                          DWORD_PTR param1,
                          DWORD_PTR param2)
{
    // TODO:
    switch (message)
    {
        case WOM_CLOSE:
            Debug("WOM_CLOSE");
            break;
        case WOM_OPEN:
        {
            DWORD callbackThreadId = GetCurrentThreadId();
            cout << "CallbackThread: " << callbackThreadId << endl;
            Debug("WOM_OPEN");
        }
        break;
        case WOM_DONE:
        {
            Debug("Done playing one chunk");
            FillNextBuffer(true);
            // audioThread = thread(generateNextChunk);
            // generateNextChunk();
            // this_thread::sleep_for(chrono::milliseconds(1000));
        }
        break;
    }
}
