/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.12

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "vban_functions.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PluginFace  : public juce::Component,
                    public juce::ComboBox::Listener,
                    public juce::Button::Listener
{
public:
    //==============================================================================
    PluginFace ();
    ~PluginFace() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    VBanHeader* vban_header;
    void getContext();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> juce__labelIP;
    std::unique_ptr<juce::Label> juce__labelPort;
    std::unique_ptr<juce::Label> juce__labelSN;
    std::unique_ptr<juce::TextEditor> juce__textEditorIP;
    std::unique_ptr<juce::TextEditor> juce__textEditorPort;
    std::unique_ptr<juce::TextEditor> juce__textEditorSN;
    std::unique_ptr<juce::ComboBox> juce__comboBoxNQ;
    std::unique_ptr<juce::ComboBox> juce__comboBoxFmt;
    std::unique_ptr<juce::TextButton> juce__textButtonGo;
    std::unique_ptr<juce::Label> juce__labelRed;
    std::unique_ptr<juce::Label> juce__labelFmt;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginFace)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

