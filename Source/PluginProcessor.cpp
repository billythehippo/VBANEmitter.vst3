/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "vban.h"
#include <cstdint>
#include <cstdio>

//==============================================================================
VBANEmitterAudioProcessor::VBANEmitterAudioProcessor()
     : AudioProcessor (BusesProperties()
                       //.withInput  ("Input",  juce::AudioChannelSet::stereo(), true) //discreteChannels(nbinputs)
                       //.withOutput("Output",  juce::AudioChannelSet::stereo(), true) //canonicalChannelSet(nboutputs)
                        .withInput  ("Input",  juce::AudioChannelSet::canonicalChannelSet(2), true)
                        .withOutput ("Output", juce::AudioChannelSet::canonicalChannelSet(2), true)
                       ),
    apvts(*this, nullptr, "Parameters", createParameterLayout()),
    juce::Thread("rxThread")
{
    //txSocket = std::make_unique<juce::DatagramSocket>(true);
    //txSocket->bindToPort(0);

    apvts.addParameterListener("onoff", this);
    apvts.addParameterListener ("format", this);
    for (int i = 0; i < 16; i++) apvts.addParameterListener("streamname" + juce::String::formatted("%02d", i + 1), this);
    for (int i = 0; i < 5; i++)  apvts.addParameterListener("port" + juce::String(i), this);
    for (int i = 0; i < 4; i++)  apvts.addParameterListener("ip" + juce::String(i + 1), this);

    //startThread();
}

VBANEmitterAudioProcessor::~VBANEmitterAudioProcessor()
{
    //stopThread(1000);
    txSocket = nullptr;
    apvts.removeParameterListener("onoff", this);
    apvts.removeParameterListener ("format", this);
    for (int i = 0; i < 16; i++) apvts.removeParameterListener("streamname" + juce::String::formatted("%02d", i + 1), this);
    for (int i = 0; i < 5; i++)  apvts.removeParameterListener("port" + juce::String(i), this);
    for (int i = 0; i < 4; i++)  apvts.removeParameterListener("ip" + juce::String(i + 1), this);
}

juce::AudioProcessorValueTreeState::ParameterLayout VBANEmitterAudioProcessor::createParameterLayout()
{
    uint8_t ipbytes[4] = {127, 0, 0, 1};
    uint8_t portdigits[5] = {0, 6, 9, 8, 0};
    memset(txPacket.header.streamname, 0, 16);
    strncpy(txPacket.header.streamname, "Stream1", 7);
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("onoff", 1), "OnOff", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("gain", 1), "Gain", 0.0f, 1.0f, 1.0f));
    for (int i = 0; i < 4; i++)
    {
        auto paramID = "ip" + juce::String(i + 1);
        auto paramNm = "IP" + juce::String(i + 1);
        layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID(paramID, 1), paramNm, 0, 255, ipbytes[i]));
    }
    for (int i = 0; i < 5; i++)
    {
        auto paramID = "port" + juce::String(i);
        auto paramNm = "Port" + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID(paramID, 1), paramNm, 0, 9, portdigits[i]));
    }
    for (int i = 0; i < 16; i++)
    {
        auto paramID = "streamname" + juce::String::formatted("%02d", i + 1, 2);
        auto paramNm = "Streamname" + juce::String::formatted("%02d", i + 1, 2);
        layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID(paramID, 1), paramNm, 0, 255, txPacket.header.streamname[i]));
    }
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("redundancy", 1), "Redundancy", 0, 4, 1));
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("format", 1),
        "Format",
        0, 2, 2,
        juce::AudioParameterIntAttributes().withStringFromValueFunction([](int value, int maxLength)
            {
                switch (value)
                {
                    case 0:  return "16I";
                    case 1:  return "24I";
                    case 2:  return "32F";
                    default: return "Unknown";
                }
            })
    ));
    return layout;
}

void VBANEmitterAudioProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == "onoff")
    {
        bool isOn = apvts.getRawParameterValue("onoff")->load() > 0.5f;
        if (isOn)
        {
            if (txSocket == nullptr) txSocket = std::make_unique<juce::DatagramSocket>(true);
            if (!txSocket->bindToPort(0))
            {
                txSocket = nullptr;
                DBG ("Failed to bind socket");
            }
            else
            {
                //startThread();
                fprintf(stderr, "Starting...\r\n");
            }
        }
        else
        {
            //stopThread(500);
            txSocket = nullptr;
        }
    }
    else if (parameterID.startsWith("port"))
    {
        juce::String fullPortString = "";
        for (int i = 0; i < 5; i++)
        {
            int digit = (int)*apvts.getRawParameterValue("port" + juce::String(i));
            fullPortString += juce::String(digit);
        }
        int finalPort = fullPortString.getIntValue();
        udpPort = juce::jlimit(0, 65535, finalPort); // Ограничиваем валидным диапазоном TCP/UDP портов
        fprintf(stderr, "New Port: %d\r\n", udpPort);
    }
    else if (parameterID.startsWith("streamname"))
    {
        int index = parameterID.substring(10).getTrailingIntValue() - 1;
        if (index >= 0 && index < 16)
        {
            txPacket.header.streamname[index] = static_cast<char>(std::round(newValue));
        }
    }
    else if (parameterID.startsWith("ip"))
    {
         ipAddr = juce::IPAddress((uint8_t)*apvts.getRawParameterValue("ip1"),
                                  (uint8_t)*apvts.getRawParameterValue("ip2"),
                                  (uint8_t)*apvts.getRawParameterValue("ip3"),
                                  (uint8_t)*apvts.getRawParameterValue("ip4"));
    }
    else if (parameterID == "format")
    {
        uint8_t fmt = (int)*apvts.getRawParameterValue("format");
        txPacket.header.format_bit = (fmt == 2 ? VBAN_BITFMT_32_FLOAT : fmt + 1);
    }
}

void VBANEmitterAudioProcessor::updateIP(juce::IPAddress newAddr, bool notifyUI)
{
    for (int i = 0; i < 4; i++)
    {
        auto paramID = "ip" + juce::String(i + 1);
        if (auto* param = apvts.getParameter(paramID))
        {
            float byteVal = (float)newAddr.address[i];
            float normalized = param->getNormalisableRange().convertTo0to1(byteVal);
            if (std::abs(param->getValue() - normalized) > 0.0001f) param->setValueNotifyingHost(normalized);
        }
    }
    ipAddr = newAddr;
    if (hasEditor()&&notifyUI) sendChangeMessage();
}

void VBANEmitterAudioProcessor::updateStreamName(const juce::String& newSN, bool notifyUI)
{
    for (int i = 0; i < 16; i++)
    {
        auto paramID = "streamname" + juce::String::formatted ("%02d", i + 1);
        if (auto* param = apvts.getParameter (paramID))
        {
            auto& range = param->getNormalisableRange();
            float charVal = (i < newSN.length()) ? (float)(uint8_t)newSN[i] : 0.0f;
            float normalized = range.convertTo0to1 (charVal);
            if (std::abs (param->getValue() - normalized) > 0.0001f) param->setValueNotifyingHost (normalized);
            txPacket.header.streamname[i] = static_cast<char> (range.convertFrom0to1 (normalized));
        }
    }
    if (hasEditor()&&notifyUI) sendChangeMessage();
}

int VBANEmitterAudioProcessor::scanReceptors()
{
    VBanPacket packet;
    juce::IPAddress ipBroadCastAddr;
    VBanPacket rxPacket;
    juce::String senderIPAddress;
    juce::String infoString;
    int senderPort;
    int pktlen = 0;
    int got = 0;

    if (apvts.getRawParameterValue("onoff")->load() < 0.5f)
    {
        txSocket = std::make_unique<juce::DatagramSocket>(true);
        txSocket->bindToPort(0);
        //startThread();
        memset(&packet, 0, VBAN_PROTOCOL_MAX_SIZE);
        packet.header.vban = VBAN_HEADER_FOURC;
        packet.header.format_SR = VBAN_PROTOCOL_TXT;
        packet.header.format_bit = 0;
        packet.header.format_nbc = 0;
        packet.header.format_nbs = 0;
        strcpy(packet.header.streamname, "INFO");
        packet.header.nuFrame = 0;
        strcpy(packet.data, "/info");
        receptorInfos.clear();
        ips.clear();
        juce::IPAddress::findAllAddresses (ips);
        for (int n = 0; n < ips.size(); n++)
        {
            ipBroadCastAddr = ips[n];
            ipBroadCastAddr.address[3] = 255;
            txSocket->write(ipBroadCastAddr.toString(), udpPort, &packet, VBAN_HEADER_SIZE+strlen(packet.data));
            usleep(10000);
            if (txSocket->waitUntilReady(true, 10) == 1)
            {
                memset(&rxPacket, 0, VBAN_PROTOCOL_MAX_SIZE);
                pktlen = txSocket->read(&rxPacket, VBAN_PROTOCOL_MAX_SIZE, false, senderIPAddress, senderPort);
                if ((pktlen>VBAN_HEADER_SIZE)&&(rxPacket.header.vban==VBAN_HEADER_FOURC)&&((rxPacket.header.format_SR&VBAN_PROTOCOL_MASK)==VBAN_PROTOCOL_TXT))
                {
                    if ((memcmp(rxPacket.header.streamname, "INFO", 4)==0)&&(pktlen > 28))
                    {
                        fprintf(stderr, "%s\r\n", rxPacket.data);
                        infoString = juce::String("ip=" + senderIPAddress + " ") + juce::String(rxPacket.data);
                        receptorInfos.add(infoString);
                        got++;
                    }
                }
            }
        }
        //stopThread(500);
        txSocket = nullptr;
        sendChangeMessage();
    }
    return got;
}

//==============================================================================
const juce::String VBANEmitterAudioProcessor::getName() const
{
    char* pname;
    return juce::String(pname);//JucePlugin_Name;
}

const juce::String VBANEmitterAudioProcessor::getIdentifierString() const
{
    char* pname;
    return juce::String(pname);
}

bool VBANEmitterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VBANEmitterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VBANEmitterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VBANEmitterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VBANEmitterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VBANEmitterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VBANEmitterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VBANEmitterAudioProcessor::getProgramName (int index)
{
    return {};
}

void VBANEmitterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VBANEmitterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    uint8_t format = apvts.getRawParameterValue("format")->load();
    uint8_t vbanFormat = vbanFormats[format];
    hostSamplerate = (uint32_t)sampleRate;
    hostNframes = samplesPerBlock;
    hostNBChannels = getTotalNumInputChannels();
    //txBuffer.resize(hostNBChannels * hostNframes);
    //txBuffer.assign(hostNBChannels * hostNframes, 0.0f);
    inputChannelData.resize(hostNBChannels);
    outputChannelData.resize(getTotalNumOutputChannels());

    fprintf(stderr, "samplerate %ld nframes %d, nbchannels %d\r\n", hostSamplerate, hostNframes, hostNBChannels);

    vbanPacketsPerBlock = 1;
    vbanPacketNFrames = hostNframes;
    while ((vbanPacketNFrames > 256)||(vbanPacketNFrames * hostNBChannels * VBanBitResolutionSize[vbanFormat] > VBAN_DATA_MAX_SIZE))
    {
        vbanPacketNFrames = vbanPacketNFrames >> 1; // divide to 2
        vbanPacketsPerBlock = vbanPacketsPerBlock << 1; // multiple with 2
    }

    fprintf(stderr, "packets %ld nframes per packet %d\r\n", vbanPacketsPerBlock, vbanPacketNFrames);

    vbanPacketDataLen = VBanBitResolutionSize[vbanFormat] * vbanPacketNFrames * hostNBChannels;
    txPacket.header.vban = VBAN_HEADER_FOURC;
    for (int i = 0; i < VBAN_SR_MAXNUMBER; i++) if (hostSamplerate==VBanSRList[i])
    {
        txPacket.header.format_SR = i;
        break;
    }
    txPacket.header.format_nbc = hostNBChannels - 1;
    txPacket.header.format_nbs = vbanPacketNFrames - 1;
    txPacket.header.format_bit = vbanFormat;
    txPacket.header.nuFrame = 0;
    memset(txPacket.data, 0, VBAN_DATA_MAX_SIZE);

    fprintf(stderr, "Packet prepared\r\n");

    if (hasEditor()) sendChangeMessage();

    fprintf(stderr, "Editor refreshed\r\n");
}

void VBANEmitterAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VBANEmitterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    auto mainIn  = layouts.getMainInputChannelSet();
    auto mainOut = layouts.getMainOutputChannelSet();
    if (mainIn != mainOut) return false;
    int n = mainIn.size();
    //return (n >= 1 && n <= 64);
    if (n!= NUMBER_OF_CHANNELS) return false;
    return true;
}
#endif

void VBANEmitterAudioProcessor::run()
{
    int i;
    VBanPacket rxPacket;
    juce::String senderIPAddress;
    juce::String infoString;
    int senderPort;
    int pktlen = 0;

    while (!threadShouldExit())
    {
        if ((txSocket!= nullptr)&&(txSocket->getBoundPort() > 0))
        {
            while (txSocket->waitUntilReady(true, 10))
            {
                memset(&rxPacket, 0, VBAN_PROTOCOL_MAX_SIZE);
                pktlen = txSocket->read(&rxPacket, VBAN_PROTOCOL_MAX_SIZE, false, senderIPAddress, senderPort);
                if ((pktlen>VBAN_HEADER_SIZE)&&(rxPacket.header.vban==VBAN_HEADER_FOURC))
                {
                    switch (rxPacket.header.format_SR&VBAN_PROTOCOL_MASK)
                    {
                    case VBAN_PROTOCOL_AUDIO:
                        break;
                    case VBAN_PROTOCOL_SERIAL: // TODO: midi
                        break;
                    case VBAN_PROTOCOL_TXT: // TEXT
                        if ((memcmp(rxPacket.header.streamname, "INFO", 4)==0)&&(pktlen > 28))
                        {
                            fprintf(stderr, "%s\r\n", rxPacket.data);
                            infoString = juce::String("ip=" + senderIPAddress + " ") + juce::String(rxPacket.data);
                            receptorInfos.add(infoString);
                            // for (i = 0; i < ips.size(); i++)
                            //     if ((ips.getReference(i).toString() == senderIPAddress)&&(senderIPAddress != "127.0.0.1")) break;
                            // if ((senderIPAddress == "127.0.0.1")||(i == (int)ips.size()))
                            // {
                            //     infoString.clear();
                            //     infoString = "ip = " + senderIPAddress + " " + juce::String(rxPacket.data);
                            //     receptorInfos.add(infoString);
                            // }
                            //TODO TODO TODO
                            /*fprintf(stderr, "Info request from %s:%d\n", senderIPAddress.toRawUTF8(), senderPort);
                            *                       memset(&infopacket, 0, VBAN_PROTOCOL_MAX_SIZE);
                            *                       infopacket.header.vban = VBAN_HEADER_FOURC;
                            *                       infopacket.header.format_SR = VBAN_PROTOCOL_TXT;
                            *                       strcpy(infopacket.header.streamname, "INFO");
                            *                       sprintf(infopacket.data, "VST3_Receptor streamname=%s nboutputs=%d", plugin_rx_context.streamname, nbchannels);
                            *                       socket->write(senderIPAddress, senderPort, &infopacket, VBAN_HEADER_SIZE + strlen(infopacket.data));//*/
                        }
                        break;
                    case VBAN_PROTOCOL_USER: // RAW or other
                        break;
                    default: // non matching packets etc
                        break;
                    }
                }
            }
        }
        else Thread::sleep(50);
    }
}

void VBANEmitterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    juce::ScopedNoDenormals noDenormals;
    bool onoff = apvts.getRawParameterValue("onoff")->load() > 0.5f;
    //if (!onoff || txSocket == nullptr) return;

    for (int channel = totalNumInputChannels; channel < totalNumOutputChannels; channel++)
        buffer.clear (channel, 0, buffer.getNumSamples());

    if (onoff && txSocket != nullptr && txSocket->getRawSocketHandle() >= 0 && txSocket->getBoundPort() > 0)
    {
        int nbchannels = buffer.getNumChannels();
        int nframes = buffer.getNumSamples();
        for (int channel = 0; channel < nbchannels; channel++) inputChannelData[channel] = buffer.getReadPointer(channel);
        const float* shifted_src[256];

        for (int pac = 0; pac < vbanPacketsPerBlock; pac++)
        {
            for (int channel = 0; channel < nbchannels; channel++) shifted_src[channel] = inputChannelData[channel] +  pac * vbanPacketNFrames;
            floatToVban(reinterpret_cast<char*>(txPacket.data), shifted_src, vbanPacketNFrames, nbchannels, txPacket.header.format_bit);
            for (int red = 0; red <= (int)apvts.getRawParameterValue("redundancy")->load(); red++)
                txSocket->write(ipAddr.toString(), udpPort, &txPacket, VBAN_HEADER_SIZE + vbanPacketDataLen);
            txPacket.header.nuFrame++;
        }
        //fprintf(stderr, "data sent\r\n", onoff);
    }
    // else fprintf(stderr, "ONOFF state %d\r\n", onoff);
}


int VBANEmitterAudioProcessor::getNBC()
{
    return hostNBChannels;
}

//==============================================================================
bool VBANEmitterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VBANEmitterAudioProcessor::createEditor()
{
    return new VBANEmitterAudioProcessorEditor (*this);
}

//==============================================================================
void VBANEmitterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree stateTree = apvts.copyState();
    juce::MemoryOutputStream stream(destData, true);
    stateTree.writeToStream(stream);//*/
}

void VBANEmitterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (sizeInBytes > 0)
    {
        apvts.getParameter("onoff")->setValueNotifyingHost(0.0);
        juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
        juce::ValueTree stateTree = juce::ValueTree::readFromStream(stream);
        if (stateTree.isValid()) apvts.replaceState(stateTree);
    }
}

//==============================================================================
void VBANEmitterAudioProcessor::floatToVban(char* dst, const float** src, int nframes, int nchannels, uint8_t format)
{
    switch (format & VBAN_BIT_RESOLUTION_MASK)
    {
        case VBAN_BITFMT_16_INT:
        {
            int16_t* out16 = reinterpret_cast<int16_t*>(dst);
            for (int f = 0; f < nframes; ++f)
            {
                for (int c = 0; c < nchannels; ++c)
                {
                    float v = src[c][f] * 32767.0f;
                    if (v > 32767.0f) v = 32767.0f;
                    else if (v < -32768.0f) v = -32768.0f;
                    out16[f * nchannels + c] = static_cast<int16_t>(v);
                }
            }
            break;
        }
        case VBAN_BITFMT_24_INT:
        {
            uint8_t* out24 = reinterpret_cast<uint8_t*>(dst);
            for (int f = 0; f < nframes; ++f)
            {
                for (int c = 0; c < nchannels; ++c)
                {
                    float v = src[c][f] * 8388607.0f;
                    if (v > 8388607.0f) v = 8388607.0f;
                    else if (v < -8388608.0f) v = -8388608.0f;
                    int32_t v32 = static_cast<int32_t>(v);
                    int idx = (f * nchannels + c) * 3;
                    out24[idx + 0] = static_cast<uint8_t>(v32 & 0xFF);
                    out24[idx + 1] = static_cast<uint8_t>((v32 >> 8) & 0xFF);
                    out24[idx + 2] = static_cast<uint8_t>((v32 >> 16) & 0xFF);
                }
            }
            break;
        }
        case VBAN_BITFMT_32_FLOAT:
        {
            float* outf = reinterpret_cast<float*>(dst);
            for (int f = 0; f < nframes; ++f)
            {
                for (int c = 0; c < nchannels; ++c)
                {
                    outf[f * nchannels + c] = src[c][f];
                }
            }
            break;
        }
    }
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VBANEmitterAudioProcessor();
}
