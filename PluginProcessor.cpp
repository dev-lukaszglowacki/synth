#include "PluginProcessor.h"
#include "PluginEditor.h"

const juce::String AudioPluginAudioProcessor::OSC1_WAVE = "OSC1_WAVE";
const juce::String AudioPluginAudioProcessor::OSC2_WAVE = "OSC2_WAVE";
const juce::String AudioPluginAudioProcessor::MASTER_ENABLED = "MASTER_ENABLED";
const juce::String AudioPluginAudioProcessor::OSC1_FREQ = "OSC1_FREQ";
const juce::String AudioPluginAudioProcessor::OSC2_FREQ = "OSC2_FREQ";
const juce::String AudioPluginAudioProcessor::OSC_MIX = "OSC_MIX";
const juce::String AudioPluginAudioProcessor::FILTER_TYPE = "FILTER_TYPE";
const juce::String AudioPluginAudioProcessor::FILTER_CUTOFF = "FILTER_CUTOFF";
const juce::String AudioPluginAudioProcessor::FILTER_RESONANCE = "FILTER_RESONANCE";
const juce::String AudioPluginAudioProcessor::ATTACK = "ATTACK";
const juce::String AudioPluginAudioProcessor::DECAY = "DECAY";
const juce::String AudioPluginAudioProcessor::SUSTAIN = "SUSTAIN";
const juce::String AudioPluginAudioProcessor::RELEASE = "RELEASE";
const juce::String AudioPluginAudioProcessor::LFO_RATE = "LFO_RATE";
const juce::String AudioPluginAudioProcessor::LFO_DEPTH = "LFO_DEPTH";
const juce::String AudioPluginAudioProcessor::MASTER_ALWAYS_ON = "MASTER_ALWAYS_ON";

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts (*this, nullptr, "Parameters", createParameters())
{
    osc1WaveType = apvts.getRawParameterValue (OSC1_WAVE);
    osc2WaveType = apvts.getRawParameterValue (OSC2_WAVE);
    filterType = apvts.getRawParameterValue (FILTER_TYPE);
    filterCutoff = apvts.getRawParameterValue (FILTER_CUTOFF);
    filterResonance = apvts.getRawParameterValue (FILTER_RESONANCE);
    attack = apvts.getRawParameterValue (ATTACK);
    decay = apvts.getRawParameterValue (DECAY);
    sustain = apvts.getRawParameterValue (SUSTAIN);
    release = apvts.getRawParameterValue (RELEASE);
    lfoRate = apvts.getRawParameterValue (LFO_RATE);
    lfoDepth = apvts.getRawParameterValue (LFO_DEPTH);
    masterAlwaysOn = apvts.getRawParameterValue (MASTER_ALWAYS_ON);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
= default;

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
    return true;
}

bool AudioPluginAudioProcessor::producesMidi() const
{
    return false;
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
    return false;
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.sampleRate = newSampleRate;
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());
    
    filter.prepare (spec);
    filter.reset();
    adsr.setSampleRate (newSampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

double AudioPluginAudioProcessor::getOscillatorSample (double angle, int waveType)
{
    switch (waveType)
    {
        case 0: // Sine
            return std::sin (angle);
        case 1: // Saw
            return (angle / juce::MathConstants<double>::pi) - 1.0;
        case 2: // Square
            return angle < juce::MathConstants<double>::pi ? 1.0 : -1.0;
        case 3: // Triangle
            return std::abs (angle / juce::MathConstants<double>::pi - 1.0) * 2.0 - 1.0;
        default:
            return 0.0;
    }
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    // Handle MIDI events to trigger ADSR, or use 'Always On' mode
    bool currentAlwaysOnState = static_cast<bool>(masterAlwaysOn->load());
    if (currentAlwaysOnState && !previousAlwaysOnState)
    {
        adsr.noteOn(); // Trigger note on when 'Always On' is activated
    }
    else if (!currentAlwaysOnState && previousAlwaysOnState)
    {
        adsr.noteOff(); // Trigger note off when 'Always On' is deactivated
    }
    previousAlwaysOnState = currentAlwaysOnState;

    if (!currentAlwaysOnState)
    {
        for (const auto metadata : midiMessages)
        {
            const auto message = metadata.getMessage();
            if (message.isNoteOn())
            {
                adsr.noteOn();
            }
            else if (message.isNoteOff())
            {
                adsr.noteOff();
            }
        }
    }

    auto* masterEnabled = static_cast<juce::AudioParameterBool*> (apvts.getParameter (MASTER_ENABLED));
    if (!masterEnabled->get())
        return;

    // Get ADSR parameter values
    juce::ADSR::Parameters adsrParams;
    adsrParams.attack = attack->load();
    adsrParams.decay = decay->load();
    adsrParams.sustain = sustain->load();
    adsrParams.release = release->load();
    adsr.setParameters (adsrParams);

    auto* osc1Freq = static_cast<juce::AudioParameterFloat*> (apvts.getParameter (OSC1_FREQ));
    auto* osc2Freq = static_cast<juce::AudioParameterFloat*> (apvts.getParameter (OSC2_FREQ));
    auto* oscMix = static_cast<juce::AudioParameterFloat*> (apvts.getParameter (OSC_MIX));

    auto osc1AngleDelta = osc1Freq->get() / sampleRate * juce::MathConstants<double>::twoPi;
    auto osc2AngleDelta = osc2Freq->get() / sampleRate * juce::MathConstants<double>::twoPi;

    float mixLevel1 = 1.0f - oscMix->get();
    float mixLevel2 = oscMix->get();

    // Calculate LFO value
    auto lfoValue = std::sin (lfoPhase);
    lfoPhase += lfoRate->load() / sampleRate * juce::MathConstants<double>::twoPi;
    if (lfoPhase > juce::MathConstants<double>::twoPi)
        lfoPhase -= juce::MathConstants<double>::twoPi;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        auto osc1Sample = AudioPluginAudioProcessor::getOscillatorSample (osc1Angle, static_cast<int> (osc1WaveType->load()));
        auto osc2Sample = AudioPluginAudioProcessor::getOscillatorSample (osc2Angle, static_cast<int> (osc2WaveType->load()));

        auto currentSample = (osc1Sample * mixLevel1 + osc2Sample * mixLevel2) * 0.15f;

        // Apply LFO modulation to filter cutoff
        float modulatedCutoff = filterCutoff->load() + (lfoValue * lfoDepth->load() * filterCutoff->load());
        filter.setCutoffFrequency (std::fmax (20.0f, std::fmin (20000.0f, modulatedCutoff)));
        filter.setResonance (std::max(0.01f, filterResonance->load()));

        switch (static_cast<int> (filterType->load()))
        {
            case 0: filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass); break;
            case 1: filter.setType (juce::dsp::StateVariableTPTFilterType::bandpass); break;
            case 2: filter.setType (juce::dsp::StateVariableTPTFilterType::highpass); break;
        }

        currentSample = filter.processSample (0, currentSample);

        // Apply ADSR envelope gain
        currentSample *= adsr.getNextSample();

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.addSample (channel, sample, currentSample);

        osc1Angle += osc1AngleDelta;
        osc2Angle += osc2AngleDelta;

        if (osc1Angle > juce::MathConstants<double>::twoPi)
            osc1Angle -= juce::MathConstants<double>::twoPi;

        if (osc2Angle > juce::MathConstants<double>::twoPi)
            osc2Angle -= juce::MathConstants<double>::twoPi;
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterChoice> (OSC1_WAVE, "Oscillator 1 Wave", juce::StringArray { "Sine", "Saw", "Square", "Triangle" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (OSC2_WAVE, "Oscillator 2 Wave", juce::StringArray { "Sine", "Saw", "Square", "Triangle" }, 0));

    params.push_back (std::make_unique<juce::AudioParameterBool>  (MASTER_ENABLED, "Master Enabled", true));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (OSC1_FREQ, "Oscillator 1 Frequency", 50.0f, 2000.0f, 440.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (OSC2_FREQ, "Oscillator 2 Frequency", 50.0f, 2000.0f, 440.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (OSC_MIX, "Oscillator Mix", 0.0f, 1.0f, 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (FILTER_TYPE, "Filter Type", juce::StringArray { "Lowpass", "Bandpass", "Highpass" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (FILTER_CUTOFF, "Filter Cutoff", juce::NormalisableRange<float> (20.0f, 20000.0f, 0.2f), 500.0f, juce::String ("Hz"), juce::AudioProcessorParameter::genericParameter, [](float value, int /*maximumStringLength*/) { return juce::String (static_cast<int>(value)); }, [](const juce::String& text) { return text.getFloatValue(); }));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (FILTER_RESONANCE, "Filter Resonance", 0.0f, 1.0f, 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (ATTACK, "Attack", juce::NormalisableRange<float> (0.0f, 5.0f), 0.1f, juce::String ("s"), juce::AudioProcessorParameter::genericParameter, [](float value, int /*maximumStringLength*/) { return juce::String (value, 2); }, [](const juce::String& text) { return text.getFloatValue(); }));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (DECAY, "Decay", juce::NormalisableRange<float> (0.0f, 5.0f), 0.1f, juce::String ("s"), juce::AudioProcessorParameter::genericParameter, [](float value, int /*maximumStringLength*/) { return juce::String (value, 2); }, [](const juce::String& text) { return text.getFloatValue(); }));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (SUSTAIN, "Sustain", juce::NormalisableRange<float> (0.0f, 1.0f), 0.7f, juce::String ("level"), juce::AudioProcessorParameter::genericParameter, [](float value, int /*maximumStringLength*/) { return juce::String (value, 2); }, [](const juce::String& text) { return text.getFloatValue(); }));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (RELEASE, "Release", juce::NormalisableRange<float> (0.0f, 5.0f), 0.1f, juce::String ("s"), juce::AudioProcessorParameter::genericParameter, [](float value, int /*maximumStringLength*/) { return juce::String (value, 2); }, [](const juce::String& text) { return text.getFloatValue(); }));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (LFO_RATE, "LFO Rate", juce::NormalisableRange<float> (0.01f, 20.0f, 0.2f), 0.5f, juce::String ("Hz"), juce::AudioProcessorParameter::genericParameter, [](float value, int /*maximumStringLength*/) { return juce::String (value, 2); }, [](const juce::String& text) { return text.getFloatValue(); }));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (LFO_DEPTH, "LFO Depth", 0.0f, 1.0f, 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterBool> (MASTER_ALWAYS_ON, "Always On", true));

    return { params.begin(), params.end() };
}


//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
