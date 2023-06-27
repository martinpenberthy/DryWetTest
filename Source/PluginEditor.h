/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DryWetTestAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DryWetTestAudioProcessorEditor (DryWetTestAudioProcessor&);
    ~DryWetTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    juce::Slider sliderPreGain;
    juce::Label labelPreGain;
    
    juce::Slider sliderMix;
    juce::Label labelMix;
    
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachmentPreGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachmentMix;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DryWetTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DryWetTestAudioProcessorEditor)
};
