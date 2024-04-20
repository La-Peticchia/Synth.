/*
  ==============================================================================

    AudioSynth.h
    Created: 19 Apr 2024 5:47:17pm
    Author:  Slend

  ==============================================================================
*/

#include <JuceHeader.h>

#pragma once

class AudioSynth : public juce::Synthesiser {
public:
    static const int maxVoiceNumb = 5;

    AudioSynth() {
        //for (int i = 0; i < maxVoiceNumb; i++)
        //    addVoice(new);

        
    }

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        setCurrentPlaybackSampleRate(spec.sampleRate);

        //for (auto* v : voices)
        //    dynamic_cast<Voice*> (v)->prepare(spec);
    }
private:
    
};


class Voice : public juce::SynthesiserVoice {

    
};


