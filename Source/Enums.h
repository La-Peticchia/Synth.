/*
  ==============================================================================

    Enums.h
    Created: 30 Apr 2024 5:11:53pm
    Author:  Slend

  ==============================================================================
*/

#pragma once

enum EnvState { //generatori
    attack,
    sustain,
    release,
    idle
};

enum WaveType { //oscillator
    sine,
    sawThooth,
    squareWave,
    triangular,
    expWave
};

enum FunctionType { //waveshaper
    softClip,
    hardClip,
    waveFold,
    absolute,
    none
};

enum EnvType {
    gainEnv,
    lpFilterEnv
};

