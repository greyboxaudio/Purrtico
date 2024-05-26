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
    juce::LinearSmoothedValue<float> frequencySmoothL{};
    juce::LinearSmoothedValue<float> qfactorSmoothL{};
    juce::LinearSmoothedValue<float> gainSmoothL{};
    juce::LinearSmoothedValue<float> frequencySmoothLM{};
    juce::LinearSmoothedValue<float> qfactorSmoothLM{};
    juce::LinearSmoothedValue<float> gainSmoothLM{};
    juce::LinearSmoothedValue<float> frequencySmoothHM{};
    juce::LinearSmoothedValue<float> qfactorSmoothHM{};
    juce::LinearSmoothedValue<float> gainSmoothHM{};
    juce::LinearSmoothedValue<float> frequencySmoothH{};
    juce::LinearSmoothedValue<float> qfactorSmoothH{};
    juce::LinearSmoothedValue<float> gainSmoothH{};

    juce::dsp::Gain<float> gainModule;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> peakingEqualizerL;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> peakingEqualizerLM;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> peakingEqualizerHM;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> peakingEqualizerH;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> lowShelf;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> highShelf;

    float inputGain{};
    float frequencyL{};
    float qfactorL{};
    float gainL{};
    float frequencyLM{};
    float qfactorLM{};
    float gainLM{};
    float frequencyHM{};
    float qfactorHM{};
    float gainHM{};
    float frequencyH{};
    float qfactorH{};
    float gainH{};
    float lastSampleRate{};
    float w0{};
    float G{};
    float q{};
    float p0 {};
    float p1 {};
    float p2 {};
    float A0 {};
    float A1 {};
    float A2 {};
    float R1 {};
    float R2 {};
    float B0 {};
    float B2 {};
    float B1 {};
    float W {};
    float coeffs_L[6]{};
    float coeffs_LM[6]{};
    float coeffs_HM[6]{};
    float coeffs_H[6]{};
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PurrticoAudioProcessor)
};