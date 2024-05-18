/*
  ==============================================================================

    CustomDelay.h
    Created: 16 May 2024 2:20:54pm
    Author:  Slend

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Defines.h"
#pragma once

class CustomDelay {
public:

    CustomDelay(float sampleRate = DEFAULT_SAMPLE_RATE, float timeInterval = DEFAULT_DELAY, float gain = DEFAULT_GAIN) {

        lineLenght = (int)(timeInterval / (1 / sampleRate));
        DBG(lineLenght);
        delayLine = std::vector<float>(lineLenght, 0.f);
        this->gain = gain;

    }

    void processBlock(juce::AudioBuffer<float>& buffer) {

        int numSamples = buffer.getNumSamples();
        int startSample = 0, channels = buffer.getNumChannels();
        auto pointers = buffer.getArrayOfWritePointers();


        while (numSamples--)
        {
            for (int i = 0; i < channels; i++)
                pointers[i][startSample] = processSample(pointers[i][startSample]);
            startSample++;
        }
        
    }

    float processSample(float sample) {

        float pop = delayLine.back();
        delayLine.pop_back();

        float out = sample + pop;
        delayLine.insert(delayLine.begin(), sample + pop * gain);
        
        return out;
    }

    void SetGain(float value) {
        gain = juce::jlimit(0.1f, 0.9f, value);
    }

    void SetDelay(float sampleRate = DEFAULT_SAMPLE_RATE, float timeInterval = DEFAULT_DELAY) {
        lineLenght = (int)(timeInterval / (1 / sampleRate));
        delayLine = std::vector<float>(lineLenght, 0.f);
    }

private:

    std::vector<float> delayLine;
    int lineLenght = DEFAULT_DELAY;
    float gain = DEFAULT_GAIN;
};