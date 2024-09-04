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
    float smoothFast{0.0005f};
    inputGainSmooth.reset(sampleRate, smoothFast);
    frequencySmoothL.reset(sampleRate, smoothFast);
    qfactorSmoothL.reset(sampleRate, smoothFast);
    gainSmoothL.reset(sampleRate, smoothFast);
    frequencySmoothLM.reset(sampleRate, smoothFast);
    qfactorSmoothLM.reset(sampleRate, smoothFast);
    gainSmoothLM.reset(sampleRate, smoothFast);
    frequencySmoothHM.reset(sampleRate, smoothFast);
    qfactorSmoothHM.reset(sampleRate, smoothFast);
    gainSmoothHM.reset(sampleRate, smoothFast);
    frequencySmoothH.reset(sampleRate, smoothFast);
    qfactorSmoothH.reset(sampleRate, smoothFast);
    gainSmoothH.reset(sampleRate, smoothFast);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    gainModule.prepare(spec);
    gainModule.reset();
    peakingEqualizerL.prepare(spec);
    peakingEqualizerL.reset();
    peakingEqualizerLM.prepare(spec);
    peakingEqualizerLM.reset();
    peakingEqualizerHM.prepare(spec);
    peakingEqualizerHM.reset();
    peakingEqualizerH.prepare(spec);
    peakingEqualizerH.reset();
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
    *peakingEqualizerL.state = juce::dsp::IIR::Coefficients<double>(coeffs_L[0], coeffs_L[1], coeffs_L[2], coeffs_L[3], coeffs_L[4], coeffs_L[5]);
    *peakingEqualizerLM.state = juce::dsp::IIR::Coefficients<double>(coeffs_LM[0], coeffs_LM[1], coeffs_LM[2], coeffs_LM[3], coeffs_LM[4], coeffs_LM[5]);
    *peakingEqualizerHM.state = juce::dsp::IIR::Coefficients<double>(coeffs_HM[0], coeffs_HM[1], coeffs_HM[2], coeffs_HM[3], coeffs_HM[4], coeffs_HM[5]);
    *peakingEqualizerH.state = juce::dsp::IIR::Coefficients<double>(coeffs_H[0], coeffs_H[1], coeffs_H[2], coeffs_H[3], coeffs_H[4], coeffs_H[5]);
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
    juce::dsp::AudioBlock<double> inputBlock(inputBuffer);
    // read smoothed parameters
    float frequencyValueL = *apvts.getRawParameterValue("FREQ_L");
    frequencySmoothL.setTargetValue(frequencyValueL);
    frequencyL = frequencySmoothL.getNextValue();
    float qfactorValueL = *apvts.getRawParameterValue("QFACTOR_L");
    qfactorSmoothL.setTargetValue(qfactorValueL);
    qfactorL = qfactorSmoothL.getNextValue();
    float gainValueL = *apvts.getRawParameterValue("GAIN_L");
    gainSmoothL.setTargetValue(gainValueL);
    gainL = gainSmoothL.getNextValue();
    float frequencyValueLM = *apvts.getRawParameterValue("FREQ_LM");
    frequencySmoothLM.setTargetValue(frequencyValueLM);
    frequencyLM = frequencySmoothLM.getNextValue();
    float qfactorValueLM = *apvts.getRawParameterValue("QFACTOR_LM");
    qfactorSmoothLM.setTargetValue(qfactorValueLM);
    qfactorLM = qfactorSmoothLM.getNextValue();
    float gainValueLM = *apvts.getRawParameterValue("GAIN_LM");
    gainSmoothLM.setTargetValue(gainValueLM);
    gainLM = gainSmoothLM.getNextValue();
    float frequencyValueHM = *apvts.getRawParameterValue("FREQ_HM");
    frequencySmoothHM.setTargetValue(frequencyValueHM);
    frequencyHM = frequencySmoothHM.getNextValue();
    float qfactorValueHM = *apvts.getRawParameterValue("QFACTOR_HM");
    qfactorSmoothHM.setTargetValue(qfactorValueHM);
    qfactorHM = qfactorSmoothHM.getNextValue();
    float gainValueHM = *apvts.getRawParameterValue("GAIN_HM");
    gainSmoothHM.setTargetValue(gainValueHM);
    gainHM = gainSmoothHM.getNextValue();
    float frequencyValueH = *apvts.getRawParameterValue("FREQ_H");
    frequencySmoothH.setTargetValue(frequencyValueH);
    frequencyH = frequencySmoothH.getNextValue();
    float qfactorValueH = *apvts.getRawParameterValue("QFACTOR_H");
    qfactorSmoothH.setTargetValue(qfactorValueH);
    qfactorH = qfactorSmoothH.getNextValue();
    float gainValueH = *apvts.getRawParameterValue("GAIN_H");
    gainSmoothH.setTargetValue(gainValueH);
    gainH = gainSmoothH.getNextValue();
    // calculate filter coefficients
    double pi = juce::MathConstants<double>::pi;
    double e = juce::MathConstants<double>::euler;

    // Low Frequency
    bool peakButtonStateL = *apvts.getRawParameterValue("PEAK_L");
    double A = pow(10, (-1 * gainL) / 40);
    if (peakButtonStateL == true)
    {
        w0 = 2 * pi * (frequencyL / lastSampleRate);
        double sinw0 = sin(w0);
        double cosw0 = cos(w0);
        double Q = qfactorL;
        double alpha = sinw0 / (2 * Q);
        double b0 = 1 + alpha * A;
        double b1 = -2 * cosw0;
        double b2 = 1 - alpha * A;
        double a0 = 1 + alpha / A;
        double a1 = -2 * cosw0;
        double a2 = 1 - alpha / A;
        coeffs_L[0] = a0;
        coeffs_L[1] = a1;
        coeffs_L[2] = a2;
        coeffs_L[3] = b0;
        coeffs_L[4] = b1;
        coeffs_L[5] = b2;
    }
    else
    {
        w0 = 2 * pi * ((2 * frequencyL) / lastSampleRate);
        double sinw0 = sin(w0);
        double cosw0 = cos(w0);
        double Q = 0.707;
        double alpha = sinw0 / (2 * Q);
        double b0 = A * ((A + 1) - (A - 1) * cosw0 + 2 * sqrt(A) * alpha);
        double b1 = 2 * A * ((A - 1) - (A + 1) * cosw0);
        double b2 = A * ((A + 1) - (A - 1) * cosw0 - 2 * sqrt(A) * alpha);
        double a0 = (A + 1) + (A - 1) * cosw0 + 2 * sqrt(A) * alpha;
        double a1 = -2 * ((A - 1) + (A + 1) * cosw0);
        double a2 = (A + 1) + (A - 1) * cosw0 - 2 * sqrt(A) * alpha;
        coeffs_L[0] = a0;
        coeffs_L[1] = a1;
        coeffs_L[2] = a2;
        coeffs_L[3] = b0;
        coeffs_L[4] = b1;
        coeffs_L[5] = b2;
    }

    // Low Mid Frequency
    w0 = 2 * pi * frequencyLM / lastSampleRate;
    G = pow(10, gainLM / 20);
    q = 1 / (2 * sqrt(G) * qfactorLM);
    coeffs_LM[3] = 1.0f;
    if (q <= 1.0f)
    {
        coeffs_LM[4] = -2 * pow(e, -1 * q * w0) * cos(sqrt(1 - pow(q, 2)) * w0);
    }
    else
    {
        coeffs_LM[4] = -2 * pow(e, -1 * q * w0) * cosh(sqrt(pow(q, 2) - 1) * w0);
    }
    coeffs_LM[5] = pow(e, -2 * q * w0);
    p0 = 1 - pow(sin(w0 / 2), 2);
    p1 = pow(sin(w0 / 2), 2);
    p2 = 4 * p0 * p1;
    A0 = pow(1 + coeffs_LM[4] + coeffs_LM[5], 2);
    A1 = pow(1 - coeffs_LM[4] + coeffs_LM[5], 2);
    A2 = -4 * coeffs_LM[5];
    R1 = (A0 * p0 + A1 * p1 + A2 * p2) * pow(G, 2);
    R2 = (-1 * A0 + A1 + 4 * (p0 - p1) * A2) * pow(G, 2);
    B0 = A0;
    B2 = (R1 - R2 * p1 - B0) / (4 * pow(p1, 2));
    B1 = R2 + B0 + 4 * (p1 - p0) * B2;
    W = 0.5 * (sqrt(B0) + sqrt(B1));
    coeffs_LM[0] = 0.5 * (W + sqrt(pow(W, 2) + B2));
    coeffs_LM[1] = 0.5 * (sqrt(B0) - sqrt(B1));
    coeffs_LM[2] = -1 * B2 / (4 * coeffs_LM[0]);

    // High Mid Frequency
    w0 = 2 * pi * frequencyHM / lastSampleRate;
    G = pow(10, gainHM / 20);
    q = 1 / (2 * sqrt(G) * qfactorHM);
    coeffs_HM[3] = 1.0f;
    if (q <= 1.0f)
    {
        coeffs_HM[4] = -2 * pow(e, -1 * q * w0) * cos(sqrt(1 - pow(q, 2)) * w0);
    }
    else
    {
        coeffs_HM[4] = -2 * pow(e, -1 * q * w0) * cosh(sqrt(pow(q, 2) - 1) * w0);
    }
    coeffs_HM[5] = pow(e, -2 * q * w0);
    p0 = 1 - pow(sin(w0 / 2), 2);
    p1 = pow(sin(w0 / 2), 2);
    p2 = 4 * p0 * p1;
    A0 = pow(1 + coeffs_HM[4] + coeffs_HM[5], 2);
    A1 = pow(1 - coeffs_HM[4] + coeffs_HM[5], 2);
    A2 = -4 * coeffs_HM[5];
    R1 = (A0 * p0 + A1 * p1 + A2 * p2) * pow(G, 2);
    R2 = (-1 * A0 + A1 + 4 * (p0 - p1) * A2) * pow(G, 2);
    B0 = A0;
    B2 = (R1 - R2 * p1 - B0) / (4 * pow(p1, 2));
    B1 = R2 + B0 + 4 * (p1 - p0) * B2;
    W = 0.5 * (sqrt(B0) + sqrt(B1));
    coeffs_HM[0] = 0.5 * (W + sqrt(pow(W, 2) + B2));
    coeffs_HM[1] = 0.5 * (sqrt(B0) - sqrt(B1));
    coeffs_HM[2] = -1 * B2 / (4 * coeffs_HM[0]);

    // High Frequency
    bool peakButtonStateH = *apvts.getRawParameterValue("PEAK_H");
    if (peakButtonStateH == true)
    {
        w0 = 2 * pi * frequencyH / lastSampleRate;
        G = pow(10, gainH / 20);
        q = 1 / (2 * sqrt(G) * qfactorH);
        coeffs_H[3] = 1.0f;
        if (q <= 1.0f)
        {
            coeffs_H[4] = -2 * pow(e, -1 * q * w0) * cos(sqrt(1 - pow(q, 2)) * w0);
        }
        else
        {
            coeffs_H[4] = -2 * pow(e, -1 * q * w0) * cosh(sqrt(pow(q, 2) - 1) * w0);
        }
        coeffs_H[5] = pow(e, -2 * q * w0);
        p0 = 1 - pow(sin(w0 / 2), 2);
        p1 = pow(sin(w0 / 2), 2);
        p2 = 4 * p0 * p1;
        A0 = pow(1 + coeffs_H[4] + coeffs_H[5], 2);
        A1 = pow(1 - coeffs_H[4] + coeffs_H[5], 2);
        A2 = -4 * coeffs_H[5];
        R1 = (A0 * p0 + A1 * p1 + A2 * p2) * pow(G, 2);
        R2 = (-1 * A0 + A1 + 4 * (p0 - p1) * A2) * pow(G, 2);
        B0 = A0;
        B2 = (R1 - R2 * p1 - B0) / (4 * pow(p1, 2));
        B1 = R2 + B0 + 4 * (p1 - p0) * B2;
        W = 0.5 * (sqrt(B0) + sqrt(B1));
        coeffs_H[0] = 0.5 * (W + sqrt(pow(W, 2) + B2));
        coeffs_H[1] = 0.5 * (sqrt(B0) - sqrt(B1));
        coeffs_H[2] = -1 * B2 / (4 * coeffs_H[0]);
    }
    else
    {
        double fc = frequencyH / lastSampleRate;
        double gain = (pow(10, (-1 * gainH) / 20));
        double g;
        if (abs(1 - gain) < 1e-6)
        {
            g = 1.00001;
        }
        else
        {
            g = gain;
        }
        // abbreviations
        double pihalf = pi * 0.5;
        double invg = 1.0 / g;
        // matching gain at Nyquist
        double fc4 = pow(fc, 4);
        double hny = (fc4 + g) / (fc4 + invg);
        // matching gain at f_1
        double f1 = fc / sqrt(0.160 + 1.543 * fc * fc);
        double f14 = pow(f1, 4);
        double h1 = (fc4 + f14 * g) / (fc4 + f14 * invg);
        double phi1 = pow(sin(pihalf * f1), 2);
        // matching gain at f_2
        double f2 = fc / sqrt(0.947 + 3.806 * fc * fc);
        double f24 = pow(f2, 4);
        double h2 = (fc4 + f24 * g) / (fc4 + f24 * invg);
        double phi2 = pow(sin(pihalf * f2), 2);
        // linear equations coefficients
        double d1 = (h1 - 1.0) * (1.0 - phi1);
        double c11 = -phi1 * d1;
        double c12 = phi1 * phi1 * (hny - h1);
        double d2 = (h2 - 1.0) * (1.0 - phi2);
        double c21 = -phi2 * d2;
        double c22 = phi2 * phi2 * (hny - h2);
        // linear equations solution
        double alfa1 = (c22 * d1 - c12 * d2) / (c11 * c22 - c12 * c21);
        double aa1 = (d1 - c11 * alfa1) / c12;
        double bb1 = hny * aa1;
        // compute A_2 and B_2
        double aa2 = 0.25 * (alfa1 - aa1);
        double bb2 = 0.25 * (alfa1 - bb1);
        // compute biquad coefficients scaled with 1/a_0
        double v = 0.5 * (1.0 + sqrt(aa1));
        double w = 0.5 * (1.0 + sqrt(bb1));
        double a0 = 0.5 * (v + sqrt(v * v + aa2));
        double inva0 = 1.0 / a0;
        double a1 = (1.0 - v) * inva0;
        double a2 = -0.25 * aa2 * inva0 * inva0;
        double b0 = (0.5 * (w + sqrt(w * w + bb2))) * inva0;
        double b1 = (1.0 - w) * inva0;
        double b2 = (-0.25 * bb2 / b0) * inva0 * inva0;
        coeffs_H[0] = 1.0;
        coeffs_H[1] = a1;
        coeffs_H[2] = a2;
        coeffs_H[3] = b0;
        coeffs_H[4] = b1;
        coeffs_H[5] = b2;
    }
    // update filters
    updateFilter();
    // clear buffers
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
        inputBuffer.clear(channel, 0, bufferSize);
    // copy audio buffer to inputBuffer
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        for (int i = 0; i < bufferSize; ++i)
        {
            inputBuffer.setSample(channel, i, static_cast<double>(buffer.getSample(channel, i)));
        }
    }
    // apply filter
    peakingEqualizerL.process(juce::dsp::ProcessContextReplacing<double>(inputBlock));
    peakingEqualizerLM.process(juce::dsp::ProcessContextReplacing<double>(inputBlock));
    peakingEqualizerHM.process(juce::dsp::ProcessContextReplacing<double>(inputBlock));
    peakingEqualizerH.process(juce::dsp::ProcessContextReplacing<double>(inputBlock));

    // apply input gain
    float inputGainValue = *apvts.getRawParameterValue("INPUT");
    inputGainSmooth.setTargetValue(inputGainValue);
    inputGain = inputGainSmooth.getNextValue();
    inputGain = static_cast<float>(pow(10, inputGain / 20));
    gainModule.setGainLinear(inputGain);
    gainModule.process(juce::dsp::ProcessContextReplacing<double>(inputBlock));

    // write input buffer back to main buffer
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        // buffer.copyFrom(channel, 0, inputBuffer, channel, 0, bufferSize);
        for (int i = 0; i < bufferSize; ++i)
        {
            buffer.setSample(channel, i, static_cast<float>(inputBuffer.getSample(channel, i)));
        }
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

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("INPUT", "inputGain", -18.0f, 18.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("PEAK_L", "Peak_L", false));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("PEAK_H", "Peak_H", false));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_L", "Gain L", -18.5f, 18.5f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FREQ_L", "Frequency L", 33.0f, 450.0f, 200.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FREQ_LM", "Frequency LM", 170.0f, 1500.0f, 350.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FREQ_HM", "Frequency HM", 1700.0f, 16000.0f, 3500.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FREQ_H", "Frequency H", 2000.0f, 21000.0f, 4500.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("QFACTOR_L", "QFactor L", 0.2f, 1.9f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_LM", "Gain LM", -18.5f, 18.5f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("QFACTOR_LM", "QFactor LM", 0.2f, 1.9f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_HM", "Gain HM", -18.5f, 18.5f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("QFACTOR_HM", "QFactor HM", 0.2f, 1.9f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_H", "Gain H", -18.5f, 18.5f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("QFACTOR_H", "QFactor H", 0.2f, 1.9f, 1.0f));
    return {parameters.begin(), parameters.end()};
}