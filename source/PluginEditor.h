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
    juce::Slider gainSlider;
    juce::Slider frequencySlider;
    juce::Slider qfactorSlider;

    juce::Label gainLabel;
    juce::Label frequencyLabel;
    juce::Label qfactorLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qfactorSliderAttachment;
    PurrticoAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PurrticoAudioProcessorEditor)
};
