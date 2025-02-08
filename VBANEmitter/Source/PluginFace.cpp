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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "PluginFace.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PluginFace::PluginFace ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    juce__labelIP.reset (new juce::Label ("new label",
                                          TRANS ("IP address")));
    addAndMakeVisible (juce__labelIP.get());
    juce__labelIP->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__labelIP->setJustificationType (juce::Justification::centredLeft);
    juce__labelIP->setEditable (false, false, false);
    juce__labelIP->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__labelIP->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__labelIP->setBounds (16, 16, 102, 24);

    juce__labelPort.reset (new juce::Label ("new label",
                                            TRANS ("Port")));
    addAndMakeVisible (juce__labelPort.get());
    juce__labelPort->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__labelPort->setJustificationType (juce::Justification::centredLeft);
    juce__labelPort->setEditable (false, false, false);
    juce__labelPort->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__labelPort->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__labelPort->setBounds (16, 48, 102, 24);

    juce__labelSN.reset (new juce::Label ("new label",
                                          TRANS ("Streamname")));
    addAndMakeVisible (juce__labelSN.get());
    juce__labelSN->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__labelSN->setJustificationType (juce::Justification::centredLeft);
    juce__labelSN->setEditable (false, false, false);
    juce__labelSN->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__labelSN->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__labelSN->setBounds (16, 80, 102, 24);

    juce__textEditorIP.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (juce__textEditorIP.get());
    juce__textEditorIP->setMultiLine (false);
    juce__textEditorIP->setReturnKeyStartsNewLine (false);
    juce__textEditorIP->setReadOnly (false);
    juce__textEditorIP->setScrollbarsShown (true);
    juce__textEditorIP->setCaretVisible (true);
    juce__textEditorIP->setPopupMenuEnabled (true);
    juce__textEditorIP->setText (TRANS ("127.0.0.1"));

    juce__textEditorIP->setBounds (120, 16, 150, 24);

    juce__textEditorPort.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (juce__textEditorPort.get());
    juce__textEditorPort->setMultiLine (false);
    juce__textEditorPort->setReturnKeyStartsNewLine (false);
    juce__textEditorPort->setReadOnly (false);
    juce__textEditorPort->setScrollbarsShown (true);
    juce__textEditorPort->setCaretVisible (true);
    juce__textEditorPort->setPopupMenuEnabled (true);
    juce__textEditorPort->setText (TRANS ("6980"));

    juce__textEditorPort->setBounds (120, 48, 150, 24);

    juce__textEditorSN.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (juce__textEditorSN.get());
    juce__textEditorSN->setMultiLine (false);
    juce__textEditorSN->setReturnKeyStartsNewLine (false);
    juce__textEditorSN->setReadOnly (false);
    juce__textEditorSN->setScrollbarsShown (true);
    juce__textEditorSN->setCaretVisible (true);
    juce__textEditorSN->setPopupMenuEnabled (true);
    juce__textEditorSN->setText (TRANS ("Stream1"));

    juce__textEditorSN->setBounds (120, 80, 150, 24);

    juce__comboBoxNQ.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (juce__comboBoxNQ.get());
    juce__comboBoxNQ->setExplicitFocusOrder (2);
    juce__comboBoxNQ->setEditableText (false);
    juce__comboBoxNQ->setJustificationType (juce::Justification::centredLeft);
    juce__comboBoxNQ->setTextWhenNothingSelected (TRANS ("0"));
    juce__comboBoxNQ->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    juce__comboBoxNQ->addItem (TRANS ("0"), 1);
    juce__comboBoxNQ->addItem (TRANS ("1"), 2);
    juce__comboBoxNQ->addItem (TRANS ("2"), 3);
    juce__comboBoxNQ->addItem (TRANS ("3"), 4);
    juce__comboBoxNQ->addItem (TRANS ("4"), 5);
    juce__comboBoxNQ->addListener (this);

    juce__comboBoxNQ->setBounds (24, 152, 111, 24);

    juce__comboBoxFmt.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (juce__comboBoxFmt.get());
    juce__comboBoxFmt->setExplicitFocusOrder (1);
    juce__comboBoxFmt->setEditableText (false);
    juce__comboBoxFmt->setJustificationType (juce::Justification::centredLeft);
    juce__comboBoxFmt->setTextWhenNothingSelected (TRANS ("32F"));
    juce__comboBoxFmt->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    juce__comboBoxFmt->addItem (TRANS ("16I"), 1);
    juce__comboBoxFmt->addItem (TRANS ("24I"), 2);
    juce__comboBoxFmt->addItem (TRANS ("32F"), 3);
    juce__comboBoxFmt->addListener (this);

    juce__comboBoxFmt->setBounds (152, 152, 111, 24);

    juce__textButtonGo.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (juce__textButtonGo.get());
    juce__textButtonGo->setButtonText (TRANS ("Go"));
    juce__textButtonGo->addListener (this);

    juce__textButtonGo->setBounds (72, 192, 142, 24);

    juce__labelRed.reset (new juce::Label ("new label",
                                           TRANS ("Redundancy")));
    addAndMakeVisible (juce__labelRed.get());
    juce__labelRed->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__labelRed->setJustificationType (juce::Justification::centredLeft);
    juce__labelRed->setEditable (false, false, false);
    juce__labelRed->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__labelRed->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__labelRed->setBounds (32, 120, 94, 24);

    juce__labelFmt.reset (new juce::Label ("new label",
                                           TRANS ("Format")));
    addAndMakeVisible (juce__labelFmt.get());
    juce__labelFmt->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__labelFmt->setJustificationType (juce::Justification::centredLeft);
    juce__labelFmt->setEditable (false, false, false);
    juce__labelFmt->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__labelFmt->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__labelFmt->setBounds (176, 120, 54, 24);


    //[UserPreSize]
    setSize (500, 250);
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    juce__comboBoxNQ->setSelectedItemIndex(0);
    juce__comboBoxFmt->setSelectedItemIndex(2);
    //[/Constructor]
}

PluginFace::~PluginFace()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    juce__labelIP = nullptr;
    juce__labelPort = nullptr;
    juce__labelSN = nullptr;
    juce__textEditorIP = nullptr;
    juce__textEditorPort = nullptr;
    juce__textEditorSN = nullptr;
    juce__comboBoxNQ = nullptr;
    juce__comboBoxFmt = nullptr;
    juce__textButtonGo = nullptr;
    juce__labelRed = nullptr;
    juce__labelFmt = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PluginFace::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PluginFace::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PluginFace::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == juce__comboBoxNQ.get())
    {
        //[UserComboBoxCode_juce__comboBoxNQ] -- add your combo box handling code here..
        //[/UserComboBoxCode_juce__comboBoxNQ]
    }
    else if (comboBoxThatHasChanged == juce__comboBoxFmt.get())
    {
        //[UserComboBoxCode_juce__comboBoxFmt] -- add your combo box handling code here..
        //[/UserComboBoxCode_juce__comboBoxFmt]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void PluginFace::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == juce__textButtonGo.get())
    {
        //[UserButtonCode_juce__textButtonGo] -- add your button handler code here..
        if (vban_header->vban==VBAN_HEADER_FOURC)
        {
            vban_header->vban = 0;
            buttonThatWasClicked->setButtonText("GO");
        }
        else
        {
            memset(plugin_tx_context.ipaddr, 0, 16);
            strcpy(plugin_tx_context.ipaddr, juce__textEditorIP->getText().toRawUTF8());
            plugin_tx_context.port = (uint16_t)juce__textEditorPort->getText().getIntValue();
            memset(plugin_tx_context.streamname, 0, 16);
            strcpy(plugin_tx_context.streamname, juce__textEditorSN->getText().toRawUTF8());
            plugin_tx_context.red = juce__comboBoxNQ->getSelectedItemIndex();
            plugin_tx_context.format = 1 + juce__comboBoxFmt->getSelectedItemIndex();
            if (plugin_tx_context.format == 3) plugin_tx_context.format++;
            fprintf(stderr, "ip %s, port %d, streamname %s, red %d, format %d\r\n", plugin_tx_context.ipaddr, plugin_tx_context.port, plugin_tx_context.streamname, plugin_tx_context.red, plugin_tx_context.format);
            vban_header->vban = VBAN_HEADER_FOURC;
            buttonThatWasClicked->setButtonText("STOP");
        }
        //[/UserButtonCode_juce__textButtonGo]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void PluginFace::getContext()
{
    juce::String str;
    juce__textEditorIP->setText(plugin_tx_context.ipaddr);
    juce__textEditorSN->setText(plugin_tx_context.streamname);
    str = juce::String::formatted("%d", plugin_tx_context.port);
    juce__textEditorPort->setText(str);
    switch (plugin_tx_context.format)
    {
    case 1:
        juce__comboBoxFmt->setSelectedItemIndex(0);
        break;
    case 2:
        juce__comboBoxFmt->setSelectedItemIndex(1);
        break;
    case 4:
        juce__comboBoxFmt->setSelectedItemIndex(2);
        break;
    default:
        plugin_tx_context.format = 4;
        juce__comboBoxFmt->setSelectedItemIndex(2);
        break;
    }
    juce__comboBoxNQ->setSelectedItemIndex(plugin_tx_context.red);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PluginFace" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <LABEL name="new label" id="2e9191ddd2e8e9f4" memberName="juce__labelIP"
         virtualName="" explicitFocusOrder="0" pos="16 16 102 24" edTextCol="ff000000"
         edBkgCol="0" labelText="IP address" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="f75a008fdfd70c88" memberName="juce__labelPort"
         virtualName="" explicitFocusOrder="0" pos="16 48 102 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Port" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="12a70c376008c3a" memberName="juce__labelSN"
         virtualName="" explicitFocusOrder="0" pos="16 80 102 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Streamname" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="new text editor" id="da3206fb06a3a0db" memberName="juce__textEditorIP"
              virtualName="" explicitFocusOrder="0" pos="120 16 150 24" initialText="127.0.0.1"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTEDITOR name="new text editor" id="b8c7c903ba1705dc" memberName="juce__textEditorPort"
              virtualName="" explicitFocusOrder="0" pos="120 48 150 24" initialText="6980"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTEDITOR name="new text editor" id="f27a4568f3ddda13" memberName="juce__textEditorSN"
              virtualName="" explicitFocusOrder="0" pos="120 80 150 24" initialText="Stream1"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <COMBOBOX name="new combo box" id="828b3e57fe83e3cf" memberName="juce__comboBoxNQ"
            virtualName="" explicitFocusOrder="2" pos="24 152 111 24" editable="0"
            layout="33" items="0&#10;1&#10;2&#10;3&#10;4" textWhenNonSelected="0"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="new combo box" id="d3784c21e61aabca" memberName="juce__comboBoxFmt"
            virtualName="" explicitFocusOrder="1" pos="152 152 111 24" editable="0"
            layout="33" items="16I&#10;24I&#10;32F" textWhenNonSelected="32F"
            textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="new button" id="93add1b2de6fe4a9" memberName="juce__textButtonGo"
              virtualName="" explicitFocusOrder="0" pos="72 192 142 24" buttonText="Go"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="3d207f6a77571d10" memberName="juce__labelRed"
         virtualName="" explicitFocusOrder="0" pos="32 120 94 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Redundancy" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="42328d1d944787f4" memberName="juce__labelFmt"
         virtualName="" explicitFocusOrder="0" pos="176 120 54 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Format" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

