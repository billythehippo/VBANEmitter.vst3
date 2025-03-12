/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//uint16_t nbinputs = 2;
//uint16_t nboutputs = 2;

//==============================================================================
VBANEmitterAudioProcessor::VBANEmitterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::discreteChannels(nbinputs), true) //canonicalChannelSet(nbinputs)
                       .withOutput ("Output", juce::AudioChannelSet::discreteChannels(nboutputs), true) //canonicalChannelSet(nboutputs)
                       ),
        parameters(*this, nullptr, "PARAMETERS",
            {
                std::make_unique<juce::AudioParameterBool>(juce::ParameterID("onoff", 1), "OnOff", false),
                std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("gain", 1), "Gain", 0.0f, 1.0f, 1.0f),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("ip1", 1), "IP1", 0, 255, 127),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("ip2", 1), "IP2", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("ip3", 1), "IP3", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("ip4", 1), "IP4", 0, 255, 1),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("port4", 1), "Port4", 0, 9, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("port3", 1), "Port3", 0, 9, 6),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("port2", 1), "Port2", 0, 9, 9),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("port1", 1), "Port1", 0, 9, 8),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("port0", 1), "Port0", 0, 9, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname01", 1), "Streamname01", 0, 255, 'S'),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname02", 1), "Streamname02", 0, 255, 't'),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname03", 1), "Streamname03", 0, 255, 'r'),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname04", 1), "Streamname04", 0, 255, 'e'),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname05", 1), "Streamname05", 0, 255, 'a'),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname06", 1), "Streamname06", 0, 255, 'm'),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname07", 1), "Streamname07", 0, 255, '1'),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname08", 1), "Streamname08", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname09", 1), "Streamname09", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname10", 1), "Streamname10", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname11", 1), "Streamname11", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname12", 1), "Streamname12", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname13", 1), "Streamname13", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname14", 1), "Streamname14", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname15", 1), "Streamname15", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("streamname16", 1), "Streamname16", 0, 255, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("redundancy", 1), "Redundancy", 0, 4, 0),
                std::make_unique<juce::AudioParameterInt>(juce::ParameterID("format", 1), "Format", 0, 4, 4)
            })
#endif
{
    //txsocket.bindToPort(0);
    txsocket = std::make_unique<juce::DatagramSocket>(true);
    txsocket->bindToPort(0);
    //rxThread = std::make_unique<PlugThread>("VBAN Receiving Thread");
}

VBANEmitterAudioProcessor::~VBANEmitterAudioProcessor()
{
    if (rxThread!= nullptr)
    {
        if (rxThread->isThreadRunning()) rxThread->stopThread(1000);
        rxThread = nullptr;
    }//*/
    if (txsocket!= nullptr)
    {
        txsocket->shutdown();
        txsocket = nullptr;
    }//*/
    if (txbuf!=nullptr)
    {
        free(txbuf);
        txbuf = nullptr;
    }//*/
}

void VBANEmitterAudioProcessor::scanReceptors()
{
    VBanPacket packet;
    char ipBroadCastAddr[16];
    uint8_t ipBytes[4];

    if (parameters.getRawParameterValue("onoff")->load() == false)
    {
        rxThread = std::make_unique<PlugThread>("VBAN Receiving Thread");
        rxThread->start(txsocket.get(), &udpPort, nbinputs, nframes, &receptorInfos, &ips);
        //Fillind request packet
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
        for (int n = 0; n < ips.size(); n++) fprintf(stderr, "IP%d %s:%d\r\n", n+1, ips[n].toString().toRawUTF8(), udpPort);

        for (int n = 0; n < ips.size(); n++)
        {
            sscanf(ips[n].toString().toRawUTF8(), "%hhu.%hhu.%hhu.%hhu", &ipBytes[3], &ipBytes[2], &ipBytes[1], &ipBytes[0]);
            ipBytes[0] = 255;
            memset(ipBroadCastAddr, 0, 16);
            sprintf(ipBroadCastAddr, "%hhu.%hhu.%hhu.%hhu", ipBytes[3], ipBytes[2], ipBytes[1], ipBytes[0]);
            txsocket->write(juce::String(ipBroadCastAddr), udpPort, &packet, VBAN_HEADER_SIZE+strlen(packet.data));
            usleep(200000);
        }
        if (rxThread->isThreadRunning()) rxThread->stopThread(1000);
        rxThread = nullptr;
    }
}

//==============================================================================
const juce::String VBANEmitterAudioProcessor::getName() const
{
    char* pname;
    sprintf(pname, "VBAN Emitter (%d channels)", nbinputs);
    return juce::String(pname);//JucePlugin_Name;
}

const juce::String VBANEmitterAudioProcessor::getIdentifierString() const
{
    char* pname;
    sprintf(pname, "VBAN Emitter (%d channels)", nbinputs);
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    int i;
    
    host_samplerate = getSampleRate();
    nframes = getBlockSize();
    txbuf = (float*)malloc(sizeof(float)*nbinputs*nframes);
    
    vban_packet.header.vban = VBAN_HEADER_FOURC;
    
    for (i=0; i<VBAN_SR_MAXNUMBER; i++)
        if (host_samplerate==VBanSRList[i])
        {
            vban_packet.header.format_SR = i;
            break;
        }

    vban_packet.header.format_nbc = nbinputs - 1;
    tuneTxPackets();
    
    refreshStreamNameTextFromParameters(vban_packet.header.streamname, VBAN_STREAM_NAME_SIZE);
    refreshIPAddressTextFromParameters(ipAddr);
    udpPort = refreshPortTextFromParameters(udpPortTxt);
    
    vban_packet.header.nuFrame = 0;
    
    fprintf(stderr, "samplerate %d nframes %d\r\n", host_samplerate, nframes);
}

void VBANEmitterAudioProcessor::releaseResources()
{
    if (rxThread!= nullptr)
    {
        if (rxThread->isThreadRunning()) rxThread->stopThread(1000);
        rxThread = nullptr;
    }//*/
    if (txsocket!= nullptr)
    {
        txsocket->shutdown();
        txsocket = nullptr;
    }//*/
    if (txbuf!=nullptr)
    {
        free(txbuf);
        txbuf = nullptr;
    }//*/
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VBANEmitterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    //if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
    // && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    //    return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VBANEmitterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    int channel;
    int frame;
    long sr = getSampleRate();

    onoff = parameters.getRawParameterValue("onoff")->load();
    float gain = parameters.getRawParameterValue("gain")->load();
    format = (int)parameters.getRawParameterValue("format")->load();
    redundancy = (int)parameters.getRawParameterValue("redundancy")->load();

    //fprintf(stderr, "onoff %d gain %f\r\n", onoff, gain);

    
    if (host_samplerate!=sr)
    {
        host_samplerate = sr;
        for (int i=0; i<VBAN_SR_MAXNUMBER; i++)
        if (host_samplerate==VBanSRList[i])
        {
            vban_packet.header.format_SR = i;
            break;
        }
    }

    if ((numSamples!=nframes)||(numInputChannels!= nbinputs))
    {
        nframes = numSamples;
        format = parameters.getRawParameterValue("format")->load();
        //fprintf(stderr, "ip %s, port %d, streamname %s, red %d, format %d\r\n", ipAddr, udpPort, vban_packet.header.streamname, redundancy, format);
        
        if (txbuf!=NULL)
        {
            free(txbuf);
            txbuf = (float*)malloc(sizeof(float)*nbinputs*nframes);
        }

        tuneTxPackets();
        vban_packet.header.format_nbc = nbinputs - 1;
        
        refreshStreamNameTextFromParameters(vban_packet.header.streamname, VBAN_STREAM_NAME_SIZE);
        refreshIPAddressTextFromParameters(ipAddr);
        udpPort = refreshPortTextFromParameters(udpPortTxt);
        
        fprintf(stderr, "samplerate %d nframes %d\r\n", host_samplerate, nframes);
    }

    if (format!= vban_packet.header.format_bit)
    {
        vban_packet.header.format_bit = format;
        tuneTxPackets();
    }

    if ((onoffCurrent==false)&&(onoff==true)) // switching on
    {
        onoffCurrent = true;
        refreshStreamNameTextFromParameters(vban_packet.header.streamname, VBAN_STREAM_NAME_SIZE);
        refreshIPAddressTextFromParameters(ipAddr);
        udpPort = refreshPortTextFromParameters(udpPortTxt);
        vban_packet.header.format_bit = format;
        rxThread = std::make_unique<PlugThread>("VBAN Receiving Thread");
        rxThread->start(txsocket.get(), &udpPort, numInputChannels, numSamples, &receptorInfos, &ips);
    }
    if ((onoffCurrent==true)&&(onoff==false)) // switching off
    {
        onoffCurrent = false;
        if (rxThread!= nullptr)
        {
            if (rxThread->isThreadRunning()) rxThread->stopThread(1000);
            rxThread = nullptr;
        }
    }
        
    for (channel = 0; channel < numInputChannels; ++channel)
    {
        inputChannelData[channel] = buffer.getWritePointer(channel);
        if (numInputChannels > numOutputChannels) continue;
        outputChannelData[channel] = buffer.getWritePointer(channel);
        std::copy(inputChannelData[channel], inputChannelData[channel] + nframes, outputChannelData[channel]);  // Копируем входные данные в выходные
    }

    for (frame = 0; frame < nframes; frame++)
    {
        for (channel = 0; channel < numInputChannels; channel++)
        {
            txbuf[frame*numInputChannels+channel] = gain*inputChannelData[channel][frame];
            // TODO calculate average values of channels to indicate
        }
    }
    if (onoff)
    {
        for (int pac = 0; pac < vban_packets_per_block; pac++)
        {
            for (int frame = 0; frame <= (vban_packet.header.format_nbs); frame++)
            {
                vban_sample_convert(&vban_packet.data[frame*(vban_packet.header.format_nbc + 1)*VBanBitResolutionSize[vban_packet.header.format_bit]], vban_packet.header.format_bit, &txbuf[(vban_packet.header.format_nbc + 1)*(pac*(vban_packet.header.format_nbs + 1) + frame)], VBAN_BITFMT_32_FLOAT, nbinputs);
            }
            //fprintf(stderr, "%s\r\n", ipAddr);
            for (int red = 0; red < (1 + redundancy); red++)
            txsocket->write(ipAddr, udpPort, &vban_packet, VBAN_HEADER_SIZE + vban_packet_data_len);
            vban_packet.header.nuFrame++;
        }
    }

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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::ValueTree stateTree = parameters.copyState();
    juce::MemoryOutputStream stream(destData, true);
    stateTree.writeToStream(stream);//*/
}

void VBANEmitterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    if (sizeInBytes > 0)
    {
        juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
        juce::ValueTree stateTree = juce::ValueTree::readFromStream(stream);
        if (stateTree.isValid())
            parameters.replaceState(stateTree);
    }//*/
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VBANEmitterAudioProcessor();
}

void VBANEmitterAudioProcessor::tuneTxPackets()
{
    vban_packet.header.format_bit = format;
    vban_packet_data_len = nbinputs*nframes*VBanBitResolutionSize[format];
    vban_packets_per_block = 1;
    //fprintf(stderr, "format %d sized %d, %d packets with length %d\r\n", format, VBanBitResolutionSize[format], vban_packets_per_block, vban_packet_data_len);
    while((vban_packet_data_len/vban_packets_per_block>VBAN_DATA_MAX_SIZE)||(nframes/vban_packets_per_block>VBAN_SAMPLES_MAX_NB)) vban_packets_per_block*= 2;
    vban_packet_data_len/= vban_packets_per_block;
    vban_packet.header.format_nbs = nframes/vban_packets_per_block - 1;
    //fprintf(stderr, "format %d sized %d, %d packets with length %d\r\n", format, VBanBitResolutionSize[format], vban_packets_per_block, vban_packet_data_len);
}

int VBANEmitterAudioProcessor::refreshStreamNameTextFromParameters(char* text, int textlen)
{
    int* asciiCode;
    uint8_t snu8t[VBAN_STREAM_NAME_SIZE];
    char sname[VBAN_STREAM_NAME_SIZE];
    memset(snu8t, 0, VBAN_STREAM_NAME_SIZE);
    snu8t[0] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname01")->load());
    if (snu8t[0]==0) return 1;
    snu8t[1] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname02")->load());
    snu8t[2] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname03")->load());
    snu8t[3] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname04")->load());
    snu8t[4] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname05")->load());
    snu8t[5] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname06")->load());
    snu8t[6] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname07")->load());
    snu8t[7] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname08")->load());
    snu8t[8] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname09")->load());
    snu8t[9] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname10")->load());
    snu8t[10] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname11")->load());
    snu8t[11] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname12")->load());
    snu8t[12] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname13")->load());
    snu8t[13] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname14")->load());
    snu8t[14] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname15")->load());
    snu8t[15] = static_cast<uint8_t>(parameters.getRawParameterValue("streamname16")->load());

    memcpy(sname, snu8t, VBAN_STREAM_NAME_SIZE);
    memcpy(text, sname, textlen);

    return 0;
}

void VBANEmitterAudioProcessor::refreshStreamNameParametersFromText(const char* text, int textlen)
{
    static uint8_t snu8t[VBAN_STREAM_NAME_SIZE];
    static float asciiCode;

    memset(snu8t, 0, VBAN_STREAM_NAME_SIZE);
    memcpy(snu8t, text, textlen);

    asciiCode = (float)snu8t[0]/255.0f;
    parameters.getParameter("streamname01")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[1]/255.0f;
    parameters.getParameter("streamname02")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[2]/255.0f;
    parameters.getParameter("streamname03")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[3]/255.0f;
    parameters.getParameter("streamname04")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[4]/255.0f;
    parameters.getParameter("streamname05")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[5]/255.0f;
    parameters.getParameter("streamname06")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[6]/255.0f;
    parameters.getParameter("streamname07")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[7]/255.0f;
    parameters.getParameter("streamname08")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[8]/255.0f;
    parameters.getParameter("streamname09")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[9]/255.0f;
    parameters.getParameter("streamname10")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[10]/255.0f;
    parameters.getParameter("streamname11")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[11]/255.0f;
    parameters.getParameter("streamname12")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[12]/255.0f;
    parameters.getParameter("streamname13")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[13]/255.0f;
    parameters.getParameter("streamname14")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[14]/255.0f;
    parameters.getParameter("streamname15")->setValueNotifyingHost(asciiCode);
    asciiCode = (float)snu8t[15]/255.0f;
    parameters.getParameter("streamname16")->setValueNotifyingHost(asciiCode);
}

int VBANEmitterAudioProcessor::refreshIPAddressTextFromParameters(char* text)
{
    char ipaddress[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t ipBytes[4];
    memset(ipaddress, 0, 16);

    ipBytes[0] = static_cast<uint8_t>(parameters.getRawParameterValue("ip1")->load());
    ipBytes[1] = static_cast<uint8_t>(parameters.getRawParameterValue("ip2")->load());
    ipBytes[2] = static_cast<uint8_t>(parameters.getRawParameterValue("ip3")->load());
    ipBytes[3] = static_cast<uint8_t>(parameters.getRawParameterValue("ip4")->load());
    sprintf(ipaddress, "%hhu.%hhu.%hhu.%hhu", ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
    fprintf(stderr, "Ipaddress: %s\r\n", ipaddress);
    memcpy(text, ipaddress, 16);
    return 0;
}

void VBANEmitterAudioProcessor::refreshIPAddressParametersFromText(const char* text)
{
    static uint8_t ipBytes[4];

    sscanf(text, "%hhu.%hhu.%hhu.%hhu", &ipBytes[0], &ipBytes[1], &ipBytes[2], &ipBytes[3]);
    parameters.getParameter("ip1")->setValueNotifyingHost((float)ipBytes[0]/255.0f);
    parameters.getParameter("ip2")->setValueNotifyingHost((float)ipBytes[1]/255.0f);
    parameters.getParameter("ip3")->setValueNotifyingHost((float)ipBytes[2]/255.0f);
    parameters.getParameter("ip4")->setValueNotifyingHost((float)ipBytes[3]/255.0f);
}

int VBANEmitterAudioProcessor::refreshPortTextFromParameters(char* text)
{
    char portText[5] = {0, 0, 0, 0, 0};
    float portDigits[5];
    int len = 0;
    int port = 0;
    int mul = 1;

    portDigits[0] = parameters.getRawParameterValue("port0")->load();
    portDigits[1] = parameters.getRawParameterValue("port1")->load();
    portDigits[2] = parameters.getRawParameterValue("port2")->load();
    portDigits[3] = parameters.getRawParameterValue("port3")->load();
    portDigits[4] = parameters.getRawParameterValue("port4")->load();
    for (int i=0; i<5; i++)
    {
        port+= portDigits[i]*mul;
        mul*= 10;
    }
    if (text!=nullptr)
    {
        sprintf(portText, "%d", port);
        fprintf(stderr, "Port: %d %s\r\n", port, portText);
        memcpy(text, portText, strlen(portText));
    }
    else fprintf(stderr, "Error: Cannot save port as text\r\n");
    return port;
}

int VBANEmitterAudioProcessor::refreshPortParametersFromText(const char* text)
{
    static uint8_t portDigits[5] = {0, 0, 0, 0, 0};
    static int len = 0;
    static int port = 0;
    static int portDiv = 0;

    sscanf(text, "%d", &port);
    portDiv = port;
    for (int i=0; i<5; i++)
    {
        if (portDiv)
        {
            portDigits[i] = portDiv%10;
            portDiv = portDiv/10;
        }
        else portDigits[i] = 0;
    }
    parameters.getParameter("port0")->setValueNotifyingHost((float)portDigits[0]/9.0f);
    parameters.getParameter("port1")->setValueNotifyingHost((float)portDigits[1]/9.0f);
    parameters.getParameter("port2")->setValueNotifyingHost((float)portDigits[2]/9.0f);
    parameters.getParameter("port3")->setValueNotifyingHost((float)portDigits[3]/9.0f);
    parameters.getParameter("port4")->setValueNotifyingHost((float)portDigits[4]/9.0f);

    return port;
}
