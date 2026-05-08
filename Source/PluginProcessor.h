/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <cstdint>
#include "vban.h"

#if __has_include("ChannelsConfig.h")
    #include "ChannelsConfig.h"
#endif

#if JucePlugin_Build_AUv3
#ifdef NUMBER_OF_CHANNELS
#undef NUMBER_OF_CHANNELS
#endif
#define NUMBER_OF_CHANNELS 2
#else
#ifndef NUMBER_OF_CHANNELS
#define NUMBER_OF_CHANNELS 2
#endif
#endif

//==============================================================================
class VBANEmitterAudioProcessor  :  public juce::AudioProcessor,
                                    public juce::AudioProcessorValueTreeState::Listener,
                                    public juce::Thread,
                                    public juce::ChangeBroadcaster
{
public:
    //==============================================================================
    VBANEmitterAudioProcessor();
    ~VBANEmitterAudioProcessor() override;

    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void updateIP(juce::IPAddress newAddr, bool notifyUI = true);
    void updateStreamName(const juce::String& newSN, bool notifyUI = true);
    juce::AudioProcessorValueTreeState apvts;

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
    const juce::String getIdentifierString() const;// override;
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
    int scanReceptors();
    int getNBC();

    juce::StringArray receptorInfos;
    VBanPacket txPacket;
    juce::IPAddress ipAddr;
    uint16_t udpPort = 6980;
    uint8_t vbanFormats[3] = { VBAN_BITFMT_16_INT, VBAN_BITFMT_24_INT, VBAN_BITFMT_32_FLOAT };

private:
    //==============================================================================
    void floatToVban(char* dst, const float** src, int nframes, int nchannels, uint8_t format);
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void run() override;
    std::unique_ptr<juce::DatagramSocket> txSocket;
    juce::Array<juce::IPAddress> ips;
    uint16_t vbanPacketDataLen;
    uint16_t vbanPacketNFrames = 0;
    uint16_t vbanPacketsPerBlock = 1;
    uint32_t hostSamplerate = 0;
    uint32_t hostNframes = 0;
    uint32_t hostNBChannels = 0;
    std::vector<float> txBuffer;
    std::vector<const float*> inputChannelData;
    std::vector<float*> outputChannelData;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VBANEmitterAudioProcessor)
};
