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
    juce::AudioBuffer<double> inputBuffer;

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

    juce::dsp::Gain<double> gainModule;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <double>, juce::dsp::IIR::Coefficients <double>> peakingEqualizerL;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <double>, juce::dsp::IIR::Coefficients <double>> peakingEqualizerLM;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <double>, juce::dsp::IIR::Coefficients <double>> peakingEqualizerHM;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <double>, juce::dsp::IIR::Coefficients <double>> peakingEqualizerH;

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
    double w0{};
    double G{};
    double q{};
    double p0 {};
    double p1 {};
    double p2 {};
    double A0 {};
    double A1 {};
    double A2 {};
    double R1 {};
    double R2 {};
    double B0 {};
    double B2 {};
    double B1 {};
    double W {};
    double coeffs_L[6]{};
    double coeffs_LM[6]{};
    double coeffs_HM[6]{};
    double coeffs_H[6]{};
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PurrticoAudioProcessor)
};