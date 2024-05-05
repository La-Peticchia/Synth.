/*
  ==============================================================================

    CustomOscillator.h
    Created: 25 Apr 2024 8:30:57am
    Author:  Slend

  ==============================================================================
*/

#pragma once
#include "Enums.h"



template <typename Type>
class CustomOscillator{

public:

    CustomOscillator() {
        SetWave(sine);
    }

    void SetWave(WaveType waveT) {

        switch (waveT) {

        case sine:
            osc.initialise([](Type x) { return std::sin(x); }, 128);
            break;

        case sawThooth:
            osc.initialise([](Type x)
                {
                    return juce::jmap(x,
                    Type(-juce::MathConstants<double>::pi),
                    Type(juce::MathConstants<double>::pi),
                    Type(-1),
                    Type(1));

                }, 2);
            break;                                                 

        case triangular:
            osc.initialise([](Type x) {

                auto val = juce::jmap(x,
                Type(-juce::MathConstants<double>::pi),
                Type(juce::MathConstants<double>::pi),
                Type(-1),
                Type(1));

                return Type(2) * std::fabs(fmod(val, Type(2)) - Type(1)) - Type(1);

                }, 128);
            break;
        case squareWave:
            osc.initialise([](Type x) {
                return jlimit(Type(-1), Type(1), sin(x) * Type(50));
                }, 128);
            break;

        }
    }

    void setFrequency(Type newValue, bool force = false)
    {
        osc.setFrequency(newValue, force);     // [7]
    }


    void reset() noexcept
    {
        osc.reset(); // [4]
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        osc.process(context);       // [9]
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        osc.prepare(spec); // [3]
    }


private:
    juce::dsp::Oscillator<Type> osc;
};