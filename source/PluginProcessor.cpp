/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PurrticoAudioProcessor::PurrticoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

PurrticoAudioProcessor::~PurrticoAudioProcessor()
{
}

//==============================================================================
const juce::String PurrticoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PurrticoAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PurrticoAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PurrticoAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PurrticoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PurrticoAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int PurrticoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PurrticoAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String PurrticoAudioProcessor::getProgramName(int index)
{
    return {};
}

void PurrticoAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void PurrticoAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // set up filters & dsp elements
    lastSampleRate = static_cast<float>(sampleRate);
    float smoothSlow{0.1f};
    float smoothFast{0.0005f};
    inputGainSmooth.reset(sampleRate, smoothFast);
    frequencySmooth.reset(sampleRate, smoothFast);
    qfactorSmooth.reset(sampleRate, smoothFast);
    gainSmooth.reset(sampleRate, smoothFast);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    gainModule.prepare(spec);
    gainModule.reset();
    peakingEqualizer.prepare(spec);
    peakingEqualizer.reset();
    //highShelf.prepare(spec);
    //highShelf.reset();
}

void PurrticoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PurrticoAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void PurrticoAudioProcessor::updateFilter()
{
    *peakingEqualizer.state = juce::dsp::IIR::Coefficients<float>(b0, b1, b2, a0, a1, a2);
    //*highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(lastSampleRate, frequency, qfactor, gain);
}

void PurrticoAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    auto bufferSize = buffer.getNumSamples();
    // prepare audio buffers
    inputBuffer.setSize(totalNumInputChannels, bufferSize);
    // set up dsp elements
    juce::dsp::AudioBlock<float> inputBlock(inputBuffer);
    // read smoothed parameters
    float frequencyValue = *apvts.getRawParameterValue("FREQ");
    frequencySmooth.setTargetValue(frequencyValue);
    frequency = frequencySmooth.getNextValue();
    float qfactorValue = *apvts.getRawParameterValue("QFACTOR");
    qfactorSmooth.setTargetValue(qfactorValue);
    qfactor = qfactorSmooth.getNextValue();
    float gainValue = *apvts.getRawParameterValue("GAIN");
    gainSmooth.setTargetValue(gainValue);
    gain = gainSmooth.getNextValue();
    // calculate filter coefficients
    auto pi = juce::MathConstants<float>::pi;
    auto e = juce::MathConstants<float>::euler;
    //frequency = 4800.0f;
    //gain = 9.0f;
    //qfactor = 1.0f;
    auto w0 = 2 * pi * frequency / lastSampleRate;
    auto q = 1 / (2 * qfactor);
    auto G = pow(10, gain / 20);

    a0 = 1.0f;
    if (q <= 1.0f)
    {
        a1 = -2 * (pow(e, -1 * q * w0)) * cos(sqrt(1 - pow(q, 2)) * w0);
    }
    else
    {
        a1 = -2 * (pow(e, -1 * q * w0)) * cosh(sqrt(pow(q, 2) - 1) * w0);
    }
    a2 = pow(e, -2 * q * w0);

    auto p0 = 1 - pow(sin(w0 / 2), 2);
    auto p1 = pow(sin(w0 / 2), 2);
    auto p2 = 4 * p0 * p1;

    auto A0 = pow(1 + a1 + a2, 2);
    auto A1 = pow(1 - a1 + a2, 2);
    auto A2 = -4 * a2;

    auto R1 = (A0 * p0 + A1 * p1 + A2 * p2) * pow(G, 2);
    auto R2 = (-1 * A0 + A1 + 4 * (p0 - p1) * A2) * pow(G, 2);

    auto B0 = A0;
    auto B2 = (R1 - R2 * p1 - B0) / (4 * pow(p1, 2));
    auto B1 = R2 + B0 + 4 * (p1 - p0) * B2;

    auto W = 0.5 * (sqrt(B0) + sqrt(B1));
    b0 = 0.5 * (W + sqrt(pow(W, 2) + B2));
    b1 = 0.5 * (sqrt(B0) - sqrt(B1));
    b2 = -1 * B2 / (4 * b0);

    // update filters
    updateFilter();
    // clear buffers
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
        inputBuffer.clear(channel, 0, bufferSize);
    // copy audio buffer to inputBuffer
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        inputBuffer.copyFrom(channel, 0, buffer, channel, 0, bufferSize);
    }
    // apply input gain
    float inputGainValue = *apvts.getRawParameterValue("INPUT");
    inputGainSmooth.setTargetValue(inputGainValue);
    inputGain = inputGainSmooth.getNextValue();
    inputGain = pow(10, inputGain / 20);
    gainModule.setGainLinear(inputGain);
    gainModule.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    // apply vicanek filter
    peakingEqualizer.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    //highShelf.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    // write input buffer back to main buffer
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        buffer.copyFrom(channel, 0, inputBuffer, channel, 0, bufferSize);
    }
}

//==============================================================================
bool PurrticoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *PurrticoAudioProcessor::createEditor()
{
    return new PurrticoAudioProcessorEditor(*this);
}

//==============================================================================
void PurrticoAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PurrticoAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new PurrticoAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout PurrticoAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("INPUT", "inputGain", -12.0f, 12.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", -12.0f, 12.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FREQ", "Frequency", 1800.0f, 16000.0f, 6000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("QFACTOR", "QFactor", 0.57f, 4.12f, 1.63f));
    return {parameters.begin(), parameters.end()};
}