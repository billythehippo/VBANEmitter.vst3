/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

uint16_t nbinputs = 2;
uint16_t nboutputs = 2;

//==============================================================================
VBANEmitterAudioProcessor::VBANEmitterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true) //canonicalChannelSet(nbinputs)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true) //canonicalChannelSet(nboutputs)
                     #endif
                       )
#endif
{
}

VBANEmitterAudioProcessor::~VBANEmitterAudioProcessor()
{
}

//==============================================================================
const juce::String VBANEmitterAudioProcessor::getName() const
{
    return JucePlugin_Name;
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
    framebuf = (float*)malloc(sizeof(float)*nbinputs);
    txbuf = (float*)malloc(sizeof(float)*nbinputs*nframes);
    inputChannelData = (float**)malloc(nbinputs*sizeof(float*));
    outputChannelData = (float**)malloc(nboutputs*sizeof(float*));
    
    strcpy(tx_context.ipaddr, "127.0.0.1");
    tx_context.port = 6980;
    strcpy(tx_context.streamname, "Stream1");
    tx_context.red = 0;
    tx_context.format = 4;
    
    plugin_tx_context = tx_context;
    
    vban_packet.header.vban = 0;//VBAN_HEADER_FOURC;
    
    for (i=0; i<VBAN_SR_MAXNUMBER; i++)
        if (host_samplerate==VBanSRList[i])
        {
            vban_packet.header.format_SR = i;
            break;
        }
    vban_packet.header.format_bit = VBAN_BITFMT_32_FLOAT;
    
    vban_packet.header.format_nbc = nbinputs - 1;
    
    vban_packet_data_len = nbinputs*nframes*sizeof(float);
    while((vban_packet_data_len/vban_packets_per_block>VBAN_DATA_MAX_SIZE)||(nframes/vban_packets_per_block>VBAN_SAMPLES_MAX_NB)) vban_packets_per_block*= 2;
    vban_packet_data_len/= vban_packets_per_block;
    vban_packet.header.format_nbs = nframes/vban_packets_per_block - 1;
    
    memset(vban_packet.header.streamname, 0, 16);
    strcpy(vban_packet.header.streamname, tx_context.streamname);
    
    vban_packet.header.nuFrame = 0;
    
    fprintf(stderr, "samplerate %d nframes %d\r\n", host_samplerate, nframes);
    
    ip_dst = new juce::IPAddress(tx_context.ipaddr);
}

void VBANEmitterAudioProcessor::releaseResources()
{
    if (framebuf!=NULL)
    {
        free(framebuf);
        framebuf = NULL;
    }
    if (txbuf!=NULL)
    {
        free(txbuf);
        txbuf = NULL;
    }
    if (inputChannelData!=NULL)
    {
        free(inputChannelData);
        inputChannelData = NULL;
    }
    if (outputChannelData!=NULL)
    {
        free(outputChannelData);
        outputChannelData = NULL;
    }
    
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
    int channel;
    int frame;
    long sr = getSampleRate();
    
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
    
    const int numSamples = buffer.getNumSamples();
    if ((numSamples!=nframes)||(memcmp(&tx_context, &plugin_tx_context, sizeof(vban_plugin_tx_context_t))!=0))
    {
        nframes = numSamples;
        memcpy(&tx_context, &plugin_tx_context, sizeof(vban_plugin_tx_context_t));
        fprintf(stderr, "ip %s, port %d, streamname %s, red %d, format %d\r\n", tx_context.ipaddr, tx_context.port, tx_context.streamname, tx_context.red, tx_context.format);
        
        if (framebuf!=NULL)
        {
            free(framebuf);
            framebuf = NULL;
        }
        framebuf = (float*)malloc(sizeof(float)*nbinputs);
        if (txbuf!=NULL)
        {
            free(txbuf);
            txbuf = NULL;
        }
        txbuf = (float*)malloc(sizeof(float)*nbinputs*nframes);

        vban_packet_data_len = nbinputs*nframes*VBanBitResolutionSize[tx_context.format];
        while((vban_packet_data_len/vban_packets_per_block>VBAN_DATA_MAX_SIZE)||(nframes/vban_packets_per_block>VBAN_SAMPLES_MAX_NB)) vban_packets_per_block*= 2;
        vban_packet_data_len/= vban_packets_per_block;
        vban_packet.header.format_nbs = nframes/vban_packets_per_block - 1;
        vban_packet.header.format_bit = tx_context.format;
        
        memcpy(vban_packet.header.streamname, tx_context.streamname, 16);
        
        fprintf(stderr, "samplerate %d nframes %d\r\n", host_samplerate, nframes);
    }
    
    if(strcmp(tx_context.ipaddr, ip_dst->toString().toRawUTF8())!=0)
    {
        delete ip_dst;
        ip_dst = new juce::IPAddress(tx_context.ipaddr);
    }
        
    for (channel = 0; channel < numInputChannels; ++channel)
    {
        inputChannelData[channel] = buffer.getWritePointer(channel);
        
        if (numInputChannels > numOutputChannels)
            continue;

        outputChannelData[channel] = buffer.getWritePointer(channel);
        std::copy(inputChannelData[channel], inputChannelData[channel] + nframes, outputChannelData[channel]);  // Копируем входные данные в выходные
    }
    for (frame = 0; frame < nframes; frame++)
    {
        for (channel = 0; channel < numInputChannels; channel++)
        {
            txbuf[frame*numInputChannels+channel] = inputChannelData[channel][frame];
        }
    }
    for (int pac = 0; pac < vban_packets_per_block; pac++)
    {
        for (int frame = 0; frame <= (vban_packet.header.format_nbs); frame++)
        {
            vban_sample_convert(&vban_packet.data[frame*(vban_packet.header.format_nbc + 1)*VBanBitResolutionSize[vban_packet.header.format_bit]], vban_packet.header.format_bit, &txbuf[(vban_packet.header.format_nbc + 1)*(pac*(vban_packet.header.format_nbs + 1) + frame)], VBAN_BITFMT_32_FLOAT, nbinputs);
        }
        if (vban_packet.header.vban==VBAN_HEADER_FOURC)
        {
            for (int red = 0; red < (1 + tx_context.red); red++)
                txsocket.write(tx_context.ipaddr, tx_context.port, &vban_packet, VBAN_HEADER_SIZE + vban_packet_data_len);
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
}

void VBANEmitterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VBANEmitterAudioProcessor();
}
