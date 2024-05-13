/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
using namespace juce;

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

    juce::Slider dialOscillator, dialDistortion, dialFilter, dialBias, dialGain, releseTimeSlider;
    juce::Label label1, label2, label3, label4;
    juce::GroupComponent border, border1, border2, oscillatorAdd, oscillatorRelese, filterAdd, filterRelese;
    std::vector<std::unique_ptr<juce::Slider>> oscillatorSliders, filterSliders, releseOscillatorSliders, releseFilterSliders;
    int maxVerticalSliders = 3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GCB_SynthAudioProcessorEditor)
};


