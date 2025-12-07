#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    static const juce::String OSC1_WAVE;
    static const juce::String OSC2_WAVE;
    static const juce::String MASTER_ENABLED;
    static const juce::String OSC1_FREQ;
    static const juce::String OSC2_FREQ;
    static const juce::String OSC_MIX;
    static const juce::String FILTER_TYPE;
    static const juce::String FILTER_CUTOFF;
    static const juce::String FILTER_RESONANCE;
    static const juce::String ATTACK;
    static const juce::String DECAY;
    static const juce::String SUSTAIN;
    static const juce::String RELEASE;
    static const juce::String LFO_RATE;
    static const juce::String LFO_DEPTH;
    static const juce::String MASTER_ALWAYS_ON;

private:
    double sampleRate = 0.0;
    double osc1Angle = 0.0;
    double osc2Angle = 0.0;
    double lfoPhase = 0.0;
    
    std::atomic<float>* osc1WaveType = nullptr;
    std::atomic<float>* osc2WaveType = nullptr;
    
    juce::dsp::StateVariableTPTFilter<float> filter;
    
    std::atomic<float>* filterType = nullptr;
    std::atomic<float>* filterCutoff = nullptr;
    std::atomic<float>* filterResonance = nullptr;

    juce::ADSR adsr;
    std::atomic<float>* attack = nullptr;
    std::atomic<float>* decay = nullptr;
    std::atomic<float>* sustain = nullptr;
    std::atomic<float>* release = nullptr;

    std::atomic<float>* lfoRate = nullptr;
    std::atomic<float>* lfoDepth = nullptr;

    std::atomic<float>* masterAlwaysOn = nullptr;

    bool previousAlwaysOnState = false;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    static double getOscillatorSample (double angle, int waveType);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
