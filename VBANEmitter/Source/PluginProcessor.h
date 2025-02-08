/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include <juce_core/juce_core.h>
//#include <juce_networking/network/juce_NetworkSocketHandler.h>
//#include <juce_networking/network/juce_UDPPacketReceiver.h>
//#include <juce_networking/network/juce_IPAddress.h>
#include "PluginFace.h"
#include "vban_functions.h"

//==============================================================================
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
    juce::IPAddress* ip_dst;
    juce::DatagramSocket txsocket;
    VBanPacket vban_packet;

private:
    //==============================================================================
    vban_plugin_tx_context_t tx_context;
    uint16_t vban_packet_data_len;
    int vban_packets_per_block = 1;
    long host_samplerate;
    int nframes;
    float* framebuf;
    float* txbuf;
    float** inputChannelData;
    float** outputChannelData;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VBANEmitterAudioProcessor)
};
