/*
  ==============================================================================

    CustomDistortion.h
    Created: 25 Apr 2024 8:31:13am
    Author:  Slend

  ==============================================================================
*/

#pragma once

#define SMOOTHING_TIME 0.01

enum FunctionType {
    softClip,
    hardClip,
    waveFold,
    none
};

template <typename Type>
class CustomDistortion {
public:

    CustomDistortion() {
        SetFunction(none);
        processorChain.get<gainIndex>().setRampDurationSeconds(SMOOTHING_TIME);
        processorChain.get<biasIndex>().setRampDurationSeconds(SMOOTHING_TIME);

        SetGain(1);
        SetBias(0);
    }


    void SetFunction(FunctionType funcT) {

        auto& waveShaper = processorChain.get<shaperIndex>();

        switch (funcT)
        {
        case softClip:
                waveShaper.functionToUse = [](Type x)
                    {
                        return juce::jlimit(Type(-1), Type(1), tanh(x));
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
        processorChain.prepare(spec);
        
    }


    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        processorChain.process(context); 
    }

    void reset() noexcept
    {
        processorChain.reset(); 
    }


    void SetGain(float val) {
        processorChain.get<gainIndex>().setGainLinear(juce::jlimit(minGain, maxGain, val));
    }

    void SetBias(float val) {
        processorChain.get<biasIndex>().setBias(juce::jlimit(minBias, maxBias, val));
    }

private:
    enum 
    {
        biasIndex,
        gainIndex,
        shaperIndex
    };
    juce::dsp::ProcessorChain<juce::dsp::Bias<Type>, juce::dsp::Gain<Type>, juce::dsp::WaveShaper<Type>> processorChain;

    float maxGain = 5, minGain = 1 / 5;
    float maxBias = 1, minBias = -1;
};