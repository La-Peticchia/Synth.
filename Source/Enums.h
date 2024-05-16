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
    logWave,
    expWave
};

enum FunctionType { //waveshaper
    softClip,
    hardClip,
    waveFold,
    none
};

enum EnvType {
    gainEnv,
    lpFilterEnv
};

