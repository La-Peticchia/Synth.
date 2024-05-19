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

class GCB_SynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    GCB_SynthAudioProcessorEditor(GCB_SynthAudioProcessor&);
    ~GCB_SynthAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GCB_SynthAudioProcessor& audioProcessor;
    juce::MidiKeyboardComponent keyboardComp;

    juce::Slider dialOscillator, dialDistortion, dialFilter, dialBias, dialGain;
    juce::Label label1, label2, label3, label4, waveTypeLabel, functionTypeLabel;
    juce::GroupComponent border, border1, border2, border3, border4, oscillatorAttackBorder, oscillatorReleaseBorder, filterAttackBorder, filterReleaseBorder;
    std::vector<std::unique_ptr<juce::Slider>> attackOscillatorSliders, attackFilterSliders, releaseOscillatorSliders, releaseFilterSliders, flangerSliders, delaySliders;
    int numAttackSliders = 4;
    int numReleaseSliders = 3;
    int numDelaySliders = 2;
    juce::ToggleButton distortionToggle, filterToggle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GCB_SynthAudioProcessorEditor)
};

