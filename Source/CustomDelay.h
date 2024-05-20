/*
  ==============================================================================

    CustomDelay.h
    Created: 16 May 2024 2:20:54pm
    Author:  Slend

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Defines.h"

class CustomDelay {
public:

    CustomDelay(float sampleRate = DEFAULT_SAMPLE_RATE, float timeInterval = DEFAULT_DELAY, float gain = DEFAULT_GAIN) {

        lineLenght = (int)(timeInterval * sampleRate);
        delayLine = std::vector<float>(lineLenght, 0.f);
        this->gain = gain;

    }

    void processBlock(juce::AudioBuffer<float>& buffer) {
        if (!enabled)
            return;

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

    void SetGain(float value ) {
        gain = juce::jlimit(0.1f, 0.9f, value);
    }

    void SetDelay(float value ) {
        lineLenght = (int)(value * sampleRate);
        delayLine = std::vector<float>(lineLenght, 0.f);
    }

    void SetSampleRate(float value ) {
        sampleRate = value;
    }

    void SetActive(bool tf) {
        enabled = tf;
    }
private:

    std::vector<float> delayLine;
    int lineLenght = 0;
    float sampleRate = DEFAULT_SAMPLE_RATE;
    float gain = DEFAULT_GAIN;
    bool enabled = true;

    float processSample(float sample) {

        float pop = delayLine.back();
        delayLine.pop_back();

        float out = sample + pop;
        delayLine.insert(delayLine.begin(), out * gain);

        

        //if (out == 0.f)
        //    DBG("error");

        return out;
    }
};