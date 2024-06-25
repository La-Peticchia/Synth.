/*
  ==============================================================================

    CustomFilter.h
    Created: 6 May 2024 3:45:57pm
    Author:  Slend

  ==============================================================================
*/


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



    void setCutoffFrequency(double freq)
    {
        svFilter.setCutoffFrequency(freq);
        svFilter.setResonance(smooth.getNextValue());
    }   

    Type processSample(int channel ,Type val) {
        if (enabled)
            return svFilter.processSample(channel, val);
        else
            return val;
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        if(enabled)
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

    bool enabled = false;

private:
    juce::LinearSmoothedValue<double> smooth{ 0.0f };

    juce::dsp::StateVariableTPTFilter<Type> svFilter;

};




template <typename Type>
class HPFilter {
public:

    HPFilter() {

        filterCount = ceil(HIGH_PASS_FILTER_ORDER / 2);
        for (int i = 0; i < filterCount; i++)
            hpFilter.add(new IIRStereoFilter());


        //auto coeffArray = juce::dsp::FilterDesign<Type>::designIIRLowpassHighOrderButterworthMethod(freq, sampleRate, 3);
    }

    void SetCoefficents(float sampleRate, int freq) {
        
        //auto coeffArray = HPFilterCoefficients::GetCoefficient(note);
        auto coeffArray = juce::dsp::FilterDesign<Type>::designIIRHighpassHighOrderButterworthMethod(freq, sampleRate, HP_FILTER_ORDER);
        //DBG(typeid(coeffArray).name());

        for (int i = 0; i < filterCount; i++)
            *hpFilter[i]->state = *coeffArray[i];

    }

    void SetCoefficents(juce::ReferenceCountedArray<struct juce::dsp::IIR::Coefficients<float>> coeffArray) {

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

struct HPFilterCoefficients {
public:

    HPFilterCoefficients(double sampleRate  = DEFAULT_SAMPLE_RATE) {

        //coefficients.add(juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(juce::MidiMessage::getMidiNoteInHertz(0) + 0.1f, sampleRate, HP_FILTER_ORDER));
        for (int i = 0; i < NOTES; i++)
        coefficients.add(juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(juce::MidiMessage::getMidiNoteInHertz(i), sampleRate, HIGH_PASS_FILTER_ORDER));
    }
    
    juce::ReferenceCountedArray<struct juce::dsp::IIR::Coefficients<float>> GetCoefficient(int index)
    {
        return coefficients[index];
    }

private:
    juce::Array<juce::ReferenceCountedArray<struct juce::dsp::IIR::Coefficients<float>>> coefficients;
    
};


class LPButtFilter {
public:

    LPButtFilter() {
        filterCount = ceil(LOW_PASS_FILTER_ORDER / 2);
        for (int i = 0; i < filterCount; i++) 
            lpFilter.add(new IIRStereoFilter());
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        for (int i = 0; i < filterCount; i++)
            lpFilter[i]->process(context);
    }

    void reset() {
        for (int i = 0; i < filterCount; i++)
            lpFilter[i]->reset();
    }

    void prepare(const juce::dsp::ProcessSpec& spec, float cutoffFreq)
    {
        auto coeffArray = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(cutoffFreq, spec.sampleRate, LOW_PASS_FILTER_ORDER);

        for (int i = 0; i < filterCount; i++) {
            lpFilter[i]->prepare(spec);
            *lpFilter[i]->state = *coeffArray[i];
        }
    }
private:
    typedef juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> IIRStereoFilter;
    juce::OwnedArray<IIRStereoFilter> lpFilter;
    int filterCount;
};




