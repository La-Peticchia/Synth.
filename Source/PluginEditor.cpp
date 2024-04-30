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

    //Aggiungo lo slider
    addAndMakeVisible(dial);
    //knob.setLookAndFeel(&customLookAndFeel);
    dial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dial.setRange(0, 3, 1.5);
    dial.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);

    addAndMakeVisible(border);
    border.setText("Tipo_forma_d'onda");

    addAndMakeVisible(dial1);
    dial1.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dial1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dial1.setRange(0, 3, 1.5);
    dial1.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);

    addAndMakeVisible(border1);
    border1.setText("Sintesi_sottrattiva");

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
    setSize (800, 450);

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

    // Calcolo delle dimensioni dei componenti
    int keyboardHeight = 130;
    auto leftMargin = getWidth() * 0.05; // Ridotto il margine sinistro
    auto topMargin = keyboardHeight + (getHeight() - keyboardHeight) * 0.05; // Spazio sopra la tastiera MIDI
    auto dialSize = getWidth() * 0.2;

    // Imposto le dimensioni della tastiera MIDI
    keyboardComp.setBounds(0, getHeight() - keyboardHeight, getWidth(), keyboardHeight);

    // Calcolo la larghezza totale dei dial
    auto totalDialWidth = dialSize * 4 + 20 * 3; // Considerando 4 dial e 3 spazi tra di essi

    // Calcolo la posizione x del primo dial per centrare i dial
    auto firstDialX = (getWidth() - totalDialWidth) / 2;

    // Imposto le dimensioni e la posizione dei nuovi slider
    dial.setBounds(firstDialX, topMargin * 0.3, dialSize, dialSize);
    dial1.setBounds(dial.getX() + dial.getWidth() + 20, dial.getY(), dialSize, dialSize);
    dial2.setBounds(dial1.getX() + dial1.getWidth() + 20, dial.getY(), dialSize, dialSize);
    dial3.setBounds(dial2.getX() + dial2.getWidth() + 20, dial.getY(), dialSize, dialSize);

    // Imposto le dimensioni e la posizione del bordo
    border.setBounds(firstDialX, topMargin * 0.1, dialSize, dial.getY() + dial.getHeight());
    border1.setBounds(dial1.getX(), topMargin * 0.1, dialSize, dial1.getY() + dial1.getHeight());
    border2.setBounds(dial2.getX(), topMargin * 0.1, dial2.getWidth() + dial3.getWidth() + 20, dial2.getHeight() + 43);
}

