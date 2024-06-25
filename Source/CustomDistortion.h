/*
  ==============================================================================

    CustomDistortion.h
    Created: 25 Apr 2024 8:31:13am
    Author:  Slend

  ==============================================================================
*/

#pragma once
#include <typeinfo>
#include <JuceHeader.h>
#include "Enums.h"
#include "Defines.h"

template <typename Type>
class CustomDistortion {
public:

    CustomDistortion() {
        SetFunction(softClip);
        processorChain.get<gainIndex>().setRampDurationSeconds(DEFAULT_RAMP_DURATION);
        processorChain.get<biasIndex>().setRampDurationSeconds(DEFAULT_RAMP_DURATION);

        SetGain(minGain);
        SetBias(0);
    }


    void SetFunction(FunctionType funcT) {
        auto& waveShaper = processorChain.get<shaperIndex>();
        switch (funcT)
        {
        case softClip:
                waveShaper.functionToUse = [](Type x)
                    {
                        return juce::dsp::FastMathApproximations::tanh(x);
                    };
                break;

        case hardClip:
            waveShaper.functionToUse = [](Type x)
                {
                    return juce::jlimit(Type(-1), Type(1), x);
                };
                break;

        case waveFold:
            waveShaper.functionToUse = [](Type x)
                {
                    return Type(2) * std::fabs(fmod(x,Type(2)) - Type(1)) - Type(1);
                };
                break;
        case absolute: 
            waveShaper.functionToUse = [](Type x)
                {
                    return juce::jlimit(Type(-1), Type(1), std::fabs(x) - Type(1));
                };
            break;
        default:
            waveShaper.functionToUse = [](Type x)
                {
                    return x;
                };
            break;
        }

    }
    
    void prepare(const juce::dsp::ProcessSpec& spec)
    {        

        juce::dsp::ProcessSpec overSampSpec{ spec.sampleRate * std::pow(2, OVERSAMPLING_FACTOR), spec.maximumBlockSize * std::pow(2, OVERSAMPLING_FACTOR), spec.numChannels };
        //processorChain.prepare(spec); //
        processorChain.prepare(overSampSpec);

        overSamp.initProcessing(overSampSpec.maximumBlockSize);
        
    }


    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {

        auto outBuffer = context.getOutputBlock();
        juce::dsp::ProcessContextReplacing<float> overSampContext(overSamp.processSamplesUp(outBuffer));

        if (enabled) {
            //processorChain.process(context); //
            processorChain.process(overSampContext);
        }
        
        overSamp.processSamplesDown(outBuffer);

    }

    void reset() noexcept
    {
        processorChain.reset(); 
        overSamp.reset();
    }


    void SetGain(float val) {
        processorChain.get<gainIndex>().setGainLinear(juce::jlimit(minGain, maxGain, val));
    }

    void SetBias(float val) {
        processorChain.get<biasIndex>().setBias(juce::jlimit(minBias, maxBias, val));
    }

    bool enabled = false;

private:
    enum 
    {
        gainIndex,
        biasIndex,
        shaperIndex,
        bias1Index
    };
    juce::dsp::ProcessorChain<juce::dsp::Gain<Type>, juce::dsp::Bias<Type>, juce::dsp::WaveShaper<Type>> processorChain;
    juce::dsp::Oversampling<Type> overSamp = juce::dsp::Oversampling<Type>(Type(2), Type(1), juce::dsp::Oversampling<Type>::FilterType::filterHalfBandPolyphaseIIR, false, true);

    float maxGain = MAX_GAIN, minGain = MIN_GAIN;
    float maxBias = 1, minBias = -1;


};