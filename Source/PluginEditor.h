/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

class GCB_SynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GCB_SynthAudioProcessorEditor (GCB_SynthAudioProcessor&);
    ~GCB_SynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GCB_SynthAudioProcessor& audioProcessor;
    juce::MidiKeyboardComponent keyboardComp;

    juce::Slider knob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GCB_SynthAudioProcessorEditor)
};


