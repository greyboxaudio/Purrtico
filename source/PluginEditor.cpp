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
    debugSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    debugSlider.setTextBoxIsEditable(false);
    addAndMakeVisible(debugSlider);
    debugSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DEBUG", debugSlider);
    
    inputGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    inputGainSlider.setTextBoxIsEditable(false);
    addAndMakeVisible(inputGainSlider);
    inputGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUT", inputGainSlider);
    addAndMakeVisible(inputGainLabel);
    inputGainLabel.setText("INPUT", juce::dontSendNotification);
    inputGainLabel.attachToComponent(&inputGainSlider, false);
    inputGainLabel.setJustificationType(juce::Justification::centred);

    gainSliderL.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    gainSliderL.setTextBoxIsEditable(false);
    addAndMakeVisible(gainSliderL);
    gainSliderAttachmentL = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN_L", gainSliderL);
    addAndMakeVisible(gainLabelL);
    gainLabelL.setText("GL", juce::dontSendNotification);
    gainLabelL.attachToComponent(&gainSliderL, false);
    gainLabelL.setJustificationType(juce::Justification::centred);

    gainSliderLM.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    gainSliderLM.setTextBoxIsEditable(false);
    addAndMakeVisible(gainSliderLM);
    gainSliderAttachmentLM = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN_LM", gainSliderLM);
    addAndMakeVisible(gainLabelLM);
    gainLabelLM.setText("GLM", juce::dontSendNotification);
    gainLabelLM.attachToComponent(&gainSliderLM, false);
    gainLabelLM.setJustificationType(juce::Justification::centred);

    gainSliderHM.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    gainSliderHM.setTextBoxIsEditable(false);
    addAndMakeVisible(gainSliderHM);
    gainSliderAttachmentHM = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN_HM", gainSliderHM);
    addAndMakeVisible(gainLabelHM);
    gainLabelHM.setText("GHM", juce::dontSendNotification);
    gainLabelHM.attachToComponent(&gainSliderHM, false);
    gainLabelHM.setJustificationType(juce::Justification::centred);

    gainSliderH.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    gainSliderH.setTextBoxIsEditable(false);
    addAndMakeVisible(gainSliderH);
    gainSliderAttachmentH = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN_H", gainSliderH);
    addAndMakeVisible(gainLabelH);
    gainLabelH.setText("GH", juce::dontSendNotification);
    gainLabelH.attachToComponent(&gainSliderH, false);
    gainLabelH.setJustificationType(juce::Justification::centred);

    frequencySliderL.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    frequencySliderL.setTextBoxIsEditable(false);
    addAndMakeVisible(frequencySliderL);
    frequencySliderAttachmentL = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ_L", frequencySliderL);
    addAndMakeVisible(frequencyLabelL);
    frequencyLabelL.setText("FL", juce::dontSendNotification);
    frequencyLabelL.attachToComponent(&frequencySliderL, false);
    frequencyLabelL.setJustificationType(juce::Justification::centred);

    frequencySliderLM.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    frequencySliderLM.setTextBoxIsEditable(false);
    addAndMakeVisible(frequencySliderLM);
    frequencySliderAttachmentLM = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ_LM", frequencySliderLM);
    addAndMakeVisible(frequencyLabelLM);
    frequencyLabelLM.setText("FLM", juce::dontSendNotification);
    frequencyLabelLM.attachToComponent(&frequencySliderLM, false);
    frequencyLabelLM.setJustificationType(juce::Justification::centred);

    frequencySliderHM.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    frequencySliderHM.setTextBoxIsEditable(false);
    addAndMakeVisible(frequencySliderHM);
    frequencySliderAttachmentHM = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ_HM", frequencySliderHM);
    addAndMakeVisible(frequencyLabelHM);
    frequencyLabelHM.setText("FHM", juce::dontSendNotification);
    frequencyLabelHM.attachToComponent(&frequencySliderHM, false);
    frequencyLabelHM.setJustificationType(juce::Justification::centred);

    frequencySliderH.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    frequencySliderH.setTextBoxIsEditable(false);
    addAndMakeVisible(frequencySliderH);
    frequencySliderAttachmentH = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ_H", frequencySliderH);
    addAndMakeVisible(frequencyLabelH);
    frequencyLabelH.setText("FH", juce::dontSendNotification);
    frequencyLabelH.attachToComponent(&frequencySliderH, false);
    frequencyLabelH.setJustificationType(juce::Justification::centred);

    qfactorSliderL.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    qfactorSliderL.setTextBoxIsEditable(false);
    addAndMakeVisible(qfactorSliderL);
    qfactorSliderAttachmentL = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "QFACTOR_L", qfactorSliderL);
    addAndMakeVisible(qfactorLabelL);
    qfactorLabelL.setText("QL", juce::dontSendNotification);
    qfactorLabelL.attachToComponent(&qfactorSliderL, false);
    qfactorLabelL.setJustificationType(juce::Justification::centred);

    qfactorSliderLM.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    qfactorSliderLM.setTextBoxIsEditable(false);
    addAndMakeVisible(qfactorSliderLM);
    qfactorSliderAttachmentLM = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "QFACTOR_LM", qfactorSliderLM);
    addAndMakeVisible(qfactorLabelLM);
    qfactorLabelLM.setText("QLM", juce::dontSendNotification);
    qfactorLabelLM.attachToComponent(&qfactorSliderLM, false);
    qfactorLabelLM.setJustificationType(juce::Justification::centred);

    qfactorSliderHM.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    qfactorSliderHM.setTextBoxIsEditable(false);
    addAndMakeVisible(qfactorSliderHM);
    qfactorSliderAttachmentHM = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "QFACTOR_HM", qfactorSliderHM);
    addAndMakeVisible(qfactorLabelHM);
    qfactorLabelHM.setText("QHM", juce::dontSendNotification);
    qfactorLabelHM.attachToComponent(&qfactorSliderHM, false);
    qfactorLabelHM.setJustificationType(juce::Justification::centred);

    qfactorSliderH.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    qfactorSliderH.setTextBoxIsEditable(false);
    addAndMakeVisible(qfactorSliderH);
    qfactorSliderAttachmentH = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "QFACTOR_H", qfactorSliderH);
    addAndMakeVisible(qfactorLabelH);
    qfactorLabelH.setText("QH", juce::dontSendNotification);
    qfactorLabelH.attachToComponent(&qfactorSliderH, false);
    qfactorLabelH.setJustificationType(juce::Justification::centred);

    peakButtonL.setButtonText("PeakL");
    addAndMakeVisible(peakButtonL);
    peakButtonAttachmentL = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "PEAK_L", peakButtonL);
    peakButtonH.setButtonText("PeakH");
    addAndMakeVisible(peakButtonH);
    peakButtonAttachmentH = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "PEAK_H", peakButtonH);

    /*addAndMakeVisible(versionLabel);
    versionLabel.setText(__DATE__ " " __TIME__ " " CMAKE_BUILD_TYPE, juce::dontSendNotification);
    versionLabel.setJustificationType(juce::Justification::centred);*/

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(800, 600);
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
    g.setFont(16.0f);
    g.drawFittedText ("v0.2.1 " __DATE__ " " __TIME__, getLocalBounds(), juce::Justification::bottomRight, 1);
}

void PurrticoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    juce::Rectangle<int> bounds = getLocalBounds();
    //versionLabel.setBounds(10,  10, getWidth() - 20,  30);
    inputGainSlider.setBounds(25, 150, 50, 200);
    peakButtonH.setBounds(25,50,100,50);
    peakButtonL.setBounds(25,375,100,50);
    //top row
    frequencySliderL.setBounds(125, 25, 50, 200);
    gainSliderL.setBounds(200, 25, 50, 200);
    qfactorSliderH.setBounds(300, 25, 50, 200);
    debugSlider.setBounds(375, 25, 50, 200);
    qfactorSliderL.setBounds(450, 25, 50, 200);
    gainSliderH.setBounds(550, 25, 50, 200);
    frequencySliderH.setBounds(625, 25, 50, 200);
    //bottom row
    qfactorSliderLM.setBounds(175, 275, 50, 200);
    gainSliderLM.setBounds(250, 275, 50, 200);
    frequencySliderLM.setBounds(325, 275, 50, 200);
    qfactorSliderHM.setBounds(425, 275, 50, 200);
    gainSliderHM.setBounds(500, 275, 50, 200);
    frequencySliderHM.setBounds(575, 275, 50, 200);
}