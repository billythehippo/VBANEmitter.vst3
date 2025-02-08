/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginFace.h"
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

    void face_get_context();
    VBANEmitterAudioProcessor& audioProcessor;
    PluginFace face;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VBANEmitterAudioProcessorEditor)
};
