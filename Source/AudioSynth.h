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
#include "Defines.h"

#pragma once



struct Sound : public juce::SynthesiserSound
{
    Sound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class AudioSynth;

class Voice : public juce::SynthesiserVoice {
    
    public:

        Voice() {
            //gainEnvGen.attacks.add(new Ramp(0.7f, 1.f));
            gainEnvGen.attacks.add(new Ramp(SUSTAIN_VALUE, DEFAULT_RAMP_DURATION));
            //gainEnvGen.releases.add(new Ramp(0.3f, 1.f));
            gainEnvGen.releases.add(new Ramp(0.f, DEFAULT_RAMP_DURATION));

            cutOffEnvGen.attacks.add(new Ramp(SUSTAIN_VALUE, DEFAULT_RAMP_DURATION));
            cutOffEnvGen.releases.add(new Ramp(0.f, DEFAULT_RAMP_DURATION));

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

            float noteInHertz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
            GetOscillator().setFrequency(noteInHertz);
            //hpFilter.SetCoefficents(getSampleRate(), noteInHertz);
            hpFilter.SetCoefficents(coeffRef->GetCoefficient(midiNoteNumber));

            gainEnvGen.Attack();
            cutOffEnvGen.Attack();

        }
    
        void stopNote(float /*velocity*/, bool allowTailOff) override
        {
            if (gainEnvGen.GetCurrentState() == idle || gainEnvGen.GetCurrentState() == release)
                return;

            gainEnvGen.Release();
            cutOffEnvGen.Release();

        }

        void prepare(const juce::dsp::ProcessSpec& spec)
        {
            tempBlock = juce::dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize);
            gainEnvGen.ResetToZero(GetSkippedSampleRate());
            cutOffEnvGen.ResetToZero(GetSkippedSampleRate());
            processorChain.prepare(spec);
            lpFilter.prepare(spec);
            hpFilter.prepare(spec);
        }
        void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
        {
            if (gainEnvGen.GetCurrentState() == idle)
                return;

            auto block = tempBlock.getSubBlock(0, (size_t)numSamples);
            block.clear();
            juce::dsp::ProcessContextReplacing<float> context(block);

            //Oscillator and Distortion processing
            processorChain.process(context);

#pragma region Envelope and LowPass Filter Processing


            int sampleIndex = 0, nSamp = numSamples/SAMPLE_SKIPS + 1, nChannels = outputBuffer.getNumChannels();
            double noteInHertz = juce::MidiMessage::getMidiNoteInHertz(getCurrentlyPlayingNote()), sampleRate = getSampleRate();

            juce::Array<float*> blockPointers;

            for (int i = 0; i < nChannels; i++)
                blockPointers.add(block.getChannelPointer(i));

            while (nSamp--)
            {   
                float nextValue = gainEnvGen.GetNextValue();
                lpFilter.setCutoffFrequency(juce::jlimit(0., (sampleRate / 2) - 1, noteInHertz * (cutOffEnvGen.GetNextValue() + 1)));
                for (auto i = SAMPLE_SKIPS; --i >= 0 && sampleIndex < numSamples;) {
                    for (auto k = nChannels; --k >= 0;) 
                        blockPointers[k][sampleIndex] = lpFilter.processSample(k, blockPointers[k][sampleIndex] * nextValue)  ;
                    sampleIndex++;
                }
            }
                
                

            //HighPass Filter processing
            
            hpFilter.process(context);

#pragma endregion


            if (gainEnvGen.GetCurrentState() != idle) {
                juce::dsp::AudioBlock<float>(outputBuffer)
                .getSubBlock((size_t)startSample, (size_t)numSamples)
                .add(tempBlock);

            }
            else {
                gainEnvGen.ResetToZero(GetSkippedSampleRate());
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

        void SetAudioSynthRef(juce::Synthesiser* ref, int index, HPFilterCoefficients* cRef) {
            audioSynthRef = ref;
            voiceIndex = index;
            coeffRef = cRef;
        }

        void AddEnvelopeRamp(EnvType type, EnvState state, float value) {
            
            if (state != attack && state != release)
                return;
            auto currentGen = (state == gainEnv) ? &gainEnvGen : &cutOffEnvGen;
            auto currentRamps = (type == attack) ? &currentGen->attacks : &currentGen->releases;
            currentRamps->insert(currentRamps->size()-1, new Ramp(value, DEFAULT_RAMP_DURATION));
        }

        void RemoveEnvelopeRamp(EnvType type, EnvState state) {

            if (state != attack && state != release)
                return;
            auto currentGen = (type == gainEnv) ? &gainEnvGen : &cutOffEnvGen;
            auto currentRamps = (state == attack) ? &currentGen->attacks : &currentGen->releases;
            currentRamps->remove(currentRamps->size() - 1, true);
        }

        void SetRampTargetValue(EnvType type, EnvState state, int index, float value) {
            if (state != attack && state != release)
                return;
            auto currentGen = (type == gainEnv) ? &gainEnvGen : &cutOffEnvGen;
            auto currentRamps = (state == attack) ? &currentGen->attacks : &currentGen->releases;
            (*currentRamps)[index]->targetValue = value;

        }

        void SetEnvDuration(EnvType type, float value) {
            auto currentGen = (type == gainEnv) ? &gainEnvGen : &cutOffEnvGen;
            currentGen->SetEnvDuration(value, getSampleRate());
        }



        SVFilter<float> lpFilter;

    private:
    
        enum
        {
            oscIndex,
            distortionIndex
        };
        
        juce::Synthesiser* audioSynthRef;
        int voiceIndex;
        HPFilterCoefficients* coeffRef;

        juce::dsp::ProcessorChain<CustomOscillator<float>, CustomDistortion<float>> processorChain;
        HPFilter<float> hpFilter;

        juce::dsp::AudioBlock<float> tempBlock;
        juce::HeapBlock<char> heapBlock;

        CustomEnvelope gainEnvGen, cutOffEnvGen;

        EnvState currentState = idle;

        double GetSkippedSampleRate() {
            return getSampleRate() / SAMPLE_SKIPS;
        }

        
};


class AudioSynth : public juce::Synthesiser {
public:
    static const int maxVoiceNumb = 10;
    AudioSynth() {
        for (int i = 0; i < maxVoiceNumb; i++) {

            auto voice = new Voice();
            voice->SetAudioSynthRef(this, i, &hpCoeff);
            addVoice(voice);
        }
        addSound(new Sound);
        setNoteStealingEnabled(true);
    }

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        setCurrentPlaybackSampleRate(spec.sampleRate);
        hpCoeff = HPFilterCoefficients(spec.sampleRate);

        for (auto* v : voices)
            dynamic_cast<Voice*> (v)->prepare(spec);

    }

#pragma region Oscillator interface

    void SetOscillatorWave(WaveType wave) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->GetOscillator().SetWave(wave);
    }

#pragma endregion

#pragma region Waveshaper interface
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

    void SetDistortionActive(bool state) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->GetDistortion().enabled = state;
    }

#pragma endregion

#pragma region LPFilter interface
    void SetLPFilterResonance(double q) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->lpFilter.setResonance(q);
    }

    void SetLPFilterActive(bool state) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->lpFilter.enabled = state;
    }

#pragma endregion


#pragma region Envelope interface
    void AddEnvelopeRamp(EnvType type, EnvState state, float value) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->AddEnvelopeRamp(type, state, value);
    }

    void RemoveEnvelopeRamp(EnvType type, EnvState state) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->RemoveEnvelopeRamp(type, state);
    }

    void SetRampTargetValue(EnvType type, EnvState state, int index, float value) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->SetRampTargetValue(type, state, index, value);
    }

    void SetEnvelopeDuration(EnvType type, float value) {
        for (int i = 0; i < getNumVoices(); i++)
            dynamic_cast<Voice*>(getVoice(i))->SetEnvDuration(type, value);
    }

#pragma endregion




    HPFilterCoefficients hpCoeff;

private:
    

};






