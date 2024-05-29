
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
		switch (i) {
		case 0:
			oscillatorSlider->setRange(0.1, 1, 0.1);
			oscillatorSlider->setValue(0.1);
			//oscillatorSlider->onValueChange();
			break;
		case 1:
			oscillatorSlider->setRange(DEFAULT_RAMP_DURATION * 1000, 1000, 1);
			oscillatorSlider->setTextValueSuffix(" ms");
			break;
		case 2:
			oscillatorSlider->setRange(0.1, 1, 0.1);
			oscillatorSlider->setValue(1);
			//oscillatorSlider->onValueChange();;
			break;
		case 3:
			oscillatorSlider->setRange(DEFAULT_RAMP_DURATION * 1000, 1000, 1);
			oscillatorSlider->setTextValueSuffix(" ms");
			break;
		}
		addAndMakeVisible(*oscillatorSlider);
		oscillatorSlider->setLookAndFeel(&otherLookAndFeel);

		attackOscillatorSliders.push_back(std::move(oscillatorSlider));

		auto* currentOscillatorSlider = attackOscillatorSliders.back().get();

		currentOscillatorSlider->onValueChange = [currentOscillatorSlider, this, i]() {
			float sliderValue = currentOscillatorSlider->getValue();
			if (i == 0 || i == 2) {
				audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::attack, (int)i / 2, sliderValue);
			}
			else if (i == 1 || i == 3) {
				audioProcessor.audioSynth.SetRampDuration(EnvType::gainEnv, EnvState::attack, (int)(i - 1) / 2, sliderValue / 1000);
			}
			};
		

		auto filterSlider = std::make_unique<juce::Slider>();
		filterSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		filterSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		filterSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		filterSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		switch (i) {
		case 0:
			filterSlider->setRange(0.1, 1, 0.1);
			filterSlider->setValue(0.1);
			//filterSlider->onValueChange();
			break;
		case 1:
			filterSlider->setRange(DEFAULT_RAMP_DURATION * 1000, 1000, 1);
			filterSlider->setTextValueSuffix(" ms");
			break;
		case 2:
			filterSlider->setRange(0.1, 1, 0.1);
			filterSlider->setValue(1);
			//filterSlider->onValueChange();
			break;
		case 3:
			filterSlider->setRange(DEFAULT_RAMP_DURATION * 1000, 1000, 1);
			filterSlider->setTextValueSuffix(" ms");
			break;
		}
		addAndMakeVisible(*filterSlider);
		filterSlider->setLookAndFeel(&otherLookAndFeel);

		attackFilterSliders.push_back(std::move(filterSlider));

		auto* currentFilterSlider = attackFilterSliders.back().get();

		currentFilterSlider->onValueChange = [currentFilterSlider, this, i]() {
			float sliderValue = currentFilterSlider->getValue();
			if (i == 0 || i == 2) {
				audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::attack, (int)i/2, sliderValue);
			}
			else if (i == 1 || i == 3) {
				audioProcessor.audioSynth.SetRampDuration(EnvType::lpFilterEnv, EnvState::attack, (int)(i - 1) / 2, sliderValue / 1000);
			}
			};

		if (!(i % 2)) {
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::attack, currentOscillatorSlider->getValue());
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::attack, currentFilterSlider->getValue());
		}
	
	}

	// Crea release sliders
	for (int i = 0; i < numReleaseSliders; ++i) {

		auto oscillatorSlider = std::make_unique<juce::Slider>();
		oscillatorSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		oscillatorSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		oscillatorSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		oscillatorSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		switch (i) {
		case 0:
			oscillatorSlider->setRange(0.1, 1, 0.1);
			oscillatorSlider->setValue(0.1);

			break;
		case 1:
			oscillatorSlider->setRange(DEFAULT_RAMP_DURATION * 1000, 1000, 1);
			oscillatorSlider->setTextValueSuffix(" ms");
			break;
		case 2:
			oscillatorSlider->setRange(DEFAULT_RAMP_DURATION * 1000, 1000, 1);
			oscillatorSlider->setTextValueSuffix(" ms");
			break;
		}
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
				audioProcessor.audioSynth.SetRampDuration(EnvType::gainEnv, EnvState::release, i-1, sliderValue / 1000);
			}
			
			};


		auto filterSlider = std::make_unique<juce::Slider>();
		filterSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		filterSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		filterSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		filterSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		switch (i) {
		case 0:
			filterSlider->setRange(0.1, 1, 0.1);
			filterSlider->setValue(0.1);

			break;
		case 1:
			filterSlider->setRange(DEFAULT_RAMP_DURATION * 1000, 1000, 1);
			DBG(filterSlider->getValue());
			filterSlider->setTextValueSuffix(" ms");
			break;
		case 2:
			filterSlider->setRange(DEFAULT_RAMP_DURATION * 1000, 1000, 1);
			filterSlider->setTextValueSuffix(" ms");
			break;
		}
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
				audioProcessor.audioSynth.SetRampDuration(EnvType::lpFilterEnv, EnvState::release, i - 1, sliderValue / 1000);
			}
			};
		
		if (!i) {
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::release, currentOscillatorSlider->getValue());
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::release, currentFilterSlider->getValue());
		}

	}

	audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::release, 0);
	audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::release, 0);

	audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::release, 1, 0);
	audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::release, 1, 0);


	// Crea flanger sliders
	for (int i = 0; i < numFlangerSliders; ++i) {

		auto flangerSlider = std::make_unique<juce::Slider>();
		if (i == 0)
		{
			flangerSlider->setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
			flangerSlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 51, 10);
		}
		else
		{
			flangerSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
			flangerSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 51, 10);
		}
		
		flangerSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		flangerSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);

		switch (i) {
		case 0:
			flangerSlider->setRange(0, 1, 0.001);
			flangerSlider->setTextValueSuffix(" %");
			break;
		case 1:
			flangerSlider->setRange(0, 1.0, 0.1);
			flangerSlider->setTextValueSuffix(" %");
			break;
		case 2:
			flangerSlider->setRange(0.01, 1, 0.01);
			flangerSlider->setTextValueSuffix(" Hz");
			break;
		case 3:
			flangerSlider->setRange(0, 1.0, 0.1);
			flangerSlider->setTextValueSuffix(" %");
			break;
		}

		addAndMakeVisible(*flangerSlider);
		flangerSlider->setLookAndFeel(&otherLookAndFeel);

		flangerSliders.push_back(std::move(flangerSlider));


		auto* currentFlangerSlider = flangerSliders.back().get();

		if (i == 0) {
			
			currentFlangerSlider->onValueChange = [currentFlangerSlider, this]() {
				float sliderValue = currentFlangerSlider->getValue();
				audioProcessor.flanger.SetDepth(sliderValue);
				};
		}
		else if (i == 1) {
			
			currentFlangerSlider->onValueChange = [currentFlangerSlider, this]() {
				float sliderValue = currentFlangerSlider->getValue();
				audioProcessor.flanger.SetWet(sliderValue);
				};
		}
		else if (i == 2) {
			
			currentFlangerSlider->onValueChange = [currentFlangerSlider, this]() {
				float sliderValue = currentFlangerSlider->getValue();
				audioProcessor.flanger.SetFrequency(sliderValue);
				};
		}
		else if (i == 3) {
			
			currentFlangerSlider->onValueChange = [currentFlangerSlider, this]() {
				float sliderValue = currentFlangerSlider->getValue();
				audioProcessor.flanger.SetDry(sliderValue);
				};
		}
	}

	//delay sliders
	for (int i = 0; i < numDelaySliders; ++i) {

		auto delaySlider = std::make_unique<juce::Slider>();
		delaySlider->setSliderStyle(i == 0 ? juce::Slider::SliderStyle::Rotary : juce::Slider::SliderStyle::LinearVertical);
		delaySlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		delaySlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		delaySlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		addAndMakeVisible(*delaySlider);
		delaySlider->setLookAndFeel(&otherLookAndFeel);

		switch (i) {
		case 0:
			delaySlider->setRange(0.1, 0.9, 0.01);
			delaySlider->setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colours::transparentWhite);
			break;
		case 1:
			delaySlider->setRange(0.1, 1, 0.01);
			delaySlider->setTextValueSuffix(" s");
			break;
		case 2:
			delaySlider->setRange(0.1, 1, 0.01);
			delaySlider->setTextValueSuffix(" %");
			break;
		case 3:
			delaySlider->setRange(0.1, 1, 0.01);
			delaySlider->setTextValueSuffix(" %");
			break;
		}

		delaySliders.push_back(std::move(delaySlider));

		auto* currentdelaySlider = delaySliders.back().get();

		if (i == 0) {
			// Slider per il gain
			currentdelaySlider->onValueChange = [currentdelaySlider, this]() {
				float sliderValue = currentdelaySlider->getValue();
				audioProcessor.delay.SetFeedback(sliderValue);
				};
		}
		else if (i == 1) {
			// slider per il delay
			currentdelaySlider->onValueChange = [currentdelaySlider, this]() {
				float sliderValue = currentdelaySlider->getValue();
				audioProcessor.delay.SetDelay(sliderValue);
				};
		}
		else if (i == 2)
		{
			//slider dry
			currentdelaySlider->onValueChange = [currentdelaySlider, this]() {
				float sliderValue = currentdelaySlider->getValue();
				audioProcessor.delay.SetDry(sliderValue);
				};
		}
		else if (i == 3)
		{
			//slider wet
			currentdelaySlider->onValueChange = [currentdelaySlider, this]() {
				float sliderValue = currentdelaySlider->getValue();
				audioProcessor.delay.SetWet(sliderValue);
				};
		}
	}


	addAndMakeVisible(oscillatorAttackBorder);
	oscillatorAttackBorder.setText("Attack");
	oscillatorAttackBorder.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::indianred);
	oscillatorAttackBorder.setTextLabelPosition(juce::Justification::centred);

	addAndMakeVisible(oscillatorReleaseBorder);
	oscillatorReleaseBorder.setText("Release");
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
	functionTypeLabel.setText("Soft Clip", juce::dontSendNotification);

	dialDistortion.onValueChange = [this]()
		{
			int dialValue = (int)dialDistortion.getValue();

			if (dialValue == 0)
			{
				audioProcessor.audioSynth.SetDistortionFunction(FunctionType::softClip);
				functionTypeLabel.setText("Soft Clip", juce::dontSendNotification);
			}
			else if (dialValue == 1)
			{
				audioProcessor.audioSynth.SetDistortionFunction(FunctionType::hardClip);
				functionTypeLabel.setText("Hard Clip", juce::dontSendNotification);
			}
			else if (dialValue == 2)
			{
				audioProcessor.audioSynth.SetDistortionFunction(FunctionType::waveFold);
				functionTypeLabel.setText("Wave Fold", juce::dontSendNotification);
			} else if (dialValue == 3)
			{
				audioProcessor.audioSynth.SetDistortionFunction(FunctionType::absolute);
				functionTypeLabel.setText("Absolute", juce::dontSendNotification);
			}
		};
	addAndMakeVisible(distortionLabel);
	distortionLabel.setText("Distortion", juce::dontSendNotification);
	distortionLabel.attachToComponent(&dialDistortion, false);
	distortionLabel.setFont(juce::Font(12.0f, juce::Font::bold));


	//slider per il bias
	addAndMakeVisible(dialBias);
	dialBias.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	dialBias.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
	dialBias.setRange(0, 1.0, 0.1);
	dialBias.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
	dialBias.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
	dialBias.onValueChange = [this]() {
		float biasValue = dialBias.getValue();
		audioProcessor.audioSynth.SetDistortionBias(biasValue);
		};
	
	addAndMakeVisible(biasLabel);
	biasLabel.setText("Bias", juce::dontSendNotification);
	biasLabel.attachToComponent(&dialBias, false);
	biasLabel.setFont(juce::Font(12.0f, juce::Font::bold));


	//slider per il gain
	addAndMakeVisible(dialGain);
	dialGain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	dialGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
	dialGain.setRange(1.1, 5.0, 0.1);
	dialGain.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
	dialGain.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
	dialGain.setColour(juce::Slider::ColourIds::textBoxTextColourId , juce::Colours::transparentWhite);
	dialGain.onValueChange = [this]() {
		float gainValue = dialGain.getValue();
		audioProcessor.audioSynth.SetDistortionGain(gainValue);
		};

	addAndMakeVisible(gainLabel);
	gainLabel.setText("Gain", juce::dontSendNotification);
	gainLabel.attachToComponent(&dialGain, false);
	gainLabel.setFont(juce::Font(12.0f, juce::Font::bold));

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
	filterAttackBorder.setText("Attack");
	filterAttackBorder.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::indianred);
	filterAttackBorder.setTextLabelPosition(juce::Justification::centred);

	addAndMakeVisible(filterReleaseBorder);
	filterReleaseBorder.setText("Release");
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
	// Disegna il gradiente di base scuro
	juce::ColourGradient gradient(juce::Colour(20, 20, 20), 0, 0, juce::Colour(40, 40, 40), getWidth(), getHeight(), false);
	g.setGradientFill(gradient);
	g.fillRect(0, 0, getWidth(), getHeight());

	// riflesso metallico
	juce::ColourGradient reflectionGradient(juce::Colours::grey.withAlpha(0.1f), 0, 0, juce::Colours::white.withAlpha(0.0f), getWidth(), getHeight() * 0.3f, false);
	g.setGradientFill(reflectionGradient);
	g.fillRect(0, 60, getWidth(), static_cast<int>(getHeight() * 0.6f));

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
	distortionLabel.setBounds(dialDistortion.getX(), dialDistortion.getY() - 15, distortionLabel.getWidth(), 25);
	biasLabel.setBounds(dialBias.getX(), dialBias.getY() - 15, biasLabel.getWidth(), 25);
	gainLabel.setBounds(dialGain.getX(), dialGain.getY() - 15, gainLabel.getWidth(), 25);
	waveTypeLabel.setBounds(dialOscillator.getX(), dialOscillator.getY() + dialOscillator.getHeight() - 30, dialOscillator.getWidth(), 25);
	functionTypeLabel.setBounds(dialDistortion.getX(), dialDistortion.getY() + dialDistortion.getHeight() - 30, dialDistortion.getWidth(), 25);

	auto attackSliderWidth = border.getWidth() / numAttackSliders;
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

	auto releaseSliderWidth = border.getWidth() / numReleaseSliders;
	
	// Posizionamento degli slider di rilascio
	for (int i = 0; i < numReleaseSliders; ++i) {
		auto sliderReleaseOsc = releaseOscillatorSliders[i].get();
		auto sliderReleaseFil = releaseFilterSliders[i].get();
		
		auto* oscillatorLabel = new juce::Label();
		oscillatorLabel->setText((i == 0) ? "Value" : "Duration", juce::dontSendNotification);
		oscillatorLabel->setJustificationType(juce::Justification::centred);
		addAndMakeVisible(oscillatorLabel);
		oscillatorLabel->setFont(10);

		auto* filterLabel = new juce::Label();
		filterLabel->setText((i == 0) ? "Value" : "Duration", juce::dontSendNotification);
		filterLabel->setJustificationType(juce::Justification::centred);
		addAndMakeVisible(filterLabel);
		filterLabel->setFont(10);

		sliderReleaseOsc->setBounds(border.getX() + i * releaseSliderWidth, border.getY() + sliderHeight + 80, releaseSliderWidth, sliderHeight - 110);
		oscillatorReleaseBorder.setBounds(border.getX() + 15, sliderReleaseOsc->getY() - 30, releaseSliderWidth * 3 - 30, sliderHeight - 65);
		oscillatorLabel->setBounds(sliderReleaseOsc->getX(), sliderReleaseOsc->getY() - 15, sliderReleaseOsc->getWidth(), 20);
		
		sliderReleaseFil->setBounds(border2.getX() + i * releaseSliderWidth, border2.getY() + sliderHeight + 80, releaseSliderWidth, sliderHeight - 110);
		filterReleaseBorder.setBounds(border2.getX() + 15, sliderReleaseFil->getY() - 30, releaseSliderWidth * 3 - 30, sliderHeight - 65);
		filterLabel->setBounds(sliderReleaseFil->getX(), sliderReleaseFil->getY() - 15, sliderReleaseFil->getWidth(), 20);

	}

	auto flangerSliderWidth = border4.getWidth() / numFlangerSliders;

	for (int i = 0; i < numFlangerSliders; ++i) {
		auto sliderFlanger = flangerSliders[i].get();
		auto* flangerLabel = new juce::Label();
		if (i == 0) {
			flangerLabel->setText("Depth", juce::dontSendNotification);
		}
		else if (i == 1) {
			flangerLabel->setText("Wet", juce::dontSendNotification);
		}
		else if (i == 2) {
			flangerLabel->setText("Freq.", juce::dontSendNotification);
		}
		else if (i == 3) {
			flangerLabel->setText("Dry", juce::dontSendNotification);
		}
		flangerLabel->setJustificationType(juce::Justification::centred);
		addAndMakeVisible(flangerLabel);
		flangerLabel->setFont(10);

		if (i == 0) {
			flangerLabel->setBounds(border4.getX(), border4.getY() + border4.getHeight() / 4, border4.getWidth(), 20);
			sliderFlanger->setBounds(border4.getX() + 20, border4.getY() + border4.getHeight() / 4.5, border4.getWidth() - 30, sliderHeight - 120);
		}
		else {
			int verticalPositionY = border4.getY() + sliderHeight - 90;
			int verticalSpacing = (border4.getWidth() - 3 * flangerSliderWidth) / 4;
			flangerLabel->setBounds(border4.getX() + verticalSpacing + (i - 1) * (flangerSliderWidth + verticalSpacing), verticalPositionY - 10, flangerSliderWidth, 20);
			sliderFlanger->setBounds(border4.getX() + verticalSpacing + (i - 1) * (flangerSliderWidth + verticalSpacing), verticalPositionY + 5, flangerSliderWidth, sliderHeight - 130);
		}

	}

	auto delaySliderWidth = border3.getWidth() / numDelaySliders;

	// Posizionamento degli slider di delay
	for (int i = 0; i < numDelaySliders; ++i) {
		auto sliderDelay = delaySliders[i].get();

		auto* delayLabel = new juce::Label();
		if (i == 0) {
			//delayLabel->setText("Gain", juce::dontSendNotification);
		}
		else if (i == 1) {
			delayLabel->setText("Delay", juce::dontSendNotification);
		}
		else if (i == 2){
			delayLabel->setText("Dry", juce::dontSendNotification);
		}
		else if (i == 3) {
			delayLabel->setText("Wet", juce::dontSendNotification);
		}	
		delayLabel->setJustificationType(juce::Justification::centred);
		addAndMakeVisible(delayLabel);
		delayLabel->setFont(10);
		

		if (i == 0) {
			//delayLabel->setBounds(border3.getX(), border3.getY() + border3.getHeight() / 5.5, border3.getWidth(), 20);
			sliderDelay->setBounds(border3.getX(), border3.getY() + border3.getHeight() / 4.5, border3.getWidth(), sliderHeight - 120);
		}
		else {
			int verticalPositionY = border3.getY() + sliderHeight - 90;
			int verticalSpacing = (border3.getWidth() - 3 * delaySliderWidth) / 4;
			delayLabel->setBounds(border3.getX() + verticalSpacing + (i - 1) * (delaySliderWidth + verticalSpacing), verticalPositionY - 5, delaySliderWidth, 20);
			sliderDelay->setBounds(border3.getX() + verticalSpacing + (i - 1) * (delaySliderWidth + verticalSpacing), verticalPositionY + 5, delaySliderWidth, sliderHeight - 130);
		}
	}

	// Imposta dimensioni e posizione dei togglebutton 
	distortionToggle.setBounds(border1.getX() + dialSpacing, topMargin - 155, 60, dialSize);
	filterToggle.setBounds(border2.getX() + dialSpacing, topMargin - 155, 60, dialSize);
	delayToggle.setBounds(border3.getX() + dialSpacing, topMargin - 155, 60, dialSize);
	flangerToggle.setBounds(border4.getX() + dialSpacing, border4.getY() - flangerSliderWidth / 3, 60, dialSize);
}


