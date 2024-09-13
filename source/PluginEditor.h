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
    juce::Slider debugSlider;
    juce::Slider inputGainSlider;
    
    juce::Slider gainSliderL;
    juce::Slider frequencySliderL;
    juce::Slider qfactorSliderL;
    juce::Slider gainSliderLM;
    juce::Slider frequencySliderLM;
    juce::Slider qfactorSliderLM;
    juce::Slider gainSliderHM;
    juce::Slider frequencySliderHM;
    juce::Slider qfactorSliderHM;
    juce::Slider gainSliderH;
    juce::Slider frequencySliderH;
    juce::Slider qfactorSliderH;
    juce::ToggleButton peakButtonL;
    juce::ToggleButton peakButtonH;
    juce::ToggleButton inButtonLM;
    juce::ToggleButton inButtonM;
    juce::ToggleButton inButtonHM;

    juce::Label versionLabel;
    juce::Label inputGainLabel;
    juce::Label gainLabelL;
    juce::Label frequencyLabelL;
    juce::Label gainLabelLM;
    juce::Label frequencyLabelLM;
    juce::Label qfactorLabelLM;
    juce::Label gainLabelHM;
    juce::Label frequencyLabelHM;
    juce::Label qfactorLabelHM;
    juce::Label gainLabelH;
    juce::Label frequencyLabelH;

    juce::Label qfactorLabelH;
    juce::Label qfactorLabelL;
    juce::Label gainLabelM;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> debugSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachmentL;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachmentL;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qfactorSliderAttachmentL;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachmentLM;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachmentLM;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qfactorSliderAttachmentLM;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachmentHM;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachmentHM;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qfactorSliderAttachmentHM;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachmentH;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachmentH;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qfactorSliderAttachmentH;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> peakButtonAttachmentL;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> peakButtonAttachmentH;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inButtonAttachmentLM;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inButtonAttachmentM;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inButtonAttachmentHM;
    PurrticoAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PurrticoAudioProcessorEditor)
};
