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
class PurrticoAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    PurrticoAudioProcessorEditor(PurrticoAudioProcessor&);
    ~PurrticoAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::Slider inputGainSlider;
    juce::Slider qfactorSlider;
    juce::Slider gainSlider;
    juce::Slider frequencySlider;

    juce::Label inputGainLabel;
    juce::Label qfactorLabel;
    juce::Label gainLabel;
    juce::Label frequencyLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qfactorSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachment;
    PurrticoAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PurrticoAudioProcessorEditor)
};
