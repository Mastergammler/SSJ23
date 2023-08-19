#include "../globals.h"
#include "../imports.h"
#include "../utils.h"
#include <mmeapi.h>

#define SAMPLING_RATE 44100
#define CHUNK_SIZE 1024 * 8
#define BIT_DEPTH 16
#define CHANNELS 2
#define TWO_PI (M_PI + M_PI)
#define VOLUME_MAX 0xFFFF
#define MAX_SIM_DEVICES 4

#define PRINT_ERROR(a, args...)                                                \
    printf("ERROR %s() %s Line %d: " a "\n",                                   \
           __FUNCTION__,                                                       \
           __FILE__,                                                           \
           __LINE__,                                                           \
           ##args);

HWAVEOUT audioPlaybackDevice[MAX_SIM_DEVICES];
int playbackDeviceIdx = 0;
WAVEHDR header[2] = {0};
bool buffer_flip = false;
short flipBuffer[2][CHUNK_SIZE];

// TODO: read bpm from file?
int bpm = 128;

struct AudioPlayback
{
    WaveBuffer wave;
    bool looping;
    int bpm;
    int loop_point_beats;
    int loop_point_samples;
    int runningSampleIndex;

    AudioPlayback() {}

    AudioPlayback(WaveBuffer wave, bool looping, int bpm, int loop_point_bars)
    {
        this->wave = wave;
        this->looping = looping;
        this->bpm = bpm;
        // only for 4/4 right now
        this->loop_point_beats = loop_point_bars * 4;
        this->loop_point_samples =
            2 * SAMPLING_RATE * 60 * loop_point_beats / bpm - 1;
        this->runningSampleIndex = 0;
    }

    u16 GetNextSample()
    {
        int lastSample = wave.sample_count;
        if (runningSampleIndex >= lastSample)
        {
            if (!looping) return 0;

            runningSampleIndex = loop_point_samples;
        }
        return wave.data[runningSampleIndex++];
    }
};

void CALLBACK WaveOutProc(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
void FillNextBuffer(HWAVEOUT device, AudioPlayback& playback, bool writeWave);

void PlayAudioFile(WaveBuffer& wave, bool loop, int volume)
{
    if (playbackDeviceIdx > MAX_SIM_DEVICES)
    {
        // TODO: improve logging - what sound is requested!
        Log(logger, "Too many open devices, sound will ont be played");
        return;
    }
    AudioPlayback* playback = new AudioPlayback(wave, loop, 128, 8);
    HWAVEOUT playbackDevice = audioPlaybackDevice[playbackDeviceIdx++];

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
        if (waveOutOpen(&playbackDevice,
                        WAVE_MAPPER,
                        &format,
                        (DWORD_PTR)WaveOutProc,
                        (DWORD_PTR)playback,
                        CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
        {
            Debug("Unable to open audio device");
            Log(logger, "Unable to open audio device");
            return;
        }
    }

    int percentage = volume * VOLUME_MAX / 100;
    long twoChannelVolume = (percentage << 16) | (percentage << 0);

    if (waveOutSetVolume(playbackDevice, twoChannelVolume) != MMSYSERR_NOERROR)
    {
        Debug("waveOUtGetVolume failed");
        Log(logger, "Unable to set the volume on the audio device");
        return;
    }

    for (int i = 0; i < 2; i++)
    {
        FillNextBuffer(playbackDevice, *playback, false);

        header[i].lpData = (CHAR*)flipBuffer[i];
        header[i].dwBufferLength = CHUNK_SIZE * 2;

        if (waveOutPrepareHeader(playbackDevice,
                                 &header[i],
                                 sizeof(header[i])) != MMSYSERR_NOERROR)
        {
            Debug("waveOutPrepareHeader[%d] failed");
            Log(logger, "Preparing the header failed");
            return;
        }

        if (waveOutWrite(playbackDevice, &header[i], sizeof(header[i])) !=
            MMSYSERR_NOERROR)
        {
            Debug("waveOutWrite[%d] failed");
            Log(logger, "Unable to write to the audio device");
            return;
        }
    }
}

void FillNextBuffer(HWAVEOUT device, AudioPlayback& playback, bool writeWave)
{
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        flipBuffer[buffer_flip][i] = playback.GetNextSample();
    }

    if (writeWave &&
        waveOutWrite(device,
                     &header[buffer_flip],
                     sizeof(header[buffer_flip])) != MMSYSERR_NOERROR)
    {
        Debug("waveOutWrite failed");
        Log(logger, "Unable to write buffer to device");
    }

    buffer_flip = !buffer_flip;
}

void CALLBACK WaveOutProc(HWAVEOUT playbackDevice,
                          UINT message,
                          DWORD_PTR instance,
                          DWORD_PTR param1,
                          DWORD_PTR param2)
{

    AudioPlayback* playback = (AudioPlayback*)instance;

    switch (message)
    {
        case WOM_CLOSE:
            Debug("WOM_CLOSE");
            // TODO: is this slow for sound effects?
            delete (playback);
            break;
        case WOM_OPEN:
        {
            Debug("WOM_OPEN");
        }
        break;
        case WOM_DONE:
        {
            FillNextBuffer(playbackDevice, *playback, true);
        }
        break;
    }
}

// -------------------------- TESTING ----------------------------------

global_var float sineValue = 0;
int runningSampleIndex = 0;
float toneFrequency = 200;

void generateNextSineChunk(HWAVEOUT device)
{
    float prevSin = sineValue;
    float wavePeriod = SAMPLING_RATE / toneFrequency;

    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        float t = TWO_PI * runningSampleIndex / wavePeriod;
        sineValue = sin(t);
        short sampleValue = (short)(sineValue * 10000);

        flipBuffer[buffer_flip][i] = sampleValue;
        if (i % CHANNELS == 0) { runningSampleIndex++; }
    }

    if (waveOutWrite(device,
                     &header[buffer_flip],
                     sizeof(header[buffer_flip])) != MMSYSERR_NOERROR)
    {
        Debug("waveOutWrite failed");
    }
    buffer_flip = !buffer_flip;
}
