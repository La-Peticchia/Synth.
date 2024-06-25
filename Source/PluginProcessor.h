/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AudioSynth.h"
#include "CustomDelay.h"
#include "CustomFilter.h"
#include "CustomFlanger.h"
#include "Enums.h"

//==============================================================================
/**
*/
class GCB_SynthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GCB_SynthAudioProcessor();
    ~GCB_SynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::MidiMessageCollector* getMidiCollector()
    {
        return &midiMessageCollector;
    }

    juce::MidiKeyboardState keyboardState;
    AudioSynth audioSynth;
    CustomDelay delay ;
    CustomFlanger flanger;
    
    juce::dsp::Limiter<float> limiter;
    LPButtFilter finalLPFilter;
private:
    juce::MidiMessageCollector midiMessageCollector;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GCB_SynthAudioProcessor)
};


