/*
  ==============================================================================

    CustomFilter.h
    Created: 6 May 2024 3:45:57pm
    Author:  Slend

  ==============================================================================
*/

#define HP_FILTER_ORDER 3

#pragma once
#include <JuceHeader.h>
#include <typeinfo>
#include "Enums.h"
#include "Defines.h"

template <typename Type>
class SVFilter {
public:

    SVFilter() {
        svFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        svFilter.setResonance(1);
    }

    void setResonance(double res) {
        smooth.setTargetValue(res);
    }

    void updateResonance() {
        svFilter.setResonance(smooth.getNextValue());
    }

    void setCutoffFrequency(double freq)
    {
        svFilter.setCutoffFrequency(freq);
    }   

    Type processSample(int channel ,Type val) {
        return svFilter.processSample(channel, val);
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        svFilter.process(context);
    }

    void reset() {
        
        svFilter.reset();
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        svFilter.prepare(spec);
        smooth.setTargetValue(DEFAULT_RESONANCE);
        smooth.reset(spec.sampleRate / SAMPLE_SKIPS, DEFAULT_RAMP_DURATION);
    }

private:
    juce::LinearSmoothedValue<double> smooth{ 0.0f };

    juce::dsp::StateVariableTPTFilter<Type> svFilter;
};




template <typename Type>
class HPFilter {
public:

    HPFilter() {

        filterCount = ceil(HP_FILTER_ORDER / 2);
        for (int i = 0; i < filterCount; i++)
            hpFilter.add(new IIRStereoFilter());


        //auto coeffArray = juce::dsp::FilterDesign<Type>::designIIRLowpassHighOrderButterworthMethod(freq, sampleRate, 3);
    }

    void SetCoefficents(float sampleRate, float freq) {
        
        auto coeffArray = juce::dsp::FilterDesign<Type>::designIIRHighpassHighOrderButterworthMethod(freq, sampleRate, HP_FILTER_ORDER);

        for (int i = 0; i < filterCount; i++)
            *hpFilter[i]->state = *coeffArray[i];
    }


    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        for (int i = 0; i < filterCount; i++) 
            hpFilter[i]->process(context);
    }

    void reset() {
        for (int i = 0; i < filterCount; i++) 
            hpFilter[i]->reset();
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {

        for (int i = 0; i < filterCount; i++) 
            hpFilter[i]->prepare(spec);
    }
private:
    typedef juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<Type>, juce::dsp::IIR::Coefficients<Type>> IIRStereoFilter;
    juce::OwnedArray<IIRStereoFilter> hpFilter;
    int filterCount;
};