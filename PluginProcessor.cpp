#include "PluginProcessor.h"
#include "PluginEditor.h"

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
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
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
}

void AudioPluginAudioProcessor::releaseResources()
{
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    auto* masterEnabled = static_cast<juce::AudioParameterBool*> (apvts.getParameter ("MASTER_ENABLED"));
    if (!masterEnabled->get())
        return;

    auto* osc1Freq = static_cast<juce::AudioParameterFloat*> (apvts.getParameter ("OSC1_FREQ"));
    auto* osc2Freq = static_cast<juce::AudioParameterFloat*> (apvts.getParameter ("OSC2_FREQ"));
    auto* oscMix = static_cast<juce::AudioParameterFloat*> (apvts.getParameter ("OSC_MIX"));

    auto osc1AngleDelta = osc1Freq->get() / sampleRate * juce::MathConstants<double>::twoPi;
    auto osc2AngleDelta = osc2Freq->get() / sampleRate * juce::MathConstants<double>::twoPi;
    
    float mixLevel1 = 1.0f - oscMix->get();
    float mixLevel2 = oscMix->get();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        auto osc1Sample = std::sin (osc1Angle);
        auto osc2Sample = std::sin (osc2Angle);
        
        auto currentSample = (osc1Sample * mixLevel1 + osc2Sample * mixLevel2) * 0.15f;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.addSample (channel, sample, currentSample);

        osc1Angle += osc1AngleDelta;
        osc2Angle += osc2AngleDelta;
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterBool>  ("MASTER_ENABLED", "Master Enabled", true));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("OSC1_FREQ", "Oscillator 1 Frequency", 50.0f, 2000.0f, 440.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("OSC2_FREQ", "Oscillator 2 Frequency", 50.0f, 2000.0f, 440.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("OSC_MIX", "Oscillator Mix", 0.0f, 1.0f, 0.5f));

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
