/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DryWetTestAudioProcessor::DryWetTestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, juce::Identifier("PARAMETERS"), createParameterLayout())
#endif
{

    treeState.state.addListener(this);

}

DryWetTestAudioProcessor::~DryWetTestAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout DryWetTestAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"PREGAIN", 1}, "PreGain", -96.0f, 48.0f, 0.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"MIX", 1}, "Mix", 0.0f, 1.0f, 0.0f));


    return {params.begin(), params.end()};
}


//==============================================================================
const juce::String DryWetTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DryWetTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DryWetTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DryWetTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DryWetTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DryWetTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DryWetTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DryWetTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DryWetTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void DryWetTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DryWetTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    waveshaper.functionToUse = [](float x)
    {
        return x / (std::abs(x) + 1);
    };
    
    preGain.setGainDecibels(*treeState.getRawParameterValue("PREGAIN"));
    
    mix.setWetMixProportion(*treeState.getRawParameterValue("MIX"));
    mix.setMixingRule(juce::dsp::DryWetMixingRule::linear);
    mix.setWetLatency(1.0f);
}

void DryWetTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DryWetTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DryWetTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //Set dry samples
    juce::dsp::AudioBlock<float> drySamplesBlock (buffer);
    mix.pushDrySamples(drySamplesBlock);
    mix.setWetMixProportion(*treeState.getRawParameterValue("MIX"));
    
    
    
    //Set preGain
    preGain.setGainDecibels(*treeState.getRawParameterValue("PREGAIN"));
    
    //Process preGain
    juce::dsp::AudioBlock<float> preGainBlock (buffer);
    preGain.process(juce::dsp::ProcessContextReplacing<float>(preGainBlock));
    
    //Process waveshaper
    juce::dsp::AudioBlock<float> waveshapeBlock (buffer);
    waveshaper.process(juce::dsp::ProcessContextReplacing<float>(waveshapeBlock));
    
    //Mix in wet samples
    juce::dsp::AudioBlock<float> wetSamplesBlock (buffer);
    mix.mixWetSamples(wetSamplesBlock);
    
}

//==============================================================================
bool DryWetTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DryWetTestAudioProcessor::createEditor()
{
    return new DryWetTestAudioProcessorEditor (*this);
}

//==============================================================================
void DryWetTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DryWetTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DryWetTestAudioProcessor();
}
