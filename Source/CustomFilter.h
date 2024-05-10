/*
  ==============================================================================

    CustomFilter.h
    Created: 6 May 2024 3:45:57pm
    Author:  Slend

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Enums.h"

template <typename Type>
class CustomFilter {
public:

    CustomFilter() {
        stFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        stFilter.setResonance(1);
    }

    void setResonance(double res) {
        stFilter.setResonance(res);
    }

    void setCutoffFrequency(double freq)
    {
        stFilter.setCutoffFrequency(freq);
    }   

    Type processSample(int channel ,Type val) {
        return stFilter.processSample(channel, val);
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        stFilter.process(context);
    }

    void reset() {
        
        stFilter.reset();
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        stFilter.prepare(spec);
    }

private:

    juce::dsp::StateVariableTPTFilter<Type> stFilter;
};