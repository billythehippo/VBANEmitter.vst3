/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <regex>
#include "PluginProcessor.h"
// #include "vban_functions.h"

//==============================================================================
/**
*/
class VBANEmitterAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::ChangeListener

{
public:
    VBANEmitterAudioProcessorEditor (VBANEmitterAudioProcessor&);
    ~VBANEmitterAudioProcessorEditor() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void updateIP();
    void updateSN();
    void updateCNlabel(int num);

    VBANEmitterAudioProcessor& audioProcessor;

private:
    void takeInfo(juce::String info);

    juce::Label labelIP;
    juce::Label labelPort;
    juce::Label labelSN;
    juce::Label labelRed;
    juce::Label labelFmt;
    juce::Label labelChannels;
    juce::TextEditor textEditorIP;
    juce::TextEditor textEditorPort;
    juce::TextEditor textEditorSN;
    juce::Slider sliderNQ;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>nqAttachment;
    juce::TextButton textButtonScan;
    juce::TextButton textButtonGo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>goButtonAttachment;
    juce::Slider sliderFmt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>fmtAttachment;
    juce::ComboBox comboBoxReceptors;
    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;

    juce::TextButton buttonL;
    juce::TextButton buttonR;
    juce::TextEditor infoLabel;
    juce::StringArray infos;
    int infonum = 0;

    bool scanEnabled = false;
    char tempText[16];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VBANEmitterAudioProcessorEditor)
};
