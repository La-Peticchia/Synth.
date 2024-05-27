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
    CustomFlanger(float sampleRate = DEFAULT_SAMPLE_RATE) {
        
        this->sampleRate = sampleRate;

        SetFrequency(DEFAULT_LFO_FREQUENCY);
        dry.setCurrentAndTargetValue(1.f);
        wet.setCurrentAndTargetValue(1.f);
        depth.setCurrentAndTargetValue(DEFAULT_LFO_DEPTH);
        currentDelay.setCurrentAndTargetValue(DEFAULT_FLANGER_DELAY);
        
        deltaAngle.reset(sampleRate, DEFAULT_RAMP_DURATION);
        dry.reset(sampleRate, DEFAULT_RAMP_DURATION);
        wet.reset(sampleRate, DEFAULT_RAMP_DURATION);
        depth.reset(sampleRate, DEFAULT_RAMP_DURATION);
        currentDelay.reset(sampleRate, DEFAULT_RAMP_DURATION);


    }

    void processBlock(juce::AudioBuffer<float>& buffer) {
        if (!enabled)
            return;

        int numSamples = buffer.getNumSamples();
        int startSample = 0, channels = buffer.getNumChannels();
        auto pointers = buffer.getArrayOfWritePointers();

        while (numSamples--)
        {
            float out = popSample();
            pushSample(pointers[0][startSample]);

            for (int i = 0; i < channels; i++)
                pointers[i][startSample] = pointers[i][startSample] * dry.getNextValue() + out * wet.getNextValue();

            startSample++;
        }

    }

    void SetDry(float value) {
        dry.setTargetValue(value);
    }
    void SetWet(float value) {
        wet.setTargetValue(value);
    }


    void SetFrequency(float value) {
        deltaAngle.setTargetValue(2 * juce::MathConstants<float>::pi * value / sampleRate);

    }

    void SetDelay(float value) {
        currentDelay.setTargetValue(value);
    }

    void SetDepth(float value) {
        depth.setTargetValue(value);
    }

    void SetSampleRate(float value) {
        sampleRate = value;
    }

    void SetActive(bool tf) {
        enabled = tf;
    }

private:

    bool enabled = false;

    float sampleRate = DEFAULT_SAMPLE_RATE, currentAngle = 0.f;
    int delayWritePos = 0, maxDelaySize = (int)(MAX_DELAY_TIME * sampleRate) + 1;
    juce::LinearSmoothedValue<float> currentDelay, depth, dry, wet, deltaAngle;
    std::vector<float> delayLine = std::vector<float>(maxDelaySize, 0.f);


    void UpdateAngle() {
        currentAngle = std::fmod(currentAngle + deltaAngle.getNextValue(), 2 * juce::MathConstants<float>::pi);
    }

    float popSample() {
        float delayInSamples = currentDelay.getNextValue() * sampleRate;
        float depthInSamples =  depth.getNextValue() * delayInSamples / 2;
        float delayFloatingReadPos = std::fmod(delayWritePos + maxDelaySize - delayInSamples + depthInSamples * (1 + sin(currentAngle)) - 1, maxDelaySize);
        int delayReadPos = std::floor(delayFloatingReadPos);
        float value1 = delayLine[delayReadPos];
        float value2 = delayLine[(delayReadPos + 1) % maxDelaySize];

        UpdateAngle();
        if (delayReadPos == delayWritePos)
            DBG(delayReadPos);

        return value1 + (value2 - value1) * (delayFloatingReadPos - delayFloatingReadPos);
    }

    void pushSample(float value) {
        delayLine[delayWritePos] = value;
        if (++delayWritePos >= maxDelaySize)
            delayWritePos -= maxDelaySize;
    }
};
