/*
  ==============================================================================

    Enums.h
    Created: 30 Apr 2024 5:11:53pm
    Author:  Slend

  ==============================================================================
*/

#pragma once

enum envState {
    attack,
    sustain,
    release,
    silence
};

enum WaveType {
    sine,
    sawThooth,
    squareWave,
    triangular
};

enum FunctionType {
    softClip,
    hardClip,
    waveFold,
    none
};