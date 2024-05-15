/*
  ==============================================================================

    CustomFilter.h
    Created: 6 May 2024 3:45:57pm
    Author:  Slend

  ==============================================================================
*/

#define HP_FILTER_ORDER 5

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

    bool enabled = true;

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

    void SetCoefficents(float sampleRate, int freq) {
        
        //auto coeffArray = HPFilterCoefficients::GetCoefficient(note);
        auto coeffArray = juce::dsp::FilterDesign<Type>::designIIRHighpassHighOrderButterworthMethod(freq, sampleRate, HP_FILTER_ORDER);
        DBG(typeid(coeffArray).name());

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
        coefficients.add(juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(juce::MidiMessage::getMidiNoteInHertz(i), sampleRate, HP_FILTER_ORDER));
    }
    
    juce::ReferenceCountedArray<struct juce::dsp::IIR::Coefficients<float>> GetCoefficient(int index)
    {
        return coefficients[index];
    }

private:
    
    juce::Array<juce::ReferenceCountedArray<struct juce::dsp::IIR::Coefficients<float>>> coefficients;
};


class FDNReverb {
public:

    FDNReverb() {
        delayLines[0].setDelay(50);
        delayLines[1].setDelay(175);
        delayLines[2].setDelay(380);

        for (int i = 0; i < 3; i++)
        {
            c[i] = 1.f;
            b[i] = 1.f;
            g[i][i] = 0.7f;
        }


        std::vector<std::vector<int>> hadam = hadamard_matrix(3);
        for (int i = 0; i < 3; i++)
            DBG(hadam[i][0] << " " << hadam[i][1] << " " << hadam[i][2]);
    }

    float processSample(float sample) {
        float y = sample * d;
        std::vector<float> z = std::vector<float>(3, 0.f);

        for (int i = 0; i < 3; i++)
            z[i] = delayLines[i].popSample(0);

        std::vector<float> out = matrix_vector_mult(g, matrix_vector_mult(A, z));



        for (int i = 0; i < 3; i++)
        {
            delayLines[i].pushSample(0, out[i] + b[i] * sample);
            y += c[i] * z[i];
        }

    }



private:
    std::vector<float> b = std::vector<float>(3, 0.f), c = std::vector<float>(3, 0.f);
    std::vector<std::vector<float>> A = std::vector<std::vector<float>>(3, std::vector<float>(3, 0.f)), g = = std::vector<std::vector<float>>(3, std::vector<float>(3, 0.f));
    juce::Array<juce::dsp::DelayLine<float>> delayLines = juce::Array<juce::dsp::DelayLine<float>>(juce::dsp::DelayLine<float>(), 3);
    float d;

    //float b[3], c[3], A[3][3], delayLines[3][100];

    float dot_product(std::vector<float> v1, std::vector<float> v2) {
        float result = 0.0;
        for (size_t i = 0; i < v1.size(); i++) {
            result += v1[i] * v2[i];
        }
        return result;
    }

    std::vector<float> matrix_vector_mult( std::vector<std::vector<float>> matrix, std::vector<float> vector) {
        std::vector<float> result(matrix.size());
        for (size_t i = 0; i < matrix.size(); i++) {
            result[i] = dot_product(matrix[i], vector);
        }
        return result;
    }

    std::vector<std::vector<int>> kronecker_product( std::vector<std::vector<int>> A,  std::vector<std::vector<int>> B) {
        std::vector<std::vector<int>> result(A.size() * B.size(), std::vector<int>(A[0].size() * B[0].size()));
        for (size_t i = 0; i < A.size(); i++) {
            for (size_t j = 0; j < A[0].size(); j++) {
                for (size_t k = 0; k < B.size(); k++) {
                    for (size_t l = 0; l < B[0].size(); l++) {
                        result[i * B.size() + k][j * B[0].size() + l] = A[i][j] * B[k][l];
                    }
                }
            }
        }
        return result;
    }

    std::vector<std::vector<int>> hadamard_matrix(int n) {
        std::vector<std::vector<int>> H = { {1} };
        for (int i = 0; i < n; i++) {
            H = kronecker_product(H, H);
        }
        return H;
    }


    
};