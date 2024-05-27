/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
GCB_SynthAudioProcessor::GCB_SynthAudioProcessor():
#ifndef JucePlugin_PreferredChannelConfigurations
     AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    keyboardState.addListener(&midiMessageCollector);

}

GCB_SynthAudioProcessor::~GCB_SynthAudioProcessor()
{
    keyboardState.removeListener(&midiMessageCollector);

}

//==============================================================================
const juce::String GCB_SynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GCB_SynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GCB_SynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GCB_SynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GCB_SynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GCB_SynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GCB_SynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GCB_SynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GCB_SynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void GCB_SynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GCB_SynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    DBG(sampleRate);
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32)samplesPerBlock, 2 };
    audioSynth.prepare(spec);

    delay = CustomDelay(sampleRate);
    delay.prepare(spec);
    
    flanger = CustomFlanger(sampleRate);

    finalLPFilter = LPButtFilter();
    finalLPFilter.prepare(spec, FINAL_CUTOFF_FREQ);


    limiter.prepare(spec);
    limiter.setThreshold(0.f);
    limiter.setRelease(300.f);


    midiMessageCollector.reset(sampleRate);
}

void GCB_SynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GCB_SynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GCB_SynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    midiMessageCollector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    audioSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    delay.processBlock(buffer);
    flanger.processBlock(buffer);
    finalLPFilter.process(juce::dsp::ProcessContextReplacing<float>(juce::dsp::AudioBlock<float>(buffer)));
    
    //limiter.process(juce::dsp::ProcessContextReplacing<float>(juce::dsp::AudioBlock<float>(buffer)));
    
    
    
    //int numSamples = buffer.getNumSamples();
    //int startSample = 0;

    

    //juce::dsp::

    //float rms = 0.f;
    //for (int i = 0; i < buffer.getNumChannels(); i++)
    //    rms += buffer.getRMSLevel(i, startSample, numSamples);
    //rms /= buffer.getNumChannels();
    //float treshold = -9.f;
    ////DBG(juce::Decibels::gainToDecibels(rms));
    //
    //if(juce::Decibels::gainToDecibels(rms) > treshold)
    //{
    //    DBG(juce::Decibels::decibelsToGain(-(juce::Decibels::gainToDecibels(rms) - treshold)));
    //}
    //while (numSamples--)
    //{
    //    DBG(buffer.getSample(0, startSample++));
    //}
}

//==============================================================================
bool GCB_SynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GCB_SynthAudioProcessor::createEditor()
{
    return new GCB_SynthAudioProcessorEditor (*this);
}

//==============================================================================
void GCB_SynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GCB_SynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GCB_SynthAudioProcessor();
}

//asdaseffsdf
//fdjoegjdropfgjdrtfo
//ciao
//siumsmmsmsms
