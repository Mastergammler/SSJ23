#pragma once

#include "../Logging/module.h"
#include "../globals.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"

WaveBuffer LoadWaveFile(string path);
void PlayAudioFile(WaveBuffer* wave, bool loop, int volume);
