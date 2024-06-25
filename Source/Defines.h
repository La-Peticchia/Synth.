/*
  ==============================================================================

    Defines.h
    Created: 12 May 2024 10:02:50am
    Author:  Slend

  ==============================================================================
*/

#pragma once

#define SUSTAIN_VALUE 0.5f
#define SAMPLE_SKIPS 5
#define NOTES 128
#define DELAY_LP_FILTER_FREQ 2000.f
#define HIGH_PASS_FILTER_ORDER 5
#define LOW_PASS_FILTER_ORDER 5
#define FINAL_CUTOFF_FREQ 10000.f
#define OVERSAMPLING_FACTOR 1



#define DEFAULT_RAMP_DURATION 0.1f
#define DEFAULT_SAMPLE_RATE 48000.
#define DEFAULT_RESONANCE 1/std::sqrt(2)
#define DEFAULT_GAIN  0.7f

//delay
#define MAX_DELAY_TIME 1.01f
#define DEFAULT_DELAY 0.1f
#define DEFAULT_FEEDBACK_GAIN  0.7f

//flanger
#define DEFAULT_FLANGER_DELAY 0.0145f
#define DEFAULT_LFO_FREQUENCY 0.01f
#define DEFAULT_LFO_DEPTH 1.f

//Distortion
#define MIN_GAIN 1.1f
#define MAX_GAIN 5.f
