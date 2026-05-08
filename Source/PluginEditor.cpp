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

    labelChannels.setText(juce::String(audioProcessor.getNBC()) + " channels", juce::dontSendNotification);
    labelChannels.setJustificationType (36);
    addAndMakeVisible (labelChannels);
    labelChannels.setBounds (120, 228, 80, 24);

    textEditorIP.setBounds (120, 16, 100, 24);
    textEditorIP.setInputRestrictions (15, "0123456789.");
    updateIP();
    textEditorIP.onTextChange = [this]()
    {
        juce::IPAddress targetIP (textEditorIP.getText());
        if (targetIP != juce::IPAddress::any() || textEditorIP.getText() == "0.0.0.0")
            audioProcessor.updateIP(targetIP);
    };
    addAndMakeVisible (textEditorIP);

    textEditorPort.setBounds (120, 48, 100, 24);
    textEditorPort.setInputRestrictions (5, "0123456789");
    juce::String currentPort = "";
    for (int i = 0; i < 5; i++)
    {
        int digit = (int)*audioProcessor.apvts.getRawParameterValue("port" + juce::String(i));
        currentPort += juce::String(digit);
    }
    textEditorPort.setText(juce::String(currentPort.getIntValue()), juce::dontSendNotification);
    textEditorPort.onTextChange = [this]()
    {
        juce::String text = textEditorPort.getText();
        juce::String paddedText = text.paddedLeft('0', 5);
        for (int i = 0; i < 5; i++)
        {
            auto paramID = "port" + juce::String(i);
            if (auto* param = audioProcessor.apvts.getParameter(paramID))
            {
                float digitValue = (float)(paddedText[i] - '0');
                float normalized = param->getNormalisableRange().convertTo0to1(digitValue);
                if (std::abs(param->getValue() - normalized) > 0.001f)
                    param->setValueNotifyingHost(normalized);
            }
        }
    };
    addAndMakeVisible (textEditorPort);

    textEditorSN.setBounds (120, 80, 100, 24);
    updateSN();
    textEditorSN.onTextChange = [this]()
    {
        auto text = textEditorSN.getText();
        for (int i = 0; i < 16; i++)
        {
            auto paramID = "streamname" + juce::String::formatted("%02d", i + 1);
            if (auto* param = audioProcessor.apvts.getParameter(paramID))
            {
                float newValue = (i < text.length()) ? (float)(uint8_t)text[i] : 0.0f;
                if (std::abs(param->getValue() - param->getNormalisableRange().convertTo0to1(newValue)) > 0.001f)
                {
                    param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(newValue));
                }
            }
        }
    };
    addAndMakeVisible (textEditorSN);

    sliderNQ.setSliderStyle(juce::Slider::LinearHorizontal);
    sliderNQ.setTextBoxStyle(juce::Slider::TextBoxRight, false, 30, 20);
    sliderNQ.setBounds(120, 112, 100, 24);
    addAndMakeVisible(sliderNQ);
    nqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "redundancy", sliderNQ);

    sliderFmt.setSliderStyle(juce::Slider::LinearHorizontal);
    sliderFmt.setTextBoxStyle(juce::Slider::TextBoxRight, false, 30, 20);
    sliderFmt.setBounds(120, 144, 100, 24);
    addAndMakeVisible(sliderFmt);
    fmtAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "format", sliderFmt);

    comboBoxReceptors.setBounds(16, 220, 285, 24);
    comboBoxReceptors.setTextWhenNothingSelected("Receptors' info");
    comboBoxReceptors.onChange = [this]()
    {
        takeInfo(infos[infonum]);
    };
    //addAndMakeVisible (comboBoxReceptors);
#if JUCE_IOS
    comboBoxReceptors.setNativePicker (true);
    // comboBoxReceptors.getLookAndFeel().setUsingNativeNativeMenus(false);  // 1. Говорим: "Рисуй меню сам, не проси систему"
    // comboBoxReceptors.setExplicitMenuWindowFlags(juce::ComponentPeer::windowAppearsOnTaskbar); // 2. Самый важный костыль для AUv3: прибиваем меню гвоздями к окну плагина
    // comboBoxReceptors.setScrollWheelEnabled(false);  // 3. Чтобы юзер случайно не прокрутил список, просто задев его пальцем
#endif

    buttonL.setBounds(16, 220, 25, 48);
    buttonL.setButtonText("<");
    buttonL.onClick = [this]()
    {
        if ((infonum == 0)||((infonum > infos.size() - 1))) infonum = infos.size();
        else infonum--;
        infoLabel.setText(infos[infonum], juce::dontSendNotification);
        takeInfo(infos[infonum]);
    };
    addChildComponent(buttonL);

    buttonR.setBounds(274, 220, 25, 48);
    buttonR.setButtonText(">");
    buttonR.onClick = [this]()
    {
        if (infonum >= infos.size() - 1) infonum = 0;
        else infonum++;
        infoLabel.setText(infos[infonum], juce::dontSendNotification);
        takeInfo(infos[infonum]);
    };
    addChildComponent(buttonR);

    infoLabel.setBounds(45, 220, 226, 48);
    infoLabel.setMultiLine(true);
    infoLabel.setEnabled(false);
    //infoLabel.setIndents(2, 2);
    addChildComponent(infoLabel);

    textButtonScan.setButtonText(TRANS ("SCAN"));
    addAndMakeVisible(textButtonScan);
    textButtonScan.setBounds (16, 184, 70, 24);
    textButtonScan.onClick = [this]()
    {
        int got;
        auto* onoffState = audioProcessor.apvts.getRawParameterValue("onoff");
        if (*onoffState==false)
        {
            comboBoxReceptors.clear();
            infos.clear();
            if (got = audioProcessor.scanReceptors() > 0)
            {
                usleep(20000);
                scanEnabled = true;
                setSize (320, 300);
                infoLabel.setVisible(true);
                buttonL.setVisible(true);
                buttonR.setVisible(true);
                labelChannels.setBounds (120, 268, 80, 24);
                infoLabel.setText(juce::String(got) + " receptor" + juce::String(got%10==1 ? " " : "s ") + "found!\nPress arrows to select");
            }
        }
    };

    textButtonGo.setButtonText("GO");
    textButtonGo.setBounds (135, 184, 70, 24);
    textButtonGo.setClickingTogglesState(true);
    goButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "onoff", textButtonGo);
    textButtonGo.onStateChange = [this]()
    {
        bool isOn = textButtonGo.getToggleState();
        if (isOn)
        {
            textEditorIP.setEnabled(false);
            textEditorPort.setEnabled(false);
            textEditorSN.setEnabled(false);
            sliderFmt.setEnabled(false);
            sliderNQ.setEnabled(false);
            textButtonGo.setButtonText("STOP");
        }
        else
        {
            textEditorIP.setEnabled(true);
            textEditorPort.setEnabled(true);
            textEditorSN.setEnabled(true);
            sliderFmt.setEnabled(true);
            sliderNQ.setEnabled(true);
            textButtonGo.setButtonText("GO");
        }
    };
    textButtonGo.onStateChange();
    addAndMakeVisible (textButtonGo);

    gainSlider.setBounds (240, 16, 70, 190);
    gainSlider.setSliderStyle(juce::Slider::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    gainSlider.setSkewFactor(0.005);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gain", gainSlider);
    addAndMakeVisible(gainSlider);

    audioProcessor.addChangeListener(this);

    setSize (320, 260);//(240, 260);
}

VBANEmitterAudioProcessorEditor::~VBANEmitterAudioProcessorEditor()
{
    audioProcessor.removeChangeListener(this);
}

void VBANEmitterAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    fprintf(stderr, "Refreshing ui...\r\n");
    updateCNlabel(audioProcessor.getNBC());
    comboBoxReceptors.clear();
    comboBoxReceptors.addItemList(audioProcessor.receptorInfos, 1);
    infos = audioProcessor.receptorInfos;
}

//==============================================================================
void VBANEmitterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll (juce::Colour (0xff323e44));
    g.setColour (juce::Colours::white);
}

void VBANEmitterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void VBANEmitterAudioProcessorEditor::takeInfo(juce::String info)
{
    juce::StringArray pairs;
    pairs.addTokens (info, " ", "");

    juce::StringPairArray params;
    for (auto& p : pairs) params.set (p.upToFirstOccurrenceOf ("=", false, false),p.fromFirstOccurrenceOf ("=", false, false));

    juce::String name = params.containsKey ("streamnamerx") ? params["streamnamerx"] : params["servername"];
    juce::String addr = params["ip"];
    int format = params["format"].getIntValue();
    int busyClients = params["max_clients"].getIntValue() - params["free_clients"].getIntValue();

    textEditorIP.setText(addr, juce::dontSendNotification);
    juce::IPAddress ipaddr(addr);
    audioProcessor.updateIP(ipaddr, false);
    if (params.containsKey ("streamnamerx"))
    {
        textEditorSN.setText(name, juce::dontSendNotification);
        audioProcessor.updateStreamName(name, false);
    }
    fprintf(stderr, "%d.%d.%d.%d\r\n", ipaddr.address[0],  ipaddr.address[1], ipaddr.address[2], ipaddr.address[3]);
    // DBG ("--- Receptor Info ---");
    // DBG ("Name: " << name);
    // DBG ("Address: " << addr);
    // DBG ("Format: " << (format==1 ? "16bit" : (format==2 ? "24bit" : "32bit")));
    // DBG ("Busy Clients: " << busyClients);
}

void VBANEmitterAudioProcessorEditor::updateIP()
{
    juce::String currentIP;
    for (int i = 0; i < 4; i++)
    {
        auto paramID = "ip" + juce::String(i + 1);
        int oct = (int)*audioProcessor.apvts.getRawParameterValue(paramID);
        currentIP += juce::String(oct);
        if (i < 3) currentIP += ".";
    }
    textEditorIP.setText(currentIP, juce::dontSendNotification);
}

void VBANEmitterAudioProcessorEditor::updateSN()
{
    juce::String currentName = "";
    for (int i = 0; i < 16; i++)
    {
        auto paramID = "streamname" + juce::String::formatted("%02d", i + 1);
        float val01 = audioProcessor.apvts.getParameter(paramID)->getValue();
        int ascii = (int)std::round(val01 * 255.0f);
        if (ascii > 0 && ascii <= 255) currentName += (char)ascii;
    }
    textEditorSN.setText(currentName, juce::dontSendNotification);
}

void VBANEmitterAudioProcessorEditor::updateCNlabel(int num)
{
    if (num == 1) labelChannels.setText("1 channel", juce::dontSendNotification);
    else labelChannels.setText(juce::String(num) + " channels", juce::dontSendNotification);
}

