#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setSize (500, 200);

    masterEnabledButton.setButtonText ("Master On/Off");
    addAndMakeVisible (masterEnabledButton);
    masterEnabledButton.addListener (this);

    osc1FreqSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    osc1FreqSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (osc1FreqSlider);

    osc2FreqSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    osc2FreqSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 25);
    addAndMakeVisible (osc2FreqSlider);
    
    oscMixSlider.setSliderStyle (juce::Slider::SliderStyle::Rotary);
    oscMixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 25);
    addAndMakeVisible (oscMixSlider);

    osc1FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, "OSC1_FREQ", osc1FreqSlider);
    osc2FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, "OSC2_FREQ", osc2FreqSlider);
    oscMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, "OSC_MIX", oscMixSlider);
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
    
    osc1FreqSlider.setBounds (120, 10, 370, 50);
    osc2FreqSlider.setBounds (120, 70, 370, 50);
    
    oscMixSlider.setBounds (10, 70, 100, 120);
}

void AudioPluginAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &masterEnabledButton)
    {
        auto* param = static_cast<juce::AudioParameterBool*> (processorRef.apvts.getParameter ("MASTER_ENABLED"));
        param->setValueNotifyingHost (!param->get());
    }
}
