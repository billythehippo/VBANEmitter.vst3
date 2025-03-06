/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VBANEmitterAudioProcessorEditor::VBANEmitterAudioProcessorEditor (VBANEmitterAudioProcessor& p) :
    AudioProcessorEditor (&p),
    audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    char IPAddr[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char udpPortTxt[5] = {0, 0, 0, 0, 0};
    int udpPort;
    char Streamname[VBAN_STREAM_NAME_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    auto* onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
    
    VBanHeader* header = &p.vban_packet.header;
    
    labelIP.setText("IP address", juce::dontSendNotification);
    addAndMakeVisible (labelIP);
    labelIP.setBounds (16, 16, 70, 24);
    
    labelPort.setText("Port", juce::dontSendNotification);
    addAndMakeVisible (labelPort);
    labelPort.setBounds (16, 48, 70, 24);
    
    labelSN.setText("Streamname", juce::dontSendNotification);
    addAndMakeVisible (labelSN);
    labelSN.setBounds (16, 80, 70, 24);
    
    labelRed.setText("Redundancy", juce::dontSendNotification);
    addAndMakeVisible (labelRed);
    labelRed.setBounds (16, 112, 70, 24);
    
    labelFmt.setText("Format", juce::dontSendNotification);
    addAndMakeVisible (labelFmt);
    labelFmt.setBounds (16, 144, 70, 24);

    char channelsnum[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (nboutputs==1) sprintf(channelsnum, "1 channel");
    else sprintf(channelsnum, "%d channels", nboutputs);
    fprintf(stderr, "%s\r\n", channelsnum);
    labelChannels.setText(channelsnum, juce::dontSendNotification);
    labelChannels.setJustificationType (36);
    addAndMakeVisible (labelChannels);
    labelChannels.setBounds (120, 248, 80, 24);

    addAndMakeVisible (textEditorIP);
    audioProcessor.refreshIPAddressTextFromParameters(IPAddr);
    textEditorIP.setText (TRANS (IPAddr));
    textEditorIP.setBounds (120, 16, 100, 24);
    textEditorIP.onTextChange = [this]()
    {
        auto* onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
        std::string ipv4_pattern = R"((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))";
        std::regex ipv4_regex(ipv4_pattern);

        if (*onoffState==false)
        {
            if (std::regex_match(textEditorIP.getText().toStdString(), ipv4_regex))
            {
                fprintf(stderr, "IP %s OK\r\n", (char*)textEditorIP.getText().toRawUTF8());
                audioProcessor.refreshIPAddressParametersFromText((char*)textEditorIP.getText().toRawUTF8());
            }
            else
            {
                fprintf(stderr, "IP invalid or incomplete\r\n");
            }
        }
        else
        {
            memset(tempText, 0, 16);
            audioProcessor.refreshIPAddressTextFromParameters(tempText);
            textEditorIP.setText (TRANS (tempText));
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Warning", "Unable to change IP address while running!", "OK");
        }
    };

    addAndMakeVisible (textEditorPort);
    udpPort = audioProcessor.refreshPortTextFromParameters(udpPortTxt);
    textEditorPort.setText (TRANS (udpPortTxt));
    textEditorPort.setBounds (120, 48, 100, 24);
    textEditorPort.onTextChange = [this]()
    {
        int udpPort;
        auto* onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
        if (*onoffState==false)
        {
            udpPort = audioProcessor.refreshPortParametersFromText((char*)textEditorPort.getText().toRawUTF8());
        }
        else
        {
            memset(tempText, 0, 16);
            udpPort = audioProcessor.refreshPortTextFromParameters(tempText);
            textEditorPort.setText (TRANS (tempText));
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Warning", "Unable to change UDP port while running!", "OK");
        }
    };

    addAndMakeVisible (textEditorSN);
    if (audioProcessor.refreshStreamNameTextFromParameters(Streamname, VBAN_STREAM_NAME_SIZE))
    {
      textEditorSN.clear();
      fprintf(stderr, "Can't refresh Streamname from parameters\r\n");
    }//*/
    else
    {
      textEditorSN.setText (TRANS (Streamname));
      fprintf(stderr, "Refreshing Streamname from parameters\r\n");
    }
    textEditorSN.setBounds (120, 80, 100, 24);
    textEditorSN.onTextChange = [this]()
    {
        auto* onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
        if (*onoffState==false)
        {
          audioProcessor.refreshStreamNameParametersFromText((char*)textEditorSN.getText().toRawUTF8(), strlen(textEditorSN.getText().toRawUTF8()));
        }
        else
        {
            memset(tempText, 0, 16);
            audioProcessor.refreshStreamNameTextFromParameters(tempText, 16);
            textEditorSN.setText (TRANS (tempText));
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Warning", "Unable to change Streamname while running!", "OK");
        }
    };

    addAndMakeVisible (comboBoxNQ);
    comboBoxNQ.addItem ("0", 1);
    comboBoxNQ.addItem ("1", 2);
    comboBoxNQ.addItem ("2", 3);
    comboBoxNQ.addItem ("3", 4);
    comboBoxNQ.addItem ("4", 5);
    comboBoxNQ.setBounds (120, 112, 100, 24);
    comboBoxNQ.setSelectedItemIndex(0);
    comboBoxNQ.setSelectedItemIndex(audioProcessor.parameters.getRawParameterValue("redundancy")->load());
    comboBoxNQ.onChange = [this]()
    {
        float newValue;
        auto* onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
        if (*onoffState==false) // Current state is OFF
        {
            newValue = comboBoxNQ.getSelectedItemIndex();
            audioProcessor.parameters.getParameter("redundancy")->setValueNotifyingHost(newValue/4.0f);
        }
        else
        {
            newValue = audioProcessor.parameters.getRawParameterValue("redundancy")->load();
            comboBoxNQ.setSelectedId((int)newValue);
        }
    };

    addAndMakeVisible (comboBoxFmt);
    comboBoxFmt.setBounds (120, 144, 100, 24);
    comboBoxFmt.setTextWhenNothingSelected("Format");
    comboBoxFmt.addItem ("16I", 1);
    comboBoxFmt.addItem ("24I", 2);
    comboBoxFmt.addItem ("32F", 3);
    comboBoxFmt.setSelectedItemIndex(2);
    //comboBoxFmt.setSelectedItemIndex(audioProcessor.parameters.getRawParameterValue("format")->load());
    comboBoxFmt.onChange = [this]()
    {
        float newValue;
        auto* onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
        if (*onoffState==false) // Current state is OFF
        {
            switch (comboBoxFmt.getSelectedItemIndex())
            {
            case 0:
                audioProcessor.parameters.getParameter("format")->setValueNotifyingHost(1.0f/4.0f);
                break;
            case 1:
                audioProcessor.parameters.getParameter("format")->setValueNotifyingHost(2.0f/4.0f);
                break;
            default:
                audioProcessor.parameters.getParameter("format")->setValueNotifyingHost(4.0f/4.0f);
            }
            //newValue = audioProcessor.parameters.getRawParameterValue("format")->load();
            //fprintf(stderr, "Format %d\r\n", (int)newValue);
        }
        else
        {
            newValue = audioProcessor.parameters.getRawParameterValue("format")->load();
            switch ((int)newValue)
            {
            case 1:
                comboBoxFmt.setSelectedItemIndex(0);
                break;
            case 2:
                comboBoxFmt.setSelectedItemIndex(1);
                break;
            default:
                comboBoxFmt.setSelectedItemIndex(2);
                break;
            }
        }
    };

    addAndMakeVisible (comboBoxReceptors);
    comboBoxReceptors.setBounds (16, 220, 200, 24);
    comboBoxReceptors.setTextWhenNothingSelected("Receptors' info");
    comboBoxReceptors.onChange = [this]()
    {
        char* itemtext;
        char text[16];
        char* ptrs = nullptr;
        char* ptre = nullptr;
        char offset = 0;
        int itemtextlen;

        auto* onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
        if (*onoffState==false)
        {
            if ((comboBoxReceptors.getSelectedItemIndex()>0)&&(comboBoxReceptors.getText()!=comboBoxReceptors.getTextWhenNothingSelected()))
            {
                itemtext = (char*)comboBoxReceptors.getText().toRawUTF8();
                if (itemtext)
                {
                    itemtextlen = strlen(itemtext);
                    ptrs = strstr(itemtext, "ip=");
                    offset = 3;
                    if (ptrs) ptre = strstr(ptrs, " ");
                    if (ptre)//>strstr(ptrs, "=")+1)
                    {
                        //textEditorIP.clear();
                        memset(text, 0, 16);
                        memcpy(text, ptrs+offset, ptre-ptrs-offset);
                        if (strlen(text))
                        {
                            fprintf(stderr, "Destination IP: %s\r\n", text);
                            textEditorIP.setText(text);
                        }
                    }
                    ptrs = nullptr;
                    ptre = nullptr;

                    ptrs = strstr(itemtext, "streamname=");
                    offset = 13;
                    if (ptrs) offset = 11;
                    else ptrs = strstr(itemtext, "streamnamerx=");
                    if (ptrs) ptre = strstr(ptrs, " ");
                    if (ptre)//>strstr(ptrs, "=")+1)
                    {
                        //textEditorSN.clear();
                        memset(text, 0, 16);
                        memcpy(text, ptrs+offset, ptre-ptrs-offset);
                        if (strlen(text))
                        {
                            fprintf(stderr, "Destination Streamname: %s\r\n", text);
                            textEditorSN.setText(text);
                        }
                    }
                    ptrs = nullptr;
                    ptre = nullptr;

                    ptrs = strstr(itemtext, "format=");
                    if (ptrs) ptre = strstr(ptrs, " ");
                    if (ptre)//>strstr(ptrs, "=")+1)
                    {
                        switch ((ptre-1)[0])
                        {
                        case '1':
                            comboBoxFmt.setSelectedId(1);
                            break;
                        case '2':
                            comboBoxFmt.setSelectedId(2);
                            break;
                        default:
                            comboBoxFmt.setSelectedId(3);
                            break;
                        }
                    }
                }
            }
        }
    };

    textButtonScan.setButtonText(TRANS ("SCAN"));
    addAndMakeVisible (textButtonScan);
    textButtonScan.setBounds (16, 184, 70, 24);
    textButtonScan.onClick = [this]()
    {
        auto* onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
        if (*onoffState==false)
        {
            comboBoxReceptors.clear();
            comboBoxReceptors.addItem("Any", 1);
            audioProcessor.scanReceptors();
            usleep(200000);
            comboBoxReceptors.addItemList(audioProcessor.receptorInfos, 2);
            scanEnabled = true;
        }
        //juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "SCAN clicked", "You clicked the button SCAN!", "OK");
    };

    onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
    addAndMakeVisible (textButtonGo);
    if (*onoffState) textButtonGo.setButtonText(TRANS ("STOP"));
    else textButtonGo.setButtonText(TRANS ("GO"));
    textButtonGo.setBounds (135, 184, 70, 24);
    textButtonGo.onClick = [this]()
    {
        auto* onoffState = audioProcessor.parameters.getRawParameterValue("onoff");
        if (onoffState != nullptr)
        {
            if (*onoffState==false) // Current state is OFF
            {
                audioProcessor.parameters.getParameter("onoff")->setValueNotifyingHost(true);
                textButtonGo.setButtonText("STOP");

                //juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "GO clicked", "GO!", "OK");
            }
            else // Current state is ON
            {
                audioProcessor.parameters.getParameter("onoff")->setValueNotifyingHost(false);
                textButtonGo.setButtonText("GO");

                //juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "GO clicked", "STOP!", "OK");
            }
        }
    };

    gainSlider.setSliderStyle(juce::Slider::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(gainSlider);
    gainSlider.setBounds (240, 16, 70, 190);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "gain", gainSlider);

    setSize (320, 280);//(240, 260);
}

VBANEmitterAudioProcessorEditor::~VBANEmitterAudioProcessorEditor()
{
}

//==============================================================================
void VBANEmitterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll (juce::Colour (0xff323e44));

    g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void VBANEmitterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
