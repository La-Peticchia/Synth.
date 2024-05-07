/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Enums.h"

//==============================================================================
GCB_SynthAudioProcessorEditor::GCB_SynthAudioProcessorEditor (GCB_SynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComp(p.keyboardState, juce::KeyboardComponentBase::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(keyboardComp);
    keyboardComp.setMidiChannel(2);
    //CustomLookAndFeel customLookAndFeel;

    //slider per l'oscillator
    addAndMakeVisible(dialOscillator);
    //knob.setLookAndFeel(&customLookAndFeel);
    dialOscillator.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dialOscillator.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dialOscillator.setRange(0, 3, 1.5);
    dialOscillator.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dialOscillator.onValueChange = [this]()
        {
            float dialValue = dialOscillator.getValue();

            if (dialValue < 1.0)
            {
                audioProcessor.audioSynth.SetOscillatorWave(WaveType::sine);
            }
            else if (dialValue >= 1.0 && dialValue < 2.0)
            {
                audioProcessor.audioSynth.SetOscillatorWave(WaveType::sawThooth);
            }
            else if (dialValue >= 2.0 && dialValue <= 3.0)
            {
                audioProcessor.audioSynth.SetOscillatorWave(WaveType::triangular);
            }

        };
    addAndMakeVisible(border);
    border.setText("Oscillator");
    
    //slider per la distorzione
    addAndMakeVisible(dialDistortion);
    dialDistortion.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dialDistortion.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dialDistortion.setRange(0, 4, 1);
    dialDistortion.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dialDistortion.onValueChange = [this]()
        {
            float dialValue = dialDistortion.getValue();

            if (dialValue < 1.0)
            {
                audioProcessor.audioSynth.SetDistortionFunction(FunctionType::none);
            }
            else if (dialValue >= 1.0 && dialValue < 2.0)
            {
                audioProcessor.audioSynth.SetDistortionFunction(FunctionType::softClip);
            }
            else if (dialValue >= 2.0 && dialValue <= 3.0)
            {
                audioProcessor.audioSynth.SetDistortionFunction(FunctionType::hardClip);
            }
            else if (dialValue >= 2.5 && dialValue <= 4.0)
            {
                audioProcessor.audioSynth.SetDistortionFunction(FunctionType::waveFold);
            }
        };
    addAndMakeVisible(label1);
    label1.setText("Distortion", juce::dontSendNotification);
    label1.attachToComponent(&dialDistortion, false);
    label1.setFont(12);
   

    //slider per il bias
    addAndMakeVisible(dialBias);
    dialBias.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dialBias.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dialBias.setRange(-1.0, 1.0, 0.1);
    dialBias.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dialBias.onValueChange = [this]() {
        float biasValue = dialBias.getValue();
        audioProcessor.audioSynth.SetDistortionBias(biasValue);
        };

    addAndMakeVisible(label2);
    label2.setText("Bias", juce::dontSendNotification);
    label2.attachToComponent(&dialBias, false);
    label2.setFont(12);


    //slider per il gain
    addAndMakeVisible(dialGain);
    dialGain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dialGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dialGain.setRange(1.0, 5.0, 1.0);
    dialGain.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dialGain.onValueChange = [this]() {
        float gainValue = dialGain.getValue();
        audioProcessor.audioSynth.SetDistortionGain(gainValue);
        };

    addAndMakeVisible(label3);
    label3.setText("Gain", juce::dontSendNotification);
    label3.attachToComponent(&dialGain, false);
    label3.setFont(12);
    
    addAndMakeVisible(border1);
    border1.setText("Waveshaper");
    

    addAndMakeVisible(dialFilter);
    dialFilter.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dialFilter.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dialFilter.setRange(0, 3, 1.5);
    dialFilter.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);

    addAndMakeVisible(border2);
    border2.setText("Filter");

    addAndMakeVisible(buttonOscillator);
    buttonOscillator.setButtonText("Aggiungi Inviluppo");
  
    addAndMakeVisible(buttonFilter);
    buttonFilter.setButtonText("Aggiungi inviluppo");

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

}

void GCB_SynthAudioProcessorEditor::resized()
{
    int keyboardHeight = 130;
    auto topMargin = keyboardHeight + (getHeight() - keyboardHeight) * 0.05;
    auto leftMargin = getWidth() * 0.02;
    auto dialSize = getWidth() * 0.10;
    auto dialSpacing = 15;

    // Imposta le dimensioni e la posizione della tastiera MIDI
    keyboardComp.setBounds(0, getHeight() - keyboardHeight, getWidth(), keyboardHeight);

    // Calcola la larghezza dei bordi
    auto borderWidth = getWidth() * 0.32;

    // Altezza dei bordi
    auto borderHeight = getHeight() - topMargin * 0.1 - dialOscillator.getY() - dialOscillator.getHeight() - dialSpacing - 135;

    // Imposta le dimensioni e la posizione dei bordi
    border.setBounds((getWidth() - borderWidth * 3 - dialSpacing * 2) / 2, topMargin * 0.1, borderWidth, borderHeight);
    border1.setBounds(border.getRight() + dialSpacing, topMargin * 0.1, borderWidth, borderHeight);
    border2.setBounds(border1.getRight() + dialSpacing, topMargin * 0.1, borderWidth, borderHeight);

    // Imposta le dimensioni e la posizione dei dial all'interno dei bordi
    dialOscillator.setBounds(border.getX() + (border.getWidth() - dialSize) / 2, topMargin * 0.3, dialSize, dialSize);
    dialDistortion.setBounds(border1.getX() + dialSpacing, topMargin * 0.6, dialSize, dialSize);
    dialBias.setBounds(border1.getX() + (border1.getWidth() - dialSize) * 0.9, topMargin * 0.6, dialSize, dialSize);
    dialGain.setBounds(border1.getX() + (border1.getWidth() - dialSize) / 2, topMargin * 1.5, dialSize, dialSize);
    dialFilter.setBounds(border2.getX() + (border2.getWidth() - dialSize) / 2, topMargin * 0.3, dialSize, dialSize);

    // Aggiorna la posizione delle etichette
    label1.setBounds(dialDistortion.getX() + (dialSize - label1.getWidth()) + 18, dialDistortion.getY() - 18, label1.getWidth(), 25);
    label2.setBounds(dialBias.getX() + (dialSize - label2.getWidth()) + 33, dialBias.getY() - 18, label2.getWidth(), 25);
    label3.setBounds(dialGain.getX() + (dialSize - label3.getWidth()) + 33, dialGain.getY() - 18, label3.getWidth(), 25);

    // Imposta le dimensioni e la posizione dei bottoni
    auto buttonWidth = 100;
    auto buttonHeight = 30;
    auto buttonSpacing = 20;

    // Posizione del bottone sotto al dialOscillator
    buttonOscillator.setBounds(dialOscillator.getX(), dialOscillator.getBottom() + buttonSpacing, buttonWidth, buttonHeight);

    // Posizione del bottone sotto al dialFilter
    buttonFilter.setBounds(dialFilter.getX(), dialFilter.getBottom() + buttonSpacing, buttonWidth, buttonHeight);

}


