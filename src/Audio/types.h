#include "module.h"

struct SoundPlayback
{
    WAVEHDR header = {};
    u16* dynamic_buffer;
    WaveBuffer* wave;

    int current_sample_index;
    bool playback_finished;
};

