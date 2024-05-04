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
    addAndMakeVisible(dial);
    //knob.setLookAndFeel(&customLookAndFeel);
    dial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dial.setRange(0, 3, 1.5);
    dial.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dial.onValueChange = [this]()
        {
            float dialValue = dial.getValue();

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
    addAndMakeVisible(dial1);
    dial1.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dial1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dial1.setRange(0, 4, 1);
    dial1.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dial1.onValueChange = [this]()
        {
            float dialValue = dial1.getValue();

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
    label1.attachToComponent(&dial1, false);
    label1.setFont(12);
   

    //slider per il bias
    addAndMakeVisible(dial4);
    dial4.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dial4.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dial4.setRange(-1.0, 1.0, 0.1);
    dial4.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dial4.onValueChange = [this]() {
        float biasValue = dial4.getValue();
        audioProcessor.audioSynth.SetDistortionBias(biasValue);
        };

    addAndMakeVisible(label2);
    label2.setText("Bias", juce::dontSendNotification);
    label2.attachToComponent(&dial4, false);
    label2.setFont(12);


    //slider per il gain
    addAndMakeVisible(dial5);
    dial5.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dial5.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dial5.setRange(1.0, 5.0, 1.0);
    dial5.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dial5.onValueChange = [this]() {
        float gainValue = dial5.getValue();
        audioProcessor.audioSynth.SetDistortionGain(gainValue);
        };

    addAndMakeVisible(label3);
    label3.setText("Gain", juce::dontSendNotification);
    label3.attachToComponent(&dial5, false);
    label3.setFont(12);
    
    addAndMakeVisible(border1);
    border1.setText("Waveshaper");
    

    addAndMakeVisible(dial2);
    dial2.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dial2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dial2.setRange(0, 3, 1.5);
    dial2.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);

    addAndMakeVisible(dial3);
    dial3.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dial3.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dial3.setRange(0, 3, 1.5);
    dial3.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);

    addAndMakeVisible(border2);
    border2.setText("Generatori_di_inviluppo");

    

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
    // Altezza della tastiera MIDI
    int keyboardHeight = 130;

    // Margine superiore: spazio sopra la tastiera MIDI
    auto topMargin = keyboardHeight + (getHeight() - keyboardHeight) * 0.05;

    // Dimensione degli slider
    auto dialSize = getWidth() * 0.15;

    // Spaziatura tra gli slider
    auto dialSpacing = 15;

    // Imposta le dimensioni e la posizione della tastiera MIDI
    keyboardComp.setBounds(0, getHeight() - keyboardHeight, getWidth(), keyboardHeight);

    // Calcola la larghezza totale degli slider e la posizione x del primo slider per centrarli
    auto totalDialWidth = dialSize * 4 + dialSpacing * 3;
    auto firstDialX = (getWidth() - totalDialWidth) / 2;

    // Imposta le dimensioni e la posizione degli slider
    dial.setBounds(firstDialX, topMargin * 0.3, dialSize, dialSize);
    dial1.setBounds(dial.getRight() + dialSpacing, dial.getY(), dialSize, dialSize);
    dial4.setBounds(dial1.getRight() + dialSpacing, dial.getY(), dialSize, dialSize);
    dial5.setBounds(dial4.getRight() + dialSpacing, dial.getY(), dialSize, dialSize);

    // Imposta le dimensioni e la posizione dei bordi attorno agli slider
    border.setBounds(firstDialX, topMargin * 0.1, dialSize, dial.getY() + dial.getHeight());
    border1.setBounds(dial1.getX(), topMargin * 0.1, dialSize * 2.9 + dialSpacing * 3, dial.getY() + dial.getHeight());

    // Calcola la posizione x e y del bordo 2 e imposta le sue dimensioni e posizione
    auto totalBorder2Width = dialSize * 2 + dialSpacing * 2;
    auto border2X = (getWidth() - totalBorder2Width) / 2;
    auto border2Y = dial.getY()+10 + dial.getHeight() + dialSpacing;

    border2.setBounds(border2X, border2Y, totalBorder2Width, dial.getY() + dial.getHeight() - 15);

    // Imposta le dimensioni e la posizione degli slider all'interno del bordo 2
    dial2.setBounds(border2X + dialSpacing - 10, border2Y + dialSpacing, dialSize, dialSize);
    dial3.setBounds(dial2.getRight() + dialSpacing, border2Y + dialSpacing, dialSize, dialSize);

    label1.setBounds(dial1.getX() + 44, topMargin * 0.1 + 15, dialSize * 2.9 + dialSpacing * 3, 25);
    label2.setBounds(dial4.getX() + 60, topMargin * 0.1 + 15, dialSize * 2.9 + dialSpacing * 3, 25);
    label3.setBounds(dial5.getX() + 57, topMargin * 0.1 + 15, dialSize * 2.9 + dialSpacing * 3, 25);
    
}
