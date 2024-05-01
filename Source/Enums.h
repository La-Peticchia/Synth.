/*
  ==============================================================================

    Enums.h
    Created: 30 Apr 2024 5:11:53pm
    Author:  Slend

  ==============================================================================
*/

#pragma once

enum envState { //generatori
    attack,
    sustain,
    release,
    silence
};

enum WaveType { //oscillator
    sine,
    sawThooth,
    squareWave,
    triangular
};

enum FunctionType { //waveshaper
    softClip,
    hardClip,
    waveFold,
    none
};