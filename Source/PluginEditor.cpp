
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
	dialOscillator.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
	dialOscillator.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
	dialOscillator.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colours::transparentWhite);

	addAndMakeVisible(waveTypeLabel);
	waveTypeLabel.setJustificationType(juce::Justification::centred);
	waveTypeLabel.setFont(juce::Font(12.0f, juce::Font::italic));
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

	/*for (int i = 0; i < maxVerticalSliders * 2; ++i) {
		auto slider = std::make_unique<juce::Slider>();// Crea uno slider verticale e lo assegna a std::unique_ptr
		slider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		slider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		slider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		slider->setRange(0, 1000, 1);
		slider->setTextValueSuffix(" ms");
		addAndMakeVisible(*slider);

		if (i < maxVerticalSliders) {
			oscillatorSliders.push_back(std::move(slider)); // Aggiunge lo slider verticale al vettore per l'inviluppo dell'oscillatore
			auto* currentSlider = oscillatorSliders.back().get(); // Ottengo un puntatore grezzo allo slider corrente
			currentSlider->onValueChange = [currentSlider, this, i]() {
				float sliderValue = currentSlider->getValue();
				audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::attack, i, sliderValue);
				};
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::attack, currentSlider->getValue());
		}
		else {
			releseOscillatorSliders.push_back(std::move(slider)); // Aggiunge lo slider verticale al vettore per l'inviluppo del filtro
			auto* currentSlider = releseOscillatorSliders.back().get();
			currentSlider->onValueChange = [currentSlider, this, i]() {
				float sliderValue = currentSlider->getValue();
				int releaseIndex = i - maxVerticalSliders;
				audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::release, releaseIndex, sliderValue);
				};
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::release, currentSlider->getValue());
		}
	}*/

	// Create oscillator sliders
	for (int i = 0; i < numAttackSliders; ++i) {
		// Creazione e configurazione dello slider per l'attacco dell'oscillatore
		auto oscillatorSlider = std::make_unique<juce::Slider>();
		oscillatorSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		oscillatorSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		oscillatorSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		oscillatorSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		oscillatorSlider->setRange(0, 1000, 1);
		oscillatorSlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*oscillatorSlider);

		// Aggiunta dello slider all'array degli slider per l'attacco dell'oscillatore
		attackOscillatorSliders.push_back(std::move(oscillatorSlider));

		// Cattura della variabile dello slider corrente
		auto* currentOscillatorSlider = attackOscillatorSliders.back().get();

		// Callback per l'aggiornamento del valore dello slider dell'oscillatore
		currentOscillatorSlider->onValueChange = [currentOscillatorSlider, this, i]() {
			float sliderValue = currentOscillatorSlider->getValue();
			audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::attack, i, sliderValue);
			};

		// Aggiunta dell'envelope ramp per l'attacco dell'oscillatore
		audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::attack, currentOscillatorSlider->getValue());

		// Creazione e configurazione dello slider per l'attacco del filtro
		auto filterSlider = std::make_unique<juce::Slider>();
		filterSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		filterSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		filterSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		filterSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		filterSlider->setRange(0, 1000, 1);
		filterSlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*filterSlider);

		// Aggiunta dello slider all'array degli slider per l'attacco del filtro
		attackFilterSliders.push_back(std::move(filterSlider));

		// Cattura della variabile dello slider corrente per il filtro
		auto* currentFilterSlider = attackFilterSliders.back().get();

		// Callback per l'aggiornamento del valore dello slider del filtro
		currentFilterSlider->onValueChange = [currentFilterSlider, this, i]() {
			float sliderValue = currentFilterSlider->getValue();
			audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::attack, i, sliderValue);
			};

		// Aggiunta dell'envelope ramp per l'attacco del filtro
		audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::attack, currentFilterSlider->getValue());
	}

	// Create release sliders
	for (int i = 0; i < numReleaseSliders; ++i) {
		// Creazione e configurazione dello slider per il rilascio dell'oscillatore
		auto oscillatorSlider = std::make_unique<juce::Slider>();
		oscillatorSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		oscillatorSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		oscillatorSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		oscillatorSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		oscillatorSlider->setRange(0, 1000, 1);
		oscillatorSlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*oscillatorSlider);

		// Aggiunta dello slider all'array degli slider per il rilascio dell'oscillatore
		releaseOscillatorSliders.push_back(std::move(oscillatorSlider));

		// Cattura della variabile dello slider corrente per l'oscillatore
		auto* currentOscillatorSlider = releaseOscillatorSliders.back().get();

		// Callback per l'aggiornamento del valore dello slider dell'oscillatore
		currentOscillatorSlider->onValueChange = [currentOscillatorSlider, this, i]() {
			float sliderValue = currentOscillatorSlider->getValue();
			audioProcessor.audioSynth.SetRampTargetValue(EnvType::gainEnv, EnvState::release, i, sliderValue);
			};

		// Aggiunta dell'envelope ramp per il rilascio dell'oscillatore
		audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::gainEnv, EnvState::release, currentOscillatorSlider->getValue());

		// Creazione e configurazione dello slider per il rilascio del filtro
		auto filterSlider = std::make_unique<juce::Slider>();
		filterSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		filterSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		filterSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		filterSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		filterSlider->setRange(0, 1000, 1);
		filterSlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*filterSlider);

		// Aggiunta dello slider all'array degli slider per il rilascio del filtro
		releaseFilterSliders.push_back(std::move(filterSlider));

		// Cattura della variabile dello slider corrente per il filtro
		auto* currentFilterSlider = releaseFilterSliders.back().get();

		// Callback per l'aggiornamento del valore dello slider del filtro
		currentFilterSlider->onValueChange = [currentFilterSlider, this, i]() {
			float sliderValue = currentFilterSlider->getValue();
			audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::release, i, sliderValue);
			};

		// Aggiunta dell'envelope ramp per il rilascio del filtro
		audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::release, currentFilterSlider->getValue());

		// Creazione e configurazione dello slider per il flanger
		auto flangerSlider = std::make_unique<juce::Slider>();
		flangerSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		flangerSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		flangerSlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		flangerSlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		flangerSlider->setRange(0, 1000, 1);
		flangerSlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*flangerSlider);

		flangerSliders.push_back(std::move(flangerSlider));
	}


	for (int i = 0; i < numDelaySliders; ++i) {
		
		// Creazione e configurazione dello slider per il flanger
		auto delaySlider = std::make_unique<juce::Slider>();
		delaySlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		delaySlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		delaySlider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		delaySlider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		delaySlider->setRange(0, 1000, 1);
		delaySlider->setTextValueSuffix(" ms");
		addAndMakeVisible(*delaySlider);

		delaySliders.push_back(std::move(delaySlider));
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
	dialGain.setRange(1.0, 5.0, 1.0);
	dialGain.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
	dialGain.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
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
	dialFilter.onValueChange = [this]() {
		double filterValue = dialFilter.getValue();
		audioProcessor.audioSynth.SetLPFilterResonance(filterValue);
		};

	addAndMakeVisible(border2);
	border2.setText("Filter");

	//slider per l'inviluppo del filter
	/*for (int i = 0; i < maxVerticalSliders * 2; ++i) {
		auto slider = std::make_unique<juce::Slider>(); // Crea uno slider verticale e lo assegna a std::unique_ptr
		slider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 10);
		slider->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
		slider->setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
		slider->setRange(5, 5000, 1);
		slider->setTextValueSuffix(" ms");
		addAndMakeVisible(*slider);

		if (i < maxVerticalSliders) {
			attackFilterSliders.push_back(std::move(slider)); // Aggiunge lo slider verticale al vettore
			auto* currentSlider = attackFilterSliders.back().get();
			currentSlider->onValueChange = [currentSlider, this, i]() {
				float sliderValue = currentSlider->getValue();

				audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::attack, i, sliderValue);
				};
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::attack, currentSlider->getValue());
		}
		else {
			releseFilterSliders.push_back(std::move(slider)); // Aggiunge lo slider verticale al vettore
			auto* currentSlider = releseFilterSliders.back().get();
			currentSlider->onValueChange = [currentSlider, this, i]() {
				float sliderValue = currentSlider->getValue();
				int releaseIndex = i - maxVerticalSliders;

				audioProcessor.audioSynth.SetRampTargetValue(EnvType::lpFilterEnv, EnvState::release, releaseIndex, sliderValue);
				};
			audioProcessor.audioSynth.AddEnvelopeRamp(EnvType::lpFilterEnv, EnvState::release, currentSlider->getValue());
		}
	}*/

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

	setSize(1000, 550);

}

GCB_SynthAudioProcessorEditor::~GCB_SynthAudioProcessorEditor()
{
}

//==============================================================================
void GCB_SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)

	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

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

	// Imposta dimensioni e posizione dei togglebutton 
	distortionToggle.setBounds(border1.getX() + dialSpacing, topMargin - 155, 150, dialSize);
	filterToggle.setBounds(border2.getX() + dialSpacing, topMargin - 155, 150, dialSize);

	// Imposta la posizione delle etichette
	label1.setBounds(dialDistortion.getX(), dialDistortion.getY() - 15, label1.getWidth(), 25);
	label2.setBounds(dialBias.getX(), dialBias.getY() - 15, label2.getWidth(), 25);
	label3.setBounds(dialGain.getX(), dialGain.getY() - 15, label3.getWidth(), 25);
	waveTypeLabel.setBounds(dialOscillator.getX(), dialOscillator.getY() + dialOscillator.getHeight() - 30, dialOscillator.getWidth(), 25);
	functionTypeLabel.setBounds(dialDistortion.getX(), dialDistortion.getY() + dialDistortion.getHeight() - 30, dialDistortion.getWidth(), 25);

	auto attackSliderWidth = border.getWidth() / numAttackSliders;
	auto releaseSliderWidth = border.getWidth() / numReleaseSliders;
	auto flangerSliderWidth = border4.getWidth() / numReleaseSliders;
	auto delaySliderWidth = border3.getWidth() / numDelaySliders;
	auto sliderHeight = border.getHeight() * 0.5;

	// Posizionamento degli slider di attacco
	for (int i = 0; i < numAttackSliders; ++i) {
		auto sliderattackOsc = attackOscillatorSliders[i].get();
		auto sliderattackFil = attackFilterSliders[i].get();
		sliderattackOsc->setBounds(border.getX() + i * attackSliderWidth, border.getY() + sliderHeight - 40, attackSliderWidth, sliderHeight - 100);
		oscillatorAttackBorder.setBounds(border.getX() + 12, sliderattackOsc->getY() - 30, attackSliderWidth * 4 - 24, sliderHeight - 60);
		sliderattackFil->setBounds(border2.getX() + i * attackSliderWidth, border2.getY() + sliderHeight - 40, attackSliderWidth, sliderHeight - 100);
		filterAttackBorder.setBounds(border2.getX() + 12, sliderattackFil->getY() - 30, attackSliderWidth * 4 - 24, sliderHeight - 60);
	}

	// Posizionamento degli slider di rilascio
	for (int i = 0; i < numReleaseSliders; ++i) {
		auto sliderReleaseOsc = releaseOscillatorSliders[i].get();
		auto sliderReleaseFil = releaseFilterSliders[i].get();
		auto sliderFlanger = flangerSliders[i].get();

		sliderReleaseOsc->setBounds(border.getX() + i * releaseSliderWidth, border.getY() + sliderHeight + 80, releaseSliderWidth, sliderHeight - 100);
		oscillatorReleaseBorder.setBounds(border.getX() + 15, sliderReleaseOsc->getY() - 10, releaseSliderWidth * 3 - 30, sliderHeight - 80);
		sliderReleaseFil->setBounds(border2.getX() + i * releaseSliderWidth, border2.getY() + sliderHeight + 80, releaseSliderWidth, sliderHeight - 100);
		filterReleaseBorder.setBounds(border2.getX() + 15, sliderReleaseFil->getY() - 10, releaseSliderWidth * 3 - 30, sliderHeight - 80);

		sliderFlanger->setBounds(border4.getX() + i * flangerSliderWidth, border4.getY() + border4.getHeight() / 4, flangerSliderWidth, sliderHeight - 100);
	}

	for (int i = 0; i < numDelaySliders; ++i) {
		auto sliderDelay = delaySliders[i].get();
		sliderDelay->setBounds(border3.getX() + i * delaySliderWidth, border3.getY() + border3.getHeight() / 4, delaySliderWidth, sliderHeight - 100);
	}
}


