/*
  ==============================================================================

    CustomFlanger.h
    Created: 18 May 2024 11:36:57am
    Author:  Slend

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Defines.h"

class CustomFlanger {
public:
    CustomFlanger(float sampleRate = DEFAULT_SAMPLE_RATE, float oscFrequency = DEFAULT_LFO_FREQUENCY, float timeInterval = DEFAULT_DELAY, float gain = DEFAULT_GAIN) {
        lineLenght = (int)(timeInterval * sampleRate);
        deltaAngle = 2 * juce::MathConstants<float>::pi * oscFrequency / sampleRate;
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


    void SetGain(float value) {
        gain = value;
    }

    void SetDelay(float value) {
        lineLenght = (int)(value * sampleRate);
        delayLine = std::vector<float>(lineLenght, 0.f);
    }

    void SetFrequency(float value ) {
        deltaAngle = 2 * juce::MathConstants<float>::pi * value / sampleRate;
    }

    void SetSampleRate(float value) {
        sampleRate = value;
    }

    void SetActive(bool tf) {
        enabled = tf;
    }

private:
    

    std::vector<float> delayLine;
    int lineLenght = 0;
    float deltaAngle, currentAngle, gain = DEFAULT_GAIN, sampleRate = DEFAULT_SAMPLE_RATE;
    bool enabled = true;

    void UpdateAngle() {
        currentAngle = std::fmod(currentAngle + deltaAngle, 2 * juce::MathConstants<float>::pi);
    }

    float Map(float val) {
        float m = (lineLenght - 1) / 2;
        return val * m + m;
    }

    float processSample(float sample) {

        int index = (int)Map(std::sin(currentAngle));
        float out = sample + delayLine[index] * gain;
        UpdateAngle();
        delayLine.pop_back();
        delayLine.insert(delayLine.begin(), sample);

        return out;
    }
};