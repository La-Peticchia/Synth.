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

    //CustomLookAndFeel customLookAndFeel;

    //aggiungo lo slider
    addAndMakeVisible(knob);
   // knob.setLookAndFeel(&customLookAndFeel);
    knob.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 32);
    knob.setRange(0, 3, 1.5);
    knob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);

    addAndMakeVisible(border);
    border.setText("Tipo_forma_d'onda");



    //metodi per resizare la window
    //getConstrainer()->setFixedAspectRatio(2.0);
    //setResizable(true, true);
    //setResizeLimits(500,250,1200,600);
    setSize (1000, 550);
}

GCB_SynthAudioProcessorEditor::~GCB_SynthAudioProcessorEditor()
{
}

//==============================================================================
void GCB_SynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
   // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("", getLocalBounds(), juce::Justification::centred, 1);
}

void GCB_SynthAudioProcessorEditor::resized()
{
    int keyboardHeight = 100;
    auto leftMargin = getWidth() * 0.02;
    auto topMargin = keyboardHeight + (getHeight() - keyboardHeight) * 0.25; // Spazio sopra la tastiera MIDI
    auto knobSize = getWidth() * 0.2;

    // Imposto le dimensioni della tastiera MIDI
    keyboardComp.setBounds(0, 0, getWidth(), keyboardHeight);

    // Imposto le dimensioni e la posizione dello slider
    knob.setBounds(leftMargin, topMargin, knobSize, knobSize);

    border.setBounds(leftMargin, topMargin * 0.5, knobSize, knob.getY() + knob.getHeight());
}
