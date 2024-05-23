/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class PurrticoAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    PurrticoAudioProcessor();
    ~PurrticoAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void updateFilter();
    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    juce::AudioBuffer<float> inputBuffer;

    juce::LinearSmoothedValue<float> inputGainSmooth{};
    juce::LinearSmoothedValue<float> frequencySmooth{};
    juce::LinearSmoothedValue<float> qfactorSmooth{};
    juce::LinearSmoothedValue<float> gainSmooth{};

    juce::dsp::Gain<float> gainModule;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> peakingEqualizer;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> highShelf;

    float inputGain{};
    float frequency{};
    float qfactor{};
    float gain{};
    float lastSampleRate{};
    float a0{};
    float a1{};
    float a2{};
    float b0{};
    float b1{};
    float b2{};
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PurrticoAudioProcessor)
};