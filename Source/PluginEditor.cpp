/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DryWetTestAudioProcessorEditor::DryWetTestAudioProcessorEditor (DryWetTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 400);
    
    
    addAndMakeVisible(sliderPreGain);
    sliderPreGain.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    labelPreGain.attachToComponent(&sliderPreGain, false);
    labelPreGain.setText("PreGain(dB)", juce::dontSendNotification);
    
    
    addAndMakeVisible(sliderMix);
    sliderMix.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    labelMix.attachToComponent(&sliderMix, false);
    labelMix.setText("Mix", juce::dontSendNotification);
    
    
    sliderAttachmentPreGain = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "PREGAIN", sliderPreGain);
    
    sliderAttachmentMix = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "MIX", sliderMix);
}

DryWetTestAudioProcessorEditor::~DryWetTestAudioProcessorEditor()
{
}

//==============================================================================
void DryWetTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void DryWetTestAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    sliderPreGain.setBounds(50, getHeight() / 2, 150, 150);
    sliderMix.setBounds(250, getHeight() / 2, 150, 150);

}
