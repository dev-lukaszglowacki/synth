#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setSize (500, 460);

    masterEnabledButton.setButtonText ("Master On/Off");
    addAndMakeVisible (masterEnabledButton);
    masterEnabledButton.addListener (this);

    osc1FreqSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    osc1FreqSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (osc1FreqSlider);
    osc1FreqLabel.setText ("Oscillator 1 Frequency", juce::dontSendNotification);
    osc1FreqLabel.setJustificationType (juce::Justification::centred);
    osc1FreqLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (osc1FreqLabel);

    osc2FreqSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    osc2FreqSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (osc2FreqSlider);
    osc2FreqLabel.setText ("Oscillator 2 Frequency", juce::dontSendNotification);
    osc2FreqLabel.setJustificationType (juce::Justification::centred);
    osc2FreqLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (osc2FreqLabel);
    
    oscMixSlider.setSliderStyle (juce::Slider::SliderStyle::Rotary);
    oscMixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 25);
    addAndMakeVisible (oscMixSlider);
    oscMixLabel.setText ("Oscillator Mix", juce::dontSendNotification);
    oscMixLabel.setJustificationType (juce::Justification::centred);
    oscMixLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (oscMixLabel);

    addAndMakeVisible (osc1WaveTypeComboBox);
    osc1WaveTypeComboBox.addItemList ({ "Sine", "Saw", "Square", "Triangle" }, 1);
    osc1WaveTypeLabel.setText ("Oscillator 1 Wave", juce::dontSendNotification);
    osc1WaveTypeLabel.setJustificationType (juce::Justification::centred);
    osc1WaveTypeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (osc1WaveTypeLabel);

    addAndMakeVisible (osc2WaveTypeComboBox);
    osc2WaveTypeComboBox.addItemList ({ "Sine", "Saw", "Square", "Triangle" }, 1);
    osc2WaveTypeLabel.setText ("Oscillator 2 Wave", juce::dontSendNotification);
    osc2WaveTypeLabel.setJustificationType (juce::Justification::centred);
    osc2WaveTypeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (osc2WaveTypeLabel);

    osc1FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::OSC1_FREQ, osc1FreqSlider);
    osc2FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::OSC2_FREQ, osc2FreqSlider);
    oscMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::OSC_MIX, oscMixSlider);
    
    osc1WaveTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (processorRef.apvts, AudioPluginAudioProcessor::OSC1_WAVE, osc1WaveTypeComboBox);
    osc2WaveTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (processorRef.apvts, AudioPluginAudioProcessor::OSC2_WAVE, osc2WaveTypeComboBox);

    addAndMakeVisible (filterTypeComboBox);
    filterTypeComboBox.addItemList ({ "Lowpass", "Bandpass", "Highpass" }, 1);
    filterTypeLabel.setText ("Filter Type", juce::dontSendNotification);
    filterTypeLabel.setJustificationType (juce::Justification::centred);
    filterTypeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (filterTypeLabel);

    filterCutoffSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    filterCutoffSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (filterCutoffSlider);
    filterCutoffLabel.setText ("Filter Cutoff", juce::dontSendNotification);
    filterCutoffLabel.setJustificationType (juce::Justification::centred);
    filterCutoffLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (filterCutoffLabel);

    filterResonanceSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    filterResonanceSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (filterResonanceSlider);
    filterResonanceLabel.setText ("Filter Resonance", juce::dontSendNotification);
    filterResonanceLabel.setJustificationType (juce::Justification::centred);
    filterResonanceLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (filterResonanceLabel);

    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (processorRef.apvts, AudioPluginAudioProcessor::FILTER_TYPE, filterTypeComboBox);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::FILTER_CUTOFF, filterCutoffSlider);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::FILTER_RESONANCE, filterResonanceSlider);

    attackSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    attackSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (attackSlider);
    attackLabel.setText ("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType (juce::Justification::centred);
    attackLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (attackLabel);

    decaySlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    decaySlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (decaySlider);
    decayLabel.setText ("Decay", juce::dontSendNotification);
    decayLabel.setJustificationType (juce::Justification::centred);
    decayLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (decayLabel);

    sustainSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    sustainSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (sustainSlider);
    sustainLabel.setText ("Sustain", juce::dontSendNotification);
    sustainLabel.setJustificationType (juce::Justification::centred);
    sustainLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (sustainLabel);

    releaseSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    releaseSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (releaseSlider);
    releaseLabel.setText ("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType (juce::Justification::centred);
    releaseLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (releaseLabel);

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::ATTACK, attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::DECAY, decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::SUSTAIN, sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::RELEASE, releaseSlider);

    lfoRateSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    lfoRateSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (lfoRateSlider);
    lfoRateLabel.setText ("LFO Rate", juce::dontSendNotification);
    lfoRateLabel.setJustificationType (juce::Justification::centred);
    lfoRateLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (lfoRateLabel);

    lfoDepthSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    lfoDepthSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (lfoDepthSlider);
    lfoDepthLabel.setText ("LFO Depth", juce::dontSendNotification);
    lfoDepthLabel.setJustificationType (juce::Justification::centred);
    lfoDepthLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (lfoDepthLabel);

    masterAlwaysOnButton.setButtonText ("Always On");
    addAndMakeVisible (masterAlwaysOnButton);

    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::LFO_RATE, lfoRateSlider);
    lfoDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, AudioPluginAudioProcessor::LFO_DEPTH, lfoDepthSlider);
    masterAlwaysOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processorRef.apvts, AudioPluginAudioProcessor::MASTER_ALWAYS_ON, masterAlwaysOnButton);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
= default;

void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void AudioPluginAudioProcessorEditor::resized()
{
    masterEnabledButton.setBounds (10, 10, 100, 50);
    
    osc1WaveTypeLabel.setBounds (120, 10, 90, 20);
    osc1WaveTypeComboBox.setBounds (120, 30, 90, 30);

    osc1FreqLabel.setBounds (220, 10, 270, 20);
    osc1FreqSlider.setBounds (220, 30, 270, 30);

    osc2WaveTypeLabel.setBounds (120, 70, 90, 20);
    osc2WaveTypeComboBox.setBounds (120, 90, 90, 30);
    
    osc2FreqLabel.setBounds (220, 70, 270, 20);
    osc2FreqSlider.setBounds (220, 90, 270, 30);
    
    oscMixLabel.setBounds (10, 70, 100, 20);
    oscMixSlider.setBounds (10, 90, 100, 100);

    filterTypeLabel.setBounds (10, 190, 100, 20);
    filterTypeComboBox.setBounds (10, 210, 100, 30);

    filterCutoffLabel.setBounds (120, 190, 370, 20);
    filterCutoffSlider.setBounds (120, 210, 370, 30);

    filterResonanceLabel.setBounds (10, 240, 370, 20);
    filterResonanceSlider.setBounds (10, 260, 370, 30);

    attackLabel.setBounds (10, 290, 120, 20);
    attackSlider.setBounds (10, 310, 120, 30);

    decayLabel.setBounds (130, 290, 120, 20);
    decaySlider.setBounds (130, 310, 120, 30);

    sustainLabel.setBounds (250, 290, 120, 20);
    sustainSlider.setBounds (250, 310, 120, 30);

    releaseLabel.setBounds (370, 290, 120, 20);
    releaseSlider.setBounds (370, 310, 120, 30);

    lfoRateLabel.setBounds (10, 340, 240, 20);
    lfoRateSlider.setBounds (10, 360, 240, 30);

    lfoDepthLabel.setBounds (250, 340, 240, 20);
    lfoDepthSlider.setBounds (250, 360, 240, 30);

    masterAlwaysOnButton.setBounds (10, 400, 100, 50);
}

void AudioPluginAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &masterEnabledButton)
    {
        auto* param = static_cast<juce::AudioParameterBool*> (processorRef.apvts.getParameter (AudioPluginAudioProcessor::MASTER_ENABLED));
        param->setValueNotifyingHost (!param->get());
    }
}
