/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PurrticoAudioProcessorEditor::PurrticoAudioProcessorEditor(PurrticoAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    gainSlider.setTextBoxIsEditable(false);
    addAndMakeVisible(gainSlider);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN", gainSlider);
    addAndMakeVisible(gainLabel);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setJustificationType(juce::Justification::centred);

    frequencySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    frequencySlider.setTextBoxIsEditable(false);
    addAndMakeVisible(frequencySlider);
    frequencySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQUENCY", frequencySlider);
    addAndMakeVisible(frequencyLabel);
    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    frequencyLabel.setJustificationType(juce::Justification::centred);

    qfactorSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    qfactorSlider.setTextBoxIsEditable(false);
    addAndMakeVisible(qfactorSlider);
    qfactorSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "QFACTOR", qfactorSlider);
    addAndMakeVisible(qfactorLabel);
    qfactorLabel.setText("Qfactor", juce::dontSendNotification);
    qfactorLabel.attachToComponent(&qfactorSlider, false);
    qfactorLabel.setJustificationType(juce::Justification::centred);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(475, 350);
}

PurrticoAudioProcessorEditor::~PurrticoAudioProcessorEditor()
{
}

//==============================================================================
void PurrticoAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText ("v0.6.3", getLocalBounds(), juce::Justification::bottomRight, 1);
}

void PurrticoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    juce::Rectangle<int> bounds = getLocalBounds();
    gainSlider.setBounds(25, 120, 50, 200);
    frequencySlider.setBounds(100, 120, 50, 200);
    qfactorSlider.setBounds(175, 120, 50, 200);

}