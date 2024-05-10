/*
  ==============================================================================

    AudioSynth.h
    Created: 19 Apr 2024 5:47:17pm
    Author:  Slend

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CustomFilter.h"
#include "CustomOscillator.h"
#include "CustomDistortion.h"
#include "CustomEnvelope.h"
#include "Enums.h"

#pragma once

#define SUSTAIN_VALUE 0.5f
#define SAMPLE_SKIPS 5
struct Sound : public juce::SynthesiserSound
{
    Sound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};


class Voice : public juce::SynthesiserVoice {
    
    public:

        Voice() {
            //gainEnvGen.attacks.add(new Ramp(0.7f, 1.f));
            gainEnvGen.attacks.add(new Ramp(SUSTAIN_VALUE, 1.f));
            //gainEnvGen.releases.add(new Ramp(0.3f, 1.f));
            gainEnvGen.releases.add(new Ramp(0.f, 0.5f));

            cutOffEnvGen.attacks.add(new Ramp(4.f, 0.1f ));
            cutOffEnvGen.attacks.add(new Ramp(0.8f, 0.1f ));
            cutOffEnvGen.releases.add(new Ramp(0.8f, 0.1f ));

            //cutOffEnvGen.attacks.add(new Ramp(3.f, 0.1f ));
            //cutOffEnvGen.attacks.add(new Ramp(0.3, 0.1f));
            //cutOffEnvGen.releases.add(new Ramp(0.3, 0.1f));


        }


        bool canPlaySound(juce::SynthesiserSound* sound) override{
            return dynamic_cast<Sound*> (sound) != nullptr;
        }

        void startNote(int midiNoteNumber, float velocity,
            juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
        {
            for (int i = 0; i < audioSynthRef->getNumVoices(); i++) {
                auto synthVoice = (Voice*)audioSynthRef->getVoice(i);
                if (i != voiceIndex)
                    if (synthVoice->getCurrentlyPlayingNote() == midiNoteNumber) {
                        clearCurrentNote();
                        synthVoice->startNote(midiNoteNumber, velocity, nullptr, 0.0f);
                        return;
                    }
            }

            processorChain.get<oscIndex>().setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));


            gainEnvGen.Attack(getSampleRate()/ SAMPLE_SKIPS);
            cutOffEnvGen.Attack(getSampleRate() / SAMPLE_SKIPS);

        }
    
        void stopNote(float /*velocity*/, bool allowTailOff) override
        {
            if (gainEnvGen.GetCurrentState() == idle || gainEnvGen.GetCurrentState() == release)
                return;

            gainEnvGen.Release(getSampleRate() / SAMPLE_SKIPS);
            cutOffEnvGen.Release(getSampleRate()/ SAMPLE_SKIPS);

        }

        void prepare(const juce::dsp::ProcessSpec& spec)
        {
            tempBlock = juce::dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize);
            gainEnvGen.ResetToZero();
            cutOffEnvGen.ResetToZero();
            processorChain.prepare(spec);
            filter.prepare(spec);
        }
        void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
        {
            if (gainEnvGen.GetCurrentState() == idle)
                return;

            auto block = tempBlock.getSubBlock(0, (size_t)numSamples);
            block.clear();
            juce::dsp::ProcessContextReplacing<float> context(block);
            processorChain.process(context);

            int sampleIndex = 0, nSamp = numSamples, nChannels = outputBuffer.getNumChannels();
            double noteInHertz = juce::MidiMessage::getMidiNoteInHertz(getCurrentlyPlayingNote()), sampleRate = getSampleRate();

            juce::Array<float*> blockPointers;

            for (int i = 0; i < nChannels; i++)
                blockPointers.add(block.getChannelPointer(i));


            nSamp = (numSamples/SAMPLE_SKIPS) + 1;
            sampleIndex = 0;

            //filter.setCutoffFrequency(noteInHertz);

            while (nSamp--)
            {   
                float nextValue = gainEnvGen.GetNextValue(sampleRate/ SAMPLE_SKIPS);
                filter.setCutoffFrequency(juce::jlimit(0., sampleRate / 2, noteInHertz * (cutOffEnvGen.GetNextValue(sampleRate))));
                //filter.setResonance(cutOffEnvGen.GetNextValue(sampleRate));
                for (auto i = SAMPLE_SKIPS; --i >= 0 && sampleIndex < numSamples;) {
                    for (auto k = nChannels; --k >= 0;) 
                        blockPointers[k][sampleIndex] = filter.processSample(k, blockPointers[k][sampleIndex] * nextValue)  ;
                    sampleIndex++;
                }
            }




            if (gainEnvGen.GetCurrentState() != idle) {
                juce::dsp::AudioBlock<float>(outputBuffer)
                .getSubBlock((size_t)startSample, (size_t)numSamples)
                .add(tempBlock);

            }
            else {
                gainEnvGen.ResetToZero();
                clearCurrentNote();
            }

        }

        void pitchWheelMoved(int) override {}
        void controllerMoved(int, int) override {}

        CustomOscillator<float>& GetOscillator() {
            return processorChain.get<oscIndex>();
        }

        CustomDistortion<float>& GetDistortion() {
            return processorChain.get<distortionIndex>();
        }

        void SetAudioSynthRef(juce::Synthesiser* ref, int index) {
            audioSynthRef = ref;
            voiceIndex = index;
        }

    private:
    
        enum
        {
            oscIndex,
            distortionIndex
        };
        
        juce::Synthesiser* audioSynthRef;
        int voiceIndex;

        juce::dsp::ProcessorChain<CustomOscillator<float>, CustomDistortion<float>> processorChain;
        CustomFilter<float> filter;


        juce::dsp::AudioBlock<float> tempBlock;
        juce::HeapBlock<char> heapBlock;

        CustomEnvelope gainEnvGen, cutOffEnvGen;

        envState currentState = idle;

        
};


class AudioSynth : public juce::Synthesiser {
public:
    static const int maxVoiceNumb = 10;
    AudioSynth() {
        for (int i = 0; i < maxVoiceNumb; i++) {

            auto voice = new Voice();
            voice->SetAudioSynthRef(this, i);
            addVoice(voice);
        }
        addSound(new Sound);
        setNoteStealingEnabled(true);
    }

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        setCurrentPlaybackSampleRate(spec.sampleRate);

        for (auto* v : voices)
            dynamic_cast<Voice*> (v)->prepare(spec);
    }

    void SetOscillatorWave(WaveType wave) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->GetOscillator().SetWave(wave);
    }

    void SetDistortionFunction(FunctionType func) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->GetDistortion().SetFunction(func);
    }

    void SetDistortionBias(float bias) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->GetDistortion().SetBias(bias);
    }

    void SetDistortionGain(float gain) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->GetDistortion().SetGain(gain);
    }

private:
    
};






