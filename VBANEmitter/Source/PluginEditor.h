/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <regex>
#include "PluginProcessor.h"
#include "vban_functions.h"

//==============================================================================
/**
*/
class VBANEmitterAudioProcessorEditor  : public juce::AudioProcessorEditor

{
public:
    VBANEmitterAudioProcessorEditor (VBANEmitterAudioProcessor&);
    ~VBANEmitterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    VBANEmitterAudioProcessor& audioProcessor;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::Label labelIP;
    juce::Label labelPort;
    juce::Label labelSN;
    juce::Label labelRed;
    juce::Label labelFmt;
    juce::Label labelChannels;
    juce::TextEditor textEditorIP;
    juce::TextEditor textEditorPort;
    juce::TextEditor textEditorSN;
    juce::ComboBox comboBoxNQ;
    juce::ComboBox comboBoxFmt;
    juce::ComboBox comboBoxReceptors;
    juce::TextButton textButtonScan;
    juce::TextButton textButtonGo;

    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;

    bool scanEnabled = false;
    char tempText[16];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VBANEmitterAudioProcessorEditor)
};
