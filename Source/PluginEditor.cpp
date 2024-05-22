
/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Enums.h"

//==============================================================================
GCB_SynthAudioProcessorEditor::GCB_SynthAudioProcessorEditor(GCB_SynthAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), keyboardComp(p.keyboardState, juce::KeyboardComponentBase::horizontalKeyboard)
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
	dialOscillator.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
	dialOscillator.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colours::transparentWhite);

	addAndMakeVisible(waveTypeLabel);
	waveTypeLabel.setJustificationType(juce::Justification::centred);
	waveTypeLabel.setFont(juce::Font(10.0f, juce::Font::italic));
	waveTypeLabel.setText("Sine Wave", juce::dontSendNotification);

	dialOscillator.onValueChange = [this]()
		{
			float dialValue = dialOscillator.getValue();

			if (dialValue < 1.0)
			{
				audioProcessor.audioSynth.SetOscillatorWave(WaveType::sine);
				waveTypeLabel.setText("Sine Wave", juce::dontSendNotification);
			}
			else if (dialValue >= 1.0 && dialValue < 2.0)
			{
				audioProcessor.audioSynth.SetOscillatorWave(WaveType::sawThooth);
				waveTypeLabel.setText("Saw Thooth", juce::dontSendNotification);
			}
			else if (dialValue >= 2.0 && dialValue <= 3.0)
			{
				audioProcessor.audioSynth.SetOscillatorWave(WaveType::squareWave);
				waveTypeLabel.setText("Square Wave", juce::dontSendNotification);
			}
		};

	addAndMakeVisible(border);
	border.setText("Oscillator");

	// Crea attack sliders
	for (int i = 0; i < numAttackSliders; ++i) {
		
		auto oscillatorSlider = std::make_unique<juce::Slider>();
		oscillatorSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		oscillatorSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		oscillatorSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		oscillatorSlider->setRange(0, 1000, 1);
		oscillatorSlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*oscillatorSlider);
		oscillatorSlider->setLookAndFeel(&otherLookAndFeel);

		attackOscillatorSliders.push_back(std::move(oscillatorSlider));

		auto* currentOscillatorSlider = attackOscillatorSliders.back().get();

		currentOscillatorSlider->onValueChange = [currentOscillatorSlider, this, i]() {
			float sliderValue = currentOscillatorSlider->getValue();
			if (i == 0 || i == 2) {
				audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::attack, i, sliderValue);
			}
			else if (i == 1 || i == 3) {
				audioProcessor.audioSynth.SetEnvelopeDuration(EnvType::gainEnv, sliderValue);
			}
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::attack, currentOscillatorSlider->getValue());
			};

		auto filterSlider = std::make_unique<juce::Slider>();
		filterSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		filterSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		filterSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		filterSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		filterSlider->setRange(0, 1000, 1);
		filterSlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*filterSlider);
		filterSlider->setLookAndFeel(&otherLookAndFeel);

		attackFilterSliders.push_back(std::move(filterSlider));

		auto* currentFilterSlider = attackFilterSliders.back().get();

		currentFilterSlider->onValueChange = [currentFilterSlider, this, i]() {
			float sliderValue = currentFilterSlider->getValue();
			if (i == 0 || i == 2) {
				audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::attack, i, sliderValue);
			}
			else if (i == 1 || i == 3) {
				audioProcessor.audioSynth.SetEnvelopeDuration(EnvType::lpFilterEnv, sliderValue);
			}
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::attack, currentFilterSlider->getValue());
			};
	}

	// Crea release sliders e flanger sliders
	for (int i = 0; i < numRelFlaSliders; ++i) {

		auto oscillatorSlider = std::make_unique<juce::Slider>();
		oscillatorSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		oscillatorSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		oscillatorSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		oscillatorSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		oscillatorSlider->setRange(0, 1000, 1);
		oscillatorSlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*oscillatorSlider);
		oscillatorSlider->setLookAndFeel(&otherLookAndFeel);


		releaseOscillatorSliders.push_back(std::move(oscillatorSlider));


		auto* currentOscillatorSlider = releaseOscillatorSliders.back().get();

		currentOscillatorSlider->onValueChange = [currentOscillatorSlider, this, i]() {
			float sliderValue = currentOscillatorSlider->getValue();
			if (i == 0)
			{
				audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::release, i, sliderValue);
			}

			else if (i == 1 || i == 2) {
				audioProcessor.audioSynth.SetEnvelopeDuration(EnvType::gainEnv, sliderValue);
			}
			
			};


		audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::release, currentOscillatorSlider->getValue());


		auto filterSlider = std::make_unique<juce::Slider>();
		filterSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		filterSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		filterSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		filterSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		filterSlider->setRange(0, 1000, 1);
		filterSlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*filterSlider);
		filterSlider->setLookAndFeel(&otherLookAndFeel);


		releaseFilterSliders.push_back(std::move(filterSlider));


		auto* currentFilterSlider = releaseFilterSliders.back().get();


		currentFilterSlider->onValueChange = [currentFilterSlider, this, i]() {
			float sliderValue = currentFilterSlider->getValue();
			if (i == 0) {
				audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::release, i, sliderValue);
			}

			else if (i == 1 || i == 2) {
				audioProcessor.audioSynth.SetEnvelopeDuration(EnvType::lpFilterEnv, sliderValue);
			}
			};


		audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::release, currentFilterSlider->getValue());


		auto flangerSlider = std::make_unique<juce::Slider>();
		flangerSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		flangerSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 51, 10);
		flangerSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		flangerSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);

		switch (i) {
		case 0:
			flangerSlider->setRange(0.005, 0.015, 0.001);
			flangerSlider->setTextValueSuffix(" ms");
			break;
		case 1:
			flangerSlider->setRange(0.5, 1.0, 0.1);
			flangerSlider->setTextValueSuffix(" dB");
			break;
		case 2:
			flangerSlider->setRange(1.0, 5.0, 1.0);
			flangerSlider->setTextValueSuffix(" Hz");
			break;
		}
		addAndMakeVisible(*flangerSlider);
		flangerSlider->setLookAndFeel(&otherLookAndFeel);

		flangerSliders.push_back(std::move(flangerSlider));


		auto* currentFlangerSlider = flangerSliders.back().get();

		if (i == 0) {
			// Slider per il tempo di delay
			currentFlangerSlider->onValueChange = [currentFlangerSlider, this]() {
				float sliderValue = currentFlangerSlider->getValue();
				audioProcessor.flanger.SetDelay(sliderValue);
				};
		}
		else if (i == 1) {
			// Slider per il gain
			currentFlangerSlider->onValueChange = [currentFlangerSlider, this]() {
				float sliderValue = currentFlangerSlider->getValue();
				audioProcessor.flanger.SetGain(sliderValue);
				};
		}
		else if (i == 2) {
			// Slider per la frequency
			currentFlangerSlider->onValueChange = [currentFlangerSlider, this]() {
				float sliderValue = currentFlangerSlider->getValue();
				audioProcessor.flanger.SetFrequency(sliderValue);
				};
		}

	}

	//delay sliders
	for (int i = 0; i < numDelaySliders; ++i) {


		auto delaySlider = std::make_unique<juce::Slider>();
		delaySlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		delaySlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		delaySlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		delaySlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		addAndMakeVisible(*delaySlider);
		delaySlider->setLookAndFeel(&otherLookAndFeel);

		switch (i) {
		case 0:
			delaySlider->setRange(0.05, 0.15, 0.01);
			delaySlider->setTextValueSuffix(" ms");
			break;
		case 1:
			delaySlider->setRange(0.5, 0.9, 0.1);
			delaySlider->setTextValueSuffix(" dB");
			break;
		}

		delaySliders.push_back(std::move(delaySlider));

		auto* currentdelaySlider = delaySliders.back().get();

		if (i == 0) {
			// Slider per il tempo di delay
			currentdelaySlider->onValueChange = [currentdelaySlider, this]() {
				float sliderValue = currentdelaySlider->getValue();
				audioProcessor.delay.SetDelay(sliderValue);
				};
		}
		else if (i == 1) {
			// Slider per il gain
			currentdelaySlider->onValueChange = [currentdelaySlider, this]() {
				float sliderValue = currentdelaySlider->getValue();
				audioProcessor.delay.SetGain(sliderValue);
				};
		}
	}


	addAndMakeVisible(oscillatorAttackBorder);
	oscillatorAttackBorder.setText("AttackEnvelope");
	oscillatorAttackBorder.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::indianred);
	oscillatorAttackBorder.setTextLabelPosition(juce::Justification::centred);

	addAndMakeVisible(oscillatorReleaseBorder);
	oscillatorReleaseBorder.setText("ReleseEnvelope");
	oscillatorReleaseBorder.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::cyan);
	oscillatorReleaseBorder.setTextLabelPosition(juce::Justification::centred);

	//slider per la distorzione
	addAndMakeVisible(dialDistortion);
	dialDistortion.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	dialDistortion.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
	dialDistortion.setRange(0, 3, 1);
	dialDistortion.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
	dialDistortion.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
	dialDistortion.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colours::transparentWhite);

	addAndMakeVisible(functionTypeLabel);
	functionTypeLabel.setJustificationType(juce::Justification::centred);
	functionTypeLabel.setFont(juce::Font(10.0f, juce::Font::italic));
	functionTypeLabel.setText("None", juce::dontSendNotification);

	dialDistortion.onValueChange = [this]()
		{
			float dialValue = dialDistortion.getValue();

			if (dialValue == 0)
			{
				audioProcessor.audioSynth.SetDistortionFunction(FunctionType::none);
				functionTypeLabel.setText("None", juce::dontSendNotification);
			}
			else if (dialValue == 1.0)
			{
				audioProcessor.audioSynth.SetDistortionFunction(FunctionType::softClip);
				functionTypeLabel.setText("Soft Clip", juce::dontSendNotification);
			}
			else if (dialValue == 2.0)
			{
				audioProcessor.audioSynth.SetDistortionFunction(FunctionType::hardClip);
				functionTypeLabel.setText("Hard Clip", juce::dontSendNotification);
			}
			else if (dialValue == 3)
			{
				audioProcessor.audioSynth.SetDistortionFunction(FunctionType::waveFold);
				functionTypeLabel.setText("Wave Fold", juce::dontSendNotification);
			}
		};
	addAndMakeVisible(label1);
	label1.setText("Distortion", juce::dontSendNotification);
	label1.attachToComponent(&dialDistortion, false);
	label1.setFont(juce::Font(12.0f, juce::Font::bold));


	//slider per il bias
	addAndMakeVisible(dialBias);
	dialBias.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	dialBias.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
	dialBias.setRange(-1.0, 1.0, 0.1);
	dialBias.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
	dialBias.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
	dialBias.onValueChange = [this]() {
		float biasValue = dialBias.getValue();
		audioProcessor.audioSynth.SetDistortionBias(biasValue);
		};

	addAndMakeVisible(label2);
	label2.setText("Bias", juce::dontSendNotification);
	label2.attachToComponent(&dialBias, false);
	label2.setFont(juce::Font(12.0f, juce::Font::bold));


	//slider per il gain
	addAndMakeVisible(dialGain);
	dialGain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	dialGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
	dialGain.setRange(2.0, 5.0, 1.0);
	dialGain.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
	dialGain.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
	dialGain.setColour(juce::Slider::ColourIds::textBoxTextColourId , juce::Colours::transparentWhite);
	dialGain.onValueChange = [this]() {
		float gainValue = dialGain.getValue();
		audioProcessor.audioSynth.SetDistortionGain(gainValue);
		};

	addAndMakeVisible(label3);
	label3.setText("Gain", juce::dontSendNotification);
	label3.attachToComponent(&dialGain, false);
	label3.setFont(juce::Font(12.0f, juce::Font::bold));

	addAndMakeVisible(border1);
	border1.setText("Waveshaper");

	//slider per il filter
	addAndMakeVisible(dialFilter);
	dialFilter.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	dialFilter.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
	dialFilter.setRange(0.3, 1.3, 0.1);
	dialFilter.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
	dialFilter.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
	dialFilter.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colours::transparentWhite);
	dialFilter.onValueChange = [this]() {
		double filterValue = dialFilter.getValue();
		audioProcessor.audioSynth.SetLPFilterResonance(filterValue);
		};

	addAndMakeVisible(border2);
	border2.setText("Filter");

	addAndMakeVisible(filterAttackBorder);
	filterAttackBorder.setText("AttackEnvelope");
	filterAttackBorder.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::indianred);
	filterAttackBorder.setTextLabelPosition(juce::Justification::centred);

	addAndMakeVisible(filterReleaseBorder);
	filterReleaseBorder.setText("ReleseEnvelope");
	filterReleaseBorder.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::cyan);
	filterReleaseBorder.setTextLabelPosition(juce::Justification::centred);

	addAndMakeVisible(distortionToggle);
	distortionToggle.setButtonText("Enable");
	distortionToggle.onClick = [this]() {
		bool distortionActive = distortionToggle.getToggleState();
		audioProcessor.audioSynth.SetDistortionActive(distortionActive);
		};

	addAndMakeVisible(filterToggle);
	filterToggle.setButtonText("Enable");
	filterToggle.onClick = [this]() {
		bool filterActive = filterToggle.getToggleState();
		audioProcessor.audioSynth.SetLPFilterActive(filterActive);
		};

	addAndMakeVisible(border3);
	border3.setText("Delay");

	addAndMakeVisible(border4);
	border4.setText("Flanger");

	addAndMakeVisible(delayToggle);
	delayToggle.setButtonText("Enable");
	delayToggle.onClick = [this]() {
		bool delayActive = delayToggle.getToggleState();
		audioProcessor.delay.SetActive(delayActive);
		};

	addAndMakeVisible(flangerToggle);
	flangerToggle.setButtonText("Enable");
	flangerToggle.onClick = [this]() {
		bool flangerActive = flangerToggle.getToggleState();
		audioProcessor.flanger.SetActive(flangerActive);
		};

	dialOscillator.setLookAndFeel(&otherLookAndFeel);
	dialDistortion.setLookAndFeel(&otherLookAndFeel);
	dialFilter.setLookAndFeel(&otherLookAndFeel);
	dialBias.setLookAndFeel(&otherLookAndFeel);
	dialGain.setLookAndFeel(&otherLookAndFeel);

	setSize(1000, 550);

}

GCB_SynthAudioProcessorEditor::~GCB_SynthAudioProcessorEditor()
{
}

//==============================================================================
void GCB_SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)

	g.fillAll(juce::Colours::transparentBlack);

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
	auto halfBorderHeight = (borderHeight - dialSpacing) / 2;

	// Imposta le dimensioni e la posizione dei bordi
	border.setBounds((getWidth() - borderWidth * 3 - dialSpacing * 2) / 2, topMargin * 0.1, borderWidth, borderHeight);
	border1.setBounds(border.getRight() + dialSpacing, topMargin * 0.1, getWidth() * 0.13, borderHeight);
	border2.setBounds(border1.getRight() + dialSpacing, topMargin * 0.1, borderWidth, borderHeight);
	border3.setBounds(border2.getRight() + dialSpacing, topMargin * 0.1, getWidth() * 0.175, halfBorderHeight);
	border4.setBounds(border2.getRight() + dialSpacing, border3.getBottom() + dialSpacing, getWidth() * 0.175, halfBorderHeight);

	// Imposta le dimensioni e la posizione dei dial all'interno dei bordi
	dialOscillator.setBounds(border.getX() + (border.getWidth() - dialSize * 2 - dialSpacing) + 5, topMargin * 0.25, dialSize, dialSize);
	dialDistortion.setBounds(border1.getX() + dialSpacing, topMargin * 0.5, dialSize, dialSize);
	dialBias.setBounds(border1.getX() + dialSpacing, topMargin * 1.18, dialSize, dialSize);
	dialGain.setBounds(border1.getX() + dialSpacing, topMargin * 1.95, dialSize, dialSize);
	dialFilter.setBounds(border2.getX() + (border2.getWidth() - dialSize) / 2, topMargin * 0.25, dialSize, dialSize);

	// Imposta la posizione delle etichette
	label1.setBounds(dialDistortion.getX(), dialDistortion.getY() - 15, label1.getWidth(), 25);
	label2.setBounds(dialBias.getX(), dialBias.getY() - 15, label2.getWidth(), 25);
	label3.setBounds(dialGain.getX(), dialGain.getY() - 15, label3.getWidth(), 25);
	waveTypeLabel.setBounds(dialOscillator.getX(), dialOscillator.getY() + dialOscillator.getHeight() - 30, dialOscillator.getWidth(), 25);
	functionTypeLabel.setBounds(dialDistortion.getX(), dialDistortion.getY() + dialDistortion.getHeight() - 30, dialDistortion.getWidth(), 25);

	auto attackSliderWidth = border.getWidth() / numAttackSliders;
	auto releaseSliderWidth = border.getWidth() / numRelFlaSliders;
	auto flangerSliderWidth = border4.getWidth() / numRelFlaSliders;
	auto delaySliderWidth = border3.getWidth() / numDelaySliders;
	auto sliderHeight = border.getHeight() * 0.5;

	// Posizionamento degli slider di attacco
	for (int i = 0; i < numAttackSliders; ++i) {
		auto sliderattackOsc = attackOscillatorSliders[i].get();
		auto sliderattackFil = attackFilterSliders[i].get();

		auto* oscillatorLabel = new juce::Label();
		oscillatorLabel->setText((i == 0 || i == 2) ? "Value" : "Duration", juce::dontSendNotification);
		oscillatorLabel->setJustificationType(juce::Justification::centred);
		oscillatorLabel->setFont(10);
		addAndMakeVisible(oscillatorLabel);

		auto* filterLabel = new juce::Label();
		filterLabel->setText((i == 0 || i == 2) ? "Value" : "Duration", juce::dontSendNotification);
		filterLabel->setJustificationType(juce::Justification::centred);
		filterLabel->setFont(10);
		addAndMakeVisible(filterLabel);

		sliderattackOsc->setBounds(border.getX() + i * attackSliderWidth, border.getY() + sliderHeight - 50, attackSliderWidth, sliderHeight - 110);
		oscillatorAttackBorder.setBounds(border.getX() + 12, sliderattackOsc->getY() - 30, attackSliderWidth * 4 - 24, sliderHeight - 70);
		oscillatorLabel->setBounds(sliderattackOsc->getX(), sliderattackOsc->getY() - 15, sliderattackOsc->getWidth(), 20);

		sliderattackFil->setBounds(border2.getX() + i * attackSliderWidth, border2.getY() + sliderHeight - 50, attackSliderWidth, sliderHeight - 110);
		filterAttackBorder.setBounds(border2.getX() + 12, sliderattackFil->getY() - 30, attackSliderWidth * 4 - 24, sliderHeight - 70);
		filterLabel->setBounds(sliderattackFil->getX(), sliderattackFil->getY() - 15, sliderattackFil->getWidth(), 20);
	}

	// Posizionamento degli slider di rilascio
	for (int i = 0; i < numRelFlaSliders; ++i) {
		auto sliderReleaseOsc = releaseOscillatorSliders[i].get();
		auto sliderReleaseFil = releaseFilterSliders[i].get();
		auto sliderFlanger = flangerSliders[i].get();

		auto* oscillatorLabel = new juce::Label();
		oscillatorLabel->setText((i == 0) ? "Value" : "Duration", juce::dontSendNotification);
		oscillatorLabel->setJustificationType(juce::Justification::centred);
		addAndMakeVisible(oscillatorLabel);
		oscillatorLabel->setFont(10);

		auto* filterLabel = new juce::Label();
		filterLabel->setText((i == 0) ? "Value" : "Duration", juce::dontSendNotification);
		auto* label = new juce::Label();
		filterLabel->setJustificationType(juce::Justification::centred);
		addAndMakeVisible(filterLabel);
		filterLabel->setFont(10);

		sliderReleaseOsc->setBounds(border.getX() + i * releaseSliderWidth, border.getY() + sliderHeight + 80, releaseSliderWidth, sliderHeight - 110);
		oscillatorReleaseBorder.setBounds(border.getX() + 15, sliderReleaseOsc->getY() - 30, releaseSliderWidth * 3 - 30, sliderHeight - 65);
		oscillatorLabel->setBounds(sliderReleaseOsc->getX(), sliderReleaseOsc->getY() - 15, sliderReleaseOsc->getWidth(), 20);
		
		sliderReleaseFil->setBounds(border2.getX() + i * releaseSliderWidth, border2.getY() + sliderHeight + 80, releaseSliderWidth, sliderHeight - 110);
		filterReleaseBorder.setBounds(border2.getX() + 15, sliderReleaseFil->getY() - 30, releaseSliderWidth * 3 - 30, sliderHeight - 65);
		filterLabel->setBounds(sliderReleaseFil->getX(), sliderReleaseFil->getY() - 15, sliderReleaseFil->getWidth(), 20);

		sliderFlanger->setBounds(border4.getX() + i * flangerSliderWidth, border4.getY() + border4.getHeight() / 3, flangerSliderWidth, sliderHeight - 110);
	}

	// Posizionamento degli slider di delay
	for (int i = 0; i < numDelaySliders; ++i) {
		auto sliderDelay = delaySliders[i].get();
		sliderDelay->setBounds(border3.getX() + i * delaySliderWidth, border3.getY() + border3.getHeight() / 3, delaySliderWidth, sliderHeight - 110);
	}

	// Imposta dimensioni e posizione dei togglebutton 
	distortionToggle.setBounds(border1.getX() + dialSpacing, topMargin - 155, 70, dialSize);
	filterToggle.setBounds(border2.getX() + dialSpacing, topMargin - 155, 70, dialSize);
	delayToggle.setBounds(border3.getX() + dialSpacing, topMargin - 155, 70, dialSize);
	flangerToggle.setBounds(border4.getX() + dialSpacing, border4.getY() - flangerSliderWidth / 3, 70, dialSize);
}


