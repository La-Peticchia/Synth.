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
    

    //slider per l'oscillator
    addAndMakeVisible(dialOscillator);
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
                audioProcessor.audioSynth.SetOscillatorWave(WaveType::squareWave);
            }

        };

    addAndMakeVisible(releseTimeSlider);
    releseTimeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    releseTimeSlider.setRange(0, 3, 1);
    releseTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    releseTimeSlider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::white);
    releseTimeSlider.onValueChange = [this]()
        {
            float durationValue = releseTimeSlider.getValue();
            audioProcessor.audioSynth.SetEnvelopeDuration(EnvType::gainEnv, durationValue);
        };


    addAndMakeVisible(label4);
    label4.setText("ReleseTimeEnvelope", juce::dontSendNotification);
    label4.attachToComponent(&releseTimeSlider, false);
    label4.setFont(12);

    addAndMakeVisible(border);
    border.setText("Oscillator");

    for (int i = 0; i < maxVerticalSliders * 2; ++i) {
        auto slider = std::make_unique<juce::Slider>();// Crea uno slider verticale e lo assegna a std::unique_ptr
        slider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
        slider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
        slider->setRange(0, 1, 0.1);
        addAndMakeVisible(*slider);

        if (i < maxVerticalSliders) {
            oscillatorSliders.push_back(std::move(slider)); // Aggiunge lo slider verticale al vettore per l'inviluppo dell'oscillatore
            auto* currentSlider = oscillatorSliders.back().get(); // Ottengo un puntatore grezzo allo slider corrente
            currentSlider->onValueChange = [currentSlider, this,i]() {
                float sliderValue = currentSlider->getValue();
                audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::attack, i, sliderValue);
                };
            audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::attack, currentSlider->getValue());
        }
        else {
            releseOscillatorSliders.push_back(std::move(slider)); // Aggiunge lo slider verticale al vettore per l'inviluppo del filtro
            auto* currentSlider = releseOscillatorSliders.back().get();
            currentSlider->onValueChange = [currentSlider, this,i]() {
                float sliderValue = currentSlider->getValue();
                int releaseIndex = i - maxVerticalSliders;
                audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::release, releaseIndex, sliderValue);
                };
            audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::release, currentSlider->getValue());
        }
    }

    addAndMakeVisible(oscillatorAdd);
    oscillatorAdd.setText("AttackEnvelope");
    oscillatorAdd.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::indianred);
    oscillatorAdd.setTextLabelPosition(juce::Justification::centred);

    addAndMakeVisible(oscillatorRelese);
    oscillatorRelese.setText("ReleseEnvelope");
    oscillatorRelese.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::cyan);
    oscillatorRelese.setTextLabelPosition(juce::Justification::centred);
    
    //slider per la distorzione
    addAndMakeVisible(dialDistortion);
    dialDistortion.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dialDistortion.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dialDistortion.setRange(0, 4, 1);
    dialDistortion.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dialDistortion.onValueChange = [this]()
        {
            float dialValue = dialDistortion.getValue();

            if (dialValue == 0)
            {
                audioProcessor.audioSynth.SetDistortionFunction(FunctionType::none);
            }
            else if (dialValue == 1.0 )
            {
                audioProcessor.audioSynth.SetDistortionFunction(FunctionType::softClip);
            }
            else if (dialValue == 2.0 )
            {
                audioProcessor.audioSynth.SetDistortionFunction(FunctionType::hardClip);
            }
            else if (dialValue == 3)
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
    
    //slider per il filter
    addAndMakeVisible(dialFilter);
    dialFilter.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dialFilter.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 16);
    dialFilter.setRange(0.3, 1.3, 0.1);
    dialFilter.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    dialFilter.onValueChange = [this]() {
        double filterValue = dialFilter.getValue();
        audioProcessor.audioSynth.SetLPFilterResonance(filterValue);
        };

    addAndMakeVisible(border2);
    border2.setText("Filter");

    //slider per l'inviluppo del filter
    for (int i = 0; i < maxVerticalSliders * 2; ++i) {
        auto slider = std::make_unique<juce::Slider>(); // Crea uno slider verticale e lo assegna a std::unique_ptr
        slider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
        slider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
        slider->setRange(0.5, 5, 0.1);
        addAndMakeVisible(*slider);

        if (i < maxVerticalSliders) {
            filterSliders.push_back(std::move(slider)); // Aggiunge lo slider verticale al vettore
            auto* currentSlider = filterSliders.back().get();
            currentSlider->onValueChange = [currentSlider, this,i]() {
                float sliderValue = currentSlider->getValue();
                DBG("Filter slider value changed: " << sliderValue);
                audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::attack, i, sliderValue);
                };
            audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::attack, currentSlider->getValue());
        }
        else {
            releseFilterSliders.push_back(std::move(slider)); // Aggiunge lo slider verticale al vettore
            auto* currentSlider = releseFilterSliders.back().get();
            currentSlider->onValueChange = [currentSlider, this,i]() {
                float sliderValue = currentSlider->getValue();
                int releaseIndex = i - maxVerticalSliders;
                DBG("Release slider value change: " << sliderValue);
                audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::release, releaseIndex, sliderValue);
                };
            audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::release, currentSlider->getValue());
        }
    }

    addAndMakeVisible(filterAdd);
    filterAdd.setText("AttackEnvelope");
    filterAdd.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::indianred);
    filterAdd.setTextLabelPosition(juce::Justification::centred);

    addAndMakeVisible(filterRelese);
    filterRelese.setText("ReleseEnvelope");
    filterRelese.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::cyan);
    filterRelese.setTextLabelPosition(juce::Justification::centred);

    addAndMakeVisible(distortionToggle);
    distortionToggle.setButtonText("Enable WaveShaper");
    distortionToggle.onClick = [this]() {
        bool distortionActive = distortionToggle.getToggleState();
        audioProcessor.audioSynth.SetDistortionActive(distortionActive);
        };

    addAndMakeVisible(filterToggle);
    filterToggle.setButtonText("Enable Filter");
    filterToggle.onClick = [this]() {
        bool filterActive = filterToggle.getToggleState();
        audioProcessor.audioSynth.SetLPFilterActive(filterActive);
        };

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

    auto borderWidth = getWidth() * 0.32;
    auto borderHeight = getHeight() - topMargin * 0.1 - dialSpacing - keyboardHeight;

    // Imposta le dimensioni e la posizione dei bordi
    border.setBounds((getWidth() - borderWidth * 3 - dialSpacing * 2) / 2, topMargin * 0.1, borderWidth, borderHeight);
    border1.setBounds(border.getRight() + dialSpacing, topMargin * 0.1, borderWidth, borderHeight);
    border2.setBounds(border1.getRight() + dialSpacing, topMargin * 0.1, borderWidth, borderHeight);


    // Imposta le dimensioni e la posizione dei dial all'interno dei bordi
    dialOscillator.setBounds(border.getX() + (border.getWidth() - dialSize * 2 - dialSpacing) / 2, topMargin * 0.3, dialSize, dialSize);
    releseTimeSlider.setBounds(dialOscillator.getRight() + 20, topMargin * 0.3, 130, dialSize);
    dialDistortion.setBounds(border1.getX() + dialSpacing, topMargin * 0.6, dialSize, dialSize);
    distortionToggle.setBounds(border1.getX() + dialSpacing, topMargin * 0.01, 150, dialSize);
    dialBias.setBounds(border1.getX() + (border1.getWidth() - dialSize) * 0.9, topMargin * 0.6, dialSize, dialSize);
    dialGain.setBounds(border1.getX() + (border1.getWidth() - dialSize) / 2, topMargin * 1.5, dialSize, dialSize);
    dialFilter.setBounds(border2.getX() + (border2.getWidth() - dialSize) / 2, topMargin * 0.3, dialSize, dialSize);
    filterToggle.setBounds(border2.getX() + dialSpacing, topMargin * 0.01, 150, dialSize);

    // Imposta la posizione delle etichette
    label1.setBounds(dialDistortion.getX() + (dialSize - label1.getWidth()) + 18, dialDistortion.getY() - 18, label1.getWidth(), 25);
    label2.setBounds(dialBias.getX() + (dialSize - label2.getWidth()) + 33, dialBias.getY() - 18, label2.getWidth(), 25);
    label3.setBounds(dialGain.getX() + (dialSize - label3.getWidth()) + 33, dialGain.getY() - 18, label3.getWidth(), 25);
    label4.setBounds(releseTimeSlider.getX() + (releseTimeSlider.getWidth() - label4.getWidth()) + 8, releseTimeSlider.getY() - 5, label4.getWidth(),25);

    auto sliderWidth = border.getWidth() / maxVerticalSliders;
    auto sliderHeight = border.getHeight() * 0.5;

    for (int i = 0; i < maxVerticalSliders; ++i) {

        auto sliderOsc = oscillatorSliders[i].get();
        auto sliderReleaseOsc = releseOscillatorSliders[i].get();
        auto sliderFilter = filterSliders[i].get();
        auto sliderReleaseFilter = releseFilterSliders[i].get();

        // Imposta la posizione dei slider per l'inviluppo dell'oscillatore
        oscillatorAdd.setBounds(border.getX() + 20, border.getY() + sliderHeight - 55, sliderWidth + 175, sliderHeight - 75);
        sliderOsc->setBounds(border.getX() + i * sliderWidth, border.getY() + sliderHeight - 40, sliderWidth, sliderHeight - 100);

        oscillatorRelese.setBounds(border.getX() + 20, border.getY() + sliderHeight + 70, sliderWidth + 175, sliderHeight - 80);
        sliderReleaseOsc->setBounds(border.getX() + i * sliderWidth, border.getY() + sliderHeight + 80, sliderWidth, sliderHeight - 100);

        // Imposta la posizione dei slider per l'inviluppo del filtro
        filterAdd.setBounds(border2.getX() + 20, border2.getY() + sliderHeight - 55, sliderWidth + 175, sliderHeight - 75);
        sliderFilter->setBounds(border2.getX() + i * sliderWidth, border2.getY() + sliderHeight - 40, sliderWidth, sliderHeight - 100);

        filterRelese.setBounds(border2.getX() + 20, border2.getY() + sliderHeight + 70, sliderWidth + 175, sliderHeight - 80);
        sliderReleaseFilter->setBounds(border2.getX() + i * sliderWidth, border2.getY() + sliderHeight + 80, sliderWidth, sliderHeight - 100);
    }

}


