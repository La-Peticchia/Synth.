/*
  ==============================================================================

    AudioSynth.h
    Created: 19 Apr 2024 5:47:17pm
    Author:  Slend

  ==============================================================================
*/

#include <JuceHeader.h>

#pragma once

template <typename Type>
class CustomOscillator : juce::dsp::Oscillator<Type> {

public:

    CustomOscillator() {

        switch (currentState) {

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

        }
    }

    void setFrequency(Type newValue, bool force = false)
    {
        osc.setFrequency(newValue, force);     // [7]
    }

    //==============================================================================
    //void setLevel(Type newValue)
    //{
    //    auto& gain = processorChain.template get<gainIndex>();
    //    gain.setGainLinear(newValue);          // [8]
    //}

    //==============================================================================
    void reset() noexcept
    {
        osc.reset(); // [4]
    }

    //==============================================================================
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        osc.process(context);       // [9]
    }

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        osc.prepare(spec); // [3]
    }


    enum WaveType {
        sawThooth,
        squareWave,
        triangular
    } currentState = sawThooth;

private:
    //juce::dsp::ProcessorChain<juce::dsp::Oscillator<Type>, juce::dsp::Gain<Type>> processorChain;
    juce::dsp::Oscillator<Type> osc;
};

struct Sound : public juce::SynthesiserSound
{
    Sound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};


class Voice : public juce::SynthesiserVoice {
    
    public:

        Voice() {
            processorChain.get<gainIndex>().setRampDurationSeconds(0.01);
        }


        bool canPlaySound(juce::SynthesiserSound* sound) override{
            return dynamic_cast<Sound*> (sound) != nullptr;
        }
    
        void startNote(int midiNoteNumber, float velocity,
            juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
        {
            processorChain.get<oscIndex>().setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
            processorChain.get<gainIndex>().setGainLinear(velocity);
        }
    
        void stopNote(float /*velocity*/, bool allowTailOff) override
        {
            clearCurrentNote();
        }

        void prepare(const juce::dsp::ProcessSpec& spec)
        {
            tempBlock = juce::dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize);
            processorChain.prepare(spec);
        }

        void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
        {
            auto block = tempBlock.getSubBlock(0, (size_t)numSamples);
            block.clear();
            juce::dsp::ProcessContextReplacing<float> context(block);
            processorChain.process(context);

            juce::dsp::AudioBlock<float>(outputBuffer)
                .getSubBlock((size_t)startSample, (size_t)numSamples)
                .add(tempBlock);
        }

        void pitchWheelMoved(int) override {}
        void controllerMoved(int, int) override {}

    private:
    
        enum
        {
            oscIndex,
            gainIndex
        };

        juce::dsp::ProcessorChain<CustomOscillator<float>, juce::dsp::Gain<float>> processorChain;
        juce::dsp::AudioBlock<float> tempBlock;
        juce::HeapBlock<char> heapBlock;

};


class AudioSynth : public juce::Synthesiser {
public:
    static const int maxVoiceNumb = 5;

    AudioSynth() {
        for (int i = 0; i < maxVoiceNumb; i++) {

            addVoice(new Voice);
        }
        addSound(new Sound);
        
    }

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        setCurrentPlaybackSampleRate(spec.sampleRate);

        for (auto* v : voices)
            dynamic_cast<Voice*> (v)->prepare(spec);
    }

private:
    
};






