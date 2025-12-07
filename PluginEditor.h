#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                          public juce::Button::Listener
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked (juce::Button* button) override;

private:
    AudioPluginAudioProcessor& processorRef;

    juce::TextButton masterEnabledButton;
    juce::Slider osc1FreqSlider;
    juce::Label osc1FreqLabel;
    juce::Slider osc2FreqSlider;
    juce::Label osc2FreqLabel;
    juce::Slider oscMixSlider;
    juce::Label oscMixLabel;
    
    juce::ComboBox osc1WaveTypeComboBox;
    juce::Label osc1WaveTypeLabel;
    juce::ComboBox osc2WaveTypeComboBox;
    juce::Label osc2WaveTypeLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1FreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2FreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscMixAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc1WaveTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc2WaveTypeAttachment;

    juce::ComboBox filterTypeComboBox;
    juce::Label filterTypeLabel;
    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;
    juce::Slider filterResonanceSlider;
    juce::Label filterResonanceLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;

    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider decaySlider;
    juce::Label decayLabel;
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    juce::Slider lfoRateSlider;
    juce::Label lfoRateLabel;
    juce::Slider lfoDepthSlider;
    juce::Label lfoDepthLabel;
    juce::TextButton masterAlwaysOnButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> masterAlwaysOnAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
