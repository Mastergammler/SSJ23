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

// TODO: read bpm from file?
int bpm = 128;

struct AudioPlayback
{
    // TODO: does this prevent delete? this could still be used somewhere
    WAVEHDR header[2] = {0};
    bool buffer_flip = false;
    short flipBuffer[2][CHUNK_SIZE];
    WaveBuffer* wave;
    bool looping;
    int bpm;
    int loop_point_beats;
    int loop_point_samples;
    int current_sample_position;
    bool playback_finished;

    AudioPlayback() {}

    AudioPlayback(WaveBuffer* wave, bool looping, int bpm, int loop_point_bars)
    {
        this->wave = wave;
        this->looping = looping;
        this->bpm = bpm;
        // only for 4/4 right now
        this->loop_point_beats = loop_point_bars * 4;
        this->loop_point_samples =
            2 * SAMPLING_RATE * 60 * loop_point_beats / bpm - 1;
        this->current_sample_position = 0;
        this->playback_finished = false;
    }

    u16 GetNextSample()
    {
        if (playback_finished) return 0;

        int lastSample = wave->sample_count;
        if (current_sample_position >= lastSample)
        {
            if (!looping)
            {
                playback_finished = true;
                return 0;
            }

            current_sample_position = loop_point_samples;
        }
        return wave->data[current_sample_position++];
    }
};

void CALLBACK WaveOutProc(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
void FillNextBuffer(HWAVEOUT device, AudioPlayback& playback, bool writeWave);

/**
 *
 * TODO: Maybe own thread for starting?
 * - Maybe a cacheing strategy per sound? So it could lower latency
 * -> Especially goood, because then i wouldn't need to create 1000 new device
 * instances etc
 * - Optimize cache sizes for sounds (how big do they need to be?)
 * -> Maybe this would run much faster / lower latency a bit
 */
void PlayAudioFile(WaveBuffer* wave, bool loop, int volume)
{
    AudioPlayback* playback = new AudioPlayback(wave, loop, 128, 8);
    HWAVEOUT playbackDevice;
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

    // cout << "Started playback on device: " << playbackDevice << endl;

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

        playback->header[i].lpData = (CHAR*)playback->flipBuffer[i];
        playback->header[i].dwBufferLength = CHUNK_SIZE * 2;

        if (waveOutPrepareHeader(playbackDevice,
                                 &playback->header[i],
                                 sizeof(playback->header[i])) !=
            MMSYSERR_NOERROR)
        {
            Debug("waveOutPrepareHeader[%d] failed");
            Log(logger, "Preparing the header failed");
            return;
        }

        if (waveOutWrite(playbackDevice,
                         &playback->header[i],
                         sizeof(playback->header[i])) != MMSYSERR_NOERROR)
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
        playback.flipBuffer[playback.buffer_flip][i] = playback.GetNextSample();
    }

    if (writeWave &&
        waveOutWrite(device,
                     &playback.header[playback.buffer_flip],
                     sizeof(playback.header[playback.buffer_flip])) !=
            MMSYSERR_NOERROR)
    {
        Debug("waveOutWrite failed");
        Log(logger, "Unable to write buffer to device");
    }

    playback.buffer_flip = !playback.buffer_flip;
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
            break;
        case WOM_OPEN:
        {
            // Debug("WOM_OPEN");
        }
        break;
        case WOM_DONE:
        {
            // Log(logger, "WOM is done");
            if (playback->playback_finished)
            {
                // FIXME: memory leak
                // this is a memory leak, becaues i never clean up my old
                // buffers. but when i try to do it, it crashes the app for some
                // reason, not sure whats going on there
                // delete playback;

                // short* first = playback->flipBuffer[0];
                //  delete[] first;

                // reseting is really device based
                // -> i guess the api is just wired
                // you have to start a new device, but when you stop it, the
                // same is used
                //
                // if (waveOutReset(playbackDevice) != MMSYSERR_NOERROR)
                //{
                //     Log(logger, "Could not reset playback device");
                // }
                //  if (waveOutClose(playbackDevice) != MMSYSERR_NOERROR)
                //{
                //      Log(logger, "Error during closing device");
                //  }
            }
            else { FillNextBuffer(playbackDevice, *playback, true); }
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

        // flipBuffer[buffer_flip][i] = sampleValue;
        if (i % CHANNELS == 0) { runningSampleIndex++; }
    }

    // if (waveOutWrite(device,
    //                  &header[buffer_flip],
    //                  sizeof(header[buffer_flip])) != MMSYSERR_NOERROR)
    //{
    //     Debug("waveOutWrite failed");
    // }
    // buffer_flip = !buffer_flip;
}
