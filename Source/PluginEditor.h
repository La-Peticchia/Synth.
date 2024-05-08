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

    juce::Slider dialOscillator, dialDistortion, dialFilter, dialBias, dialGain;
    juce::Label label1, label2, label3;
    juce::GroupComponent border, border1, border2;
    juce::TextButton buttonOscillatorAdd, buttonOscillatorRemove, buttonFilter, buttonFilter1;
    std::vector<std::unique_ptr<juce::Slider>> verticalSliders;
    int maxVerticalSliders = 5;


   /*class CustomLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
            const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
        {
            const float diameter = juce::jmin(width, height) - 4.0f;
            const float radius = diameter / 2.0f;
            const float centreX = x + width * 0.5f;
            const float centreY = y + height * 0.5f;
            const float rx = centreX - radius;
            const float ry = centreY - radius;

            // Disegna il cerchio esterno
            g.setColour(juce::Colours::darkgrey);
            g.fillEllipse(rx, ry, diameter, diameter);

            // Disegna la manopola
            const float sliderRadius = diameter * 0.4f;
            const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            const float knobX = centreX + radius * std::cos(angle - juce::MathConstants<float>::halfPi);
            const float knobY = centreY + radius * std::sin(angle - juce::MathConstants<float>::halfPi);
            g.setColour(juce::Colours::lightgrey);
            g.fillEllipse(knobX - sliderRadius, knobY - sliderRadius, sliderRadius * 2.0f, sliderRadius * 2.0f);

            // Disegna il punto centrale
            g.setColour(juce::Colours::white);
            g.fillEllipse(centreX - sliderRadius * 0.2f, centreY - sliderRadius * 0.2f, sliderRadius * 0.4f, sliderRadius * 0.4f);
        }
    };*/ 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GCB_SynthAudioProcessorEditor)
};


