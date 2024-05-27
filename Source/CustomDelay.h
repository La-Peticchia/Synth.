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

    CustomDelay(float sampleRate = DEFAULT_SAMPLE_RATE) {
        delayLine.setMaximumDelayInSamples(MAX_DELAY_TIME * sampleRate);
        delayLine.setDelay(DEFAULT_DELAY * sampleRate);
        feedBack.setCurrentAndTargetValue(DEFAULT_FEEDBACK_GAIN);
        dry.setCurrentAndTargetValue(1.f);
        wet.setCurrentAndTargetValue(1.f);
        this->sampleRate = sampleRate;
    }

    void processBlock(juce::AudioBuffer<float>& buffer) {

        if (!enabled)
            return;

        int channels = buffer.getNumChannels(), numSamples = buffer.getNumSamples(), startSample = 0;
        auto pointers = buffer.getArrayOfWritePointers();

        while (numSamples--)
        {
            for (int i = 0; i < channels; i++)
                pointers[i][startSample] = processSample(i ,pointers[i][startSample]);
            startSample++;
        }
        lpFilter.snapToZero();

    }

    void prepare(juce::dsp::ProcessSpec spec) {
        delayLine.prepare(spec);

        lpFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(spec.sampleRate, DELAY_LP_FILTER_FREQ);
        lpFilter.prepare(spec);

        feedBack.reset(spec.sampleRate, DEFAULT_RAMP_DURATION);
        dry.reset(spec.sampleRate, DEFAULT_RAMP_DURATION);
        wet.reset(spec.sampleRate, DEFAULT_RAMP_DURATION);
    }

    float processSample(int channel, float sample) {
        float out = delayLine.popSample(channel);
        out = lpFilter.processSample(out);

        delayLine.pushSample(channel,sample + out * feedBack.getNextValue());
        return out * wet.getNextValue() + sample * dry.getNextValue();
    }

    void SetDelay(float value){
        DBG(value);
        delayLine.setDelay(value * sampleRate);

    }

    void SetFeedback(float value) {
        feedBack.setTargetValue(value);
    }

    void SetDry(float value) {
        dry.setTargetValue(value);
    }
    
    void SetWet(float value) {
        wet.setTargetValue(value);
    }

    void SetActive(bool tf)
    {
        enabled = tf;
    }

private:

    bool enabled = false;

    juce::LinearSmoothedValue<float> feedBack, dry, wet;
    juce::dsp::DelayLine<float> delayLine;
    juce::dsp::IIR::Filter<float> lpFilter;

    float sampleRate;
};


