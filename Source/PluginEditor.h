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

class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 10.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(juce::Colours::grey);
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(juce::Colours::black);
        g.drawEllipse(rx, ry, rw, rw, 2.5f);

        // pointer
        juce::Path p;
        auto pointerLength = radius * 0.5f;
        auto pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(juce::Colours::whitesmoke);
        g.fillPath(p);
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style == juce::Slider::LinearHorizontal)
        {
            // Background track
            g.setColour(juce::Colours::black);
            g.fillRect((float)x, (float)(y + height / 2 - 2), (float)width, 4.0f);

            // Fill track
            g.setColour(juce::Colours::white);
            g.fillRect((float)x, (float)(y + height / 2 - 2), sliderPos - (float)x, 4.0f);

            // Vertical rectangle thumb
            g.setColour(juce::Colours::grey);
            g.fillRect((float)(sliderPos - 10), (float)(y + height / 2 - 10), 10.0f, 20.0f);

            // Outline of the rectangle thumb
            g.setColour(juce::Colours::darkgrey);
            g.drawRect((float)(sliderPos - 10), (float)(y + height / 2 - 10), 10.0f, 20.0f, 1.5f);
        }
        else if (style == juce::Slider::LinearVertical)
        {
            // Background track
            g.setColour(juce::Colours::black);
            g.fillRect((float)(x + width / 2 - 2), (float)y, 4.0f, (float)height);

            // Fill track
            g.setColour(juce::Colours::white);
            g.fillRect((float)(x + width / 2 - 2), sliderPos, 4.0f, (float)y + (float)height - sliderPos);

            // Rectangle thumb
            g.setColour(juce::Colours::grey);
            g.fillRect((float)(x + width / 2 - 10), (float)(sliderPos - 10), 20.0f, 10.0f);

            // Outline of the rectangle thumb
            g.setColour(juce::Colours::darkgrey);
            g.drawRect((float)(x + width / 2 - 10), (float)(sliderPos - 10), 20.0f, 10.0f, 1.5f);
        }
    }
};

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
	juce::Label distortionLabel, biasLabel, gainLabel, waveTypeLabel, functionTypeLabel;
	juce::GroupComponent border, border1, border2, border3, border4, oscillatorAttackBorder, oscillatorReleaseBorder, filterAttackBorder, filterReleaseBorder;
	std::vector<std::unique_ptr<juce::Slider>> attackOscillatorSliders, attackFilterSliders, releaseOscillatorSliders, releaseFilterSliders, flangerSliders, delaySliders;
	int numAttackSliders = 4;
	int numReleaseSliders = 3;
	int numFlangerSliders = 4;
	int numDelaySliders = 4;
	juce::ToggleButton distortionToggle, filterToggle, delayToggle, flangerToggle;
	OtherLookAndFeel otherLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GCB_SynthAudioProcessorEditor)
};



