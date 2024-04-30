/*
  ==============================================================================

    AudioSynth.h
    Created: 19 Apr 2024 5:47:17pm
    Author:  Slend

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CustomOscillator.h"
#include "CustomDistortion.h"
#include "Enums.h"

#pragma once

struct Sound : public juce::SynthesiserSound
{
    Sound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};


struct Ramp {
public:
    float targetValue, duration;
    Ramp(float tVal, float dur) {
        targetValue = tVal;
        duration = dur;
    }
private:

};

class Voice : public juce::SynthesiserVoice {
    
    public:

        Voice() {
            attacks.add(new Ramp(1.5f, 1.f));
            attacks.add(new Ramp(1.f, 1.f));

            releases.add(new Ramp(0.f, 1.f));
            env.setSampleRate(getSampleRate());
            env.setParameters(juce::ADSR::Parameters(1.f, 0.3f, 0.7f, 1.f));
        }


        bool canPlaySound(juce::SynthesiserSound* sound) override{
            return dynamic_cast<Sound*> (sound) != nullptr;
        }

        void startNote(int midiNoteNumber, float velocity,
            juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
        {
            DBG("note on: " << midiNoteNumber);
            env.noteOn();
            processorChain.get<oscIndex>().setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
            
            /* ADSR try */
            
            rampCount = 0;

            if (currentState == release) {

                float prevTargetValue = 0;
                for (int i = 0; i < attacks.size(); i++)
                {

                    if ((attacks[i]->targetValue > prevTargetValue && attacks[i]->targetValue > gain.getCurrentValue()) || (attacks[i]->targetValue < prevTargetValue && attacks[i]->targetValue < gain.getCurrentValue()) ) {
                        gain.setTargetValue(attacks[i]->targetValue);
                        break;
                    }

                    rampCount++;
                    prevTargetValue = attacks[i]->targetValue;
                }
            }
            else
            {
                gain.setTargetValue(attacks[0]->targetValue);
                rampCount++;
            }
            


            currentState = attack;
        }
    
        void stopNote(float /*velocity*/, bool allowTailOff) override
        {
            env.noteOff();
            DBG("note off");

            currentState = release;
            /* ADSR try
            
            rampCount = 0;

            if (currentState == attack) {

                float prevTargetValue = 0;
                for (int i = 0; i < releases.size(); i++)
                {

                    if ((releases[i]->targetValue > prevTargetValue && releases[i]->targetValue > gain.getCurrentValue()) || (releases[i]->targetValue < prevTargetValue && releases[i]->targetValue < gain.getCurrentValue())) {
                        gain.setTargetValue(releases[i]->targetValue);
                        break;
                    }

                    rampCount++;
                    prevTargetValue = releases[i]->targetValue;
                }
            }
            currentState = release;
            */


            //Debug
            gain.setCurrentAndTargetValue(0);
            clearCurrentNote();

        }

        void prepare(const juce::dsp::ProcessSpec& spec)
        {
            tempBlock = juce::dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize);
            gain.reset(getSampleRate(), attacks[0]->duration);
            gain.setCurrentAndTargetValue(0);
            processorChain.prepare(spec);
            
        }
        void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
        {
            auto block = tempBlock.getSubBlock(0, (size_t)numSamples);
            block.clear();
            juce::dsp::ProcessContextReplacing<float> context(block);
            processorChain.process(context);

            int sampleIndex = 0, nSamp = numSamples, nChannels = outputBuffer.getNumChannels();


            juce::Array<float*> blockPointers;

            for (int i = 0; i < nChannels; i++)
                blockPointers.add(block.getChannelPointer(1));
            if(!juce::approximatelyEqual(gain.getCurrentValue(), 0.f))
            DBG(gain.getCurrentValue());

            if (currentState == attack) {


                while (nSamp--) {
                    float nextValue = gain.getNextValue();

                    for (auto i = nChannels; --i >= 0;)
                        blockPointers[i][sampleIndex] *= nextValue;

                    sampleIndex++;
                    //DBG(std::fabs(gain.getTargetValue() - gain.getCurrentValue()));
                    if (std::fabs(gain.getTargetValue() - gain.getCurrentValue()) < 0.01f) {
                        if (rampCount == attacks.size()) {
                            currentState = sustain;
                            break;
                        }

                        //gain.reset(getSampleRate(), attacks[rampCount]->duration);
                        gain.setTargetValue(attacks[rampCount++]->targetValue);
                    }
                }
                
            }
            
            if(isKeyDown())
            juce::dsp::AudioBlock<float>(outputBuffer)
                .getSubBlock((size_t)startSample, (size_t)numSamples)
                .add(tempBlock);
        
            /*
            auto pointer1 = block.getChannelPointer(0);
            auto pointer2 = block.getChannelPointer(1);
            
            if (currentState == attack)
            {
                //DBG("attacking");
                //DBG(block.getNumSamples());

                while (nSamp--)
                {  
                    //DBG(sampleIndex);

                    pointer1[sampleIndex] *= 1.5f;
                    pointer2[sampleIndex++] *= 1.5f;
                }

            }
            else if (currentState == release)
            {
                clearCurrentNote();
                processorChain.get<gainIndex>().setGainLinear(0);
            }
        */

            //if (env.isActive()) {
            //    
            //    env.applyEnvelopeToBuffer(outputBuffer,startSample, numSamples);
            //}
            //else
            //{
            //    clearCurrentNote();
            //    processorChain.get<gainIndex>().setGainLinear(0);
            //}

            /* DEBUG 
            //if(env.isActive())
            //while (numSamples--)
            //{
            //    DBG(outputBuffer.getSample(0, startSample++));
            //}

            int numChannels = outputBuffer.getNumChannels();
            juce::Array<float*> buffers;
            for (size_t i = 0; i < numChannels; i++)
                buffers.add(outputBuffer.getWritePointer(i));
            outputBuffer.applyGainRamp(startSample, numSamples, c, c += (c <= 1)? 0.01f : 0);
            */

            /* ADSR try
            if (currentState == attack) {

                while (--nSamp >= 0) {
                    float nextValue = gain.getNextValue();

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.setSample(i, sampleIndex, outputBuffer.getSample(i, sampleIndex) * nextValue);


                    //DBG(std::fabs(gain.getTargetValue() - gain.getCurrentValue()));
                    sampleIndex++;
                    if (std::fabs(gain.getTargetValue() - gain.getCurrentValue()) < 0.01f) {
                        if (rampCount == attacks.size()) {
                            currentState = sustain;
                            goto Sustain;
                        }

                        //gain.reset(getSampleRate(), attacks[rampCount]->duration);
                        gain.setTargetValue(attacks[rampCount++]->targetValue);
                    }
                }

                if (rampCount == attacks.size())
                    currentState = sustain;

            }
            else if(currentState == release)
            {
                while (--numSamples >= 0) {
                    float nextValue = gain.getNextValue();
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.setSample(i, sampleIndex, outputBuffer.getSample(i, sampleIndex) * nextValue);
                    sampleIndex++;
                    if (std::fabs(gain.getTargetValue() - gain.getCurrentValue()) < 0.01f) {
                        if (rampCount == attacks.size()) {
                            currentState = silence;
                            goto Silence;
                        }

                        //gain.reset(getSampleRate(), releases[rampCount]->duration);
                        gain.setTargetValue(releases[rampCount++]->targetValue);
                    }

                }

                if (rampCount == releases.size())
                    gain.setTargetValue(0.f);

            }
            else if(currentState == sustain)
            {
                Sustain:
                while (--numSamples >= 0) {
                    float nextValue = gain.getNextValue();
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.setSample(i, sampleIndex, outputBuffer.getSample(i, sampleIndex) * nextValue);
                    sampleIndex++;
                }
                return;
            }
            else
            {
                Silence:
                if (gain.getTargetValue() != 0.f)
                    gain.setTargetValue(0.f);

                while (--numSamples >= 0) {
                    float nextValue = gain.getNextValue();
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.setSample(i, sampleIndex, outputBuffer.getSample(i, sampleIndex) * nextValue);
                    sampleIndex++;
                    
                    if (std::fabs(gain.getTargetValue() - gain.getCurrentValue()) < 0.01f) {

                        processorChain.get<gainIndex>().setGainLinear(0);
                        clearCurrentNote();
                    }
                }
                return;
            }

            */
        }

        void pitchWheelMoved(int) override {}
        void controllerMoved(int, int) override {}

    private:
    
        enum
        {
            oscIndex,
            distortionIndex
        };
        
        juce::dsp::ProcessorChain<CustomOscillator<float>, CustomDistortion<float>> processorChain;
        //enum
        //{
        //    oscIndex,
        //    gainIndex
        //};
        //juce::dsp::ProcessorChain<CustomOscillator<float>, juce::dsp::Gain<float>> processorChain;

        juce::dsp::AudioBlock<float> tempBlock;
        juce::HeapBlock<char> heapBlock;
        juce::LinearSmoothedValue<float> gain { 0.0f };
        juce::ADSR env;
        

        juce::OwnedArray<Ramp> attacks;
        juce::OwnedArray<Ramp> releases;
        int rampCount = 0;

        envState currentState = silence;


};


class AudioSynth : public juce::Synthesiser {
public:
    static const int maxVoiceNumb = 5;

    AudioSynth() {
        for (int i = 0; i < maxVoiceNumb; i++) {

            addVoice(new Voice);
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

private:
    
};






