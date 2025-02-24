/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "vban_functions.h"

extern uint16_t nbinputs;
extern uint16_t nboutputs;

//==============================================================================
class PlugThread : public juce::Thread
{
public:

    int packetFrameSize;
    juce::StringArray* infostrings;
    VBanPacket rxPacket;
    juce::Array<juce::IPAddress>* localIPAddresses;

    PlugThread(const juce::String& name) : juce::Thread(name)
    {
    }

    void start(juce::DatagramSocket* socket, uint16_t* port, int nbc, int nfr, juce::StringArray* infos, juce::Array<juce::IPAddress>* ips)
    {
        nbchannels = nbc;
        nframes = nfr;
        rxsocket = socket;
        infostrings = infos;
        localIPAddresses = ips;
        //rxsocket->bindToPort(*port);
        startThread();
    }

protected:

    void run() override
    {
        int index;
        int writecnt;
        static VBanPacket infopacket;
        int pktlen;
        juce::String senderIPAddress;
        juce::String infoString;
        int senderPortNumber;

        fprintf(stderr, "RX thread started...\r\n");

        while (!threadShouldExit())
        {
            while (rxsocket->waitUntilReady(true, 0))
            {
                memset(&rxPacket, 0, VBAN_PROTOCOL_MAX_SIZE);
                pktlen = rxsocket->read(&rxPacket, VBAN_PROTOCOL_MAX_SIZE, false, senderIPAddress, senderPortNumber);
                if ((pktlen>VBAN_HEADER_SIZE)&&(rxPacket.header.vban==VBAN_HEADER_FOURC))
                {
                    switch (rxPacket.header.format_SR&VBAN_PROTOCOL_MASK)
                    {
                        case VBAN_PROTOCOL_AUDIO:

                            break;
                        case VBAN_PROTOCOL_SERIAL:
                            // TODO: midi
                            break;
                        case VBAN_PROTOCOL_TXT:
                            // TEXT
                            if (memcmp(rxPacket.header.streamname, "INFO", 4)==0)
                            {
                                for (index=0; index<localIPAddresses->size(); index++)
                                    if ((localIPAddresses->getReference(index).toString()==senderIPAddress)&&(senderIPAddress!= "127.0.0.1")) break;
                                if ((senderIPAddress == "127.0.0.1")||(index==localIPAddresses->size()))
                                {
                                    infoString.clear();
                                    infoString = "ip="+senderIPAddress+" "+juce::String(rxPacket.data);
                                    infostrings->add(infoString);
                                }
                                //TODO TODO TODO
                                /*fprintf(stderr, "Info request from %s:%d\n", senderIPAddress.toRawUTF8(), senderPortNumber);
                                *                       memset(&infopacket, 0, VBAN_PROTOCOL_MAX_SIZE);
                                *                       infopacket.header.vban = VBAN_HEADER_FOURC;
                                *                       infopacket.header.format_SR = VBAN_PROTOCOL_TXT;
                                *                       strcpy(infopacket.header.streamname, "INFO");
                                *                       sprintf(infopacket.data, "VST3_Receptor streamname=%s nboutputs=%d", plugin_rx_context.streamname, nbchannels);
                                *                       rxsocket->write(senderIPAddress, senderPortNumber, &infopacket, VBAN_HEADER_SIZE + strlen(infopacket.data));//*/
                            }
                            break;
                        case VBAN_PROTOCOL_USER:
                            // RAW or other
                            break;
                        default:
                            // non matching packets etc.
                            break;
                    }
                }
            }
        }
        //if (plugin_rx_context.framebuf!= nullptr) free(plugin_rx_context.framebuf);
        fprintf(stderr, "RX thread stopped...\r\n");
    }

  private:

    int nbchannels;
    int nframes;
    juce::DatagramSocket* rxsocket;

    void threadWork()
    {
    }
};


/**
*/
class VBANEmitterAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    VBANEmitterAudioProcessor();
    ~VBANEmitterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================

    int refreshStreamNameTextFromParameters(char* text, int textlen);
    void refreshStreamNameParametersFromText(const char* text, int textlen);
    int refreshIPAddressTextFromParameters(char* text);
    void refreshIPAddressParametersFromText(const char* text);
    int refreshPortTextFromParameters(char* text);
    int refreshPortParametersFromText(const char* text);

    void tuneTxPackets();
    void scanReceptors();

    juce::AudioProcessorValueTreeState parameters;
    juce::DatagramSocket txsocket;
    juce::StringArray receptorInfos;
    VBanPacket vban_packet;

private:
    //==============================================================================
    std::unique_ptr<PlugThread> rxThread;
    juce::Array<juce::IPAddress> ips;

    uint16_t vban_packet_data_len;
    int vban_packets_per_block = 1;
    long host_samplerate;
    int nframes;
    char ipAddr[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char udpPortTxt[5] = {0, 0, 0, 0, 0};
    uint16_t udpPort = 0;
    uint8_t format = 4;
    uint8_t redundancy = 0;
    float* framebuf;
    float* txbuf;
    float** inputChannelData;
    float** outputChannelData;
    bool onoff = false;
    bool onoffCurrent = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VBANEmitterAudioProcessor)
};
