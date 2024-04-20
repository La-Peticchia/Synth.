/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GCB_SynthAudioProcessorEditor::GCB_SynthAudioProcessorEditor (GCB_SynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComp(p.keyboardState, juce::KeyboardComponentBase::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(keyboardComp);
    keyboardComp.setMidiChannel(2);
    setSize (600, 200);
}

GCB_SynthAudioProcessorEditor::~GCB_SynthAudioProcessorEditor()
{
}

//==============================================================================
void GCB_SynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void GCB_SynthAudioProcessorEditor::resized()
{
    keyboardComp.setBounds(0, 0, getWidth(), getHeight());
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
